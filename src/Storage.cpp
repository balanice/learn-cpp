#include "Storage.h"
#include "MyCrypt2.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>

using namespace std::string_literals;

std::string Storage::bytesToHex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    return oss.str();
}

std::string Storage::bytesToHex(const std::vector<unsigned char>& v) {
    return bytesToHex(v.data(), v.size());
}

// Base64 encode helper using OpenSSL EVP_EncodeBlock
static std::string base64Encode(const unsigned char* data, size_t len) {
    if (len == 0) return std::string();
    size_t outlen = 4 * ((len + 2) / 3);
    std::vector<unsigned char> out(outlen + 1); // +1 for safety
    int olen = EVP_EncodeBlock(out.data(), data, static_cast<int>(len));
    if (olen < 0) return std::string();
    return std::string(reinterpret_cast<char*>(out.data()), static_cast<size_t>(olen));
}

static std::string base64Encode(const std::vector<unsigned char>& v) {
    return base64Encode(v.data(), v.size());
}

Storage::Storage(const Config& cfg)
    : encrypt_(cfg.isEncrypted()), storagePath_(cfg.getStoragePath())
{
    // ensure directory exists
    try {
        std::filesystem::create_directories(storagePath_);
    } catch (const std::exception& e) {
        spdlog::error("Failed to create storage directory {}: {}", storagePath_, e.what());
    }

    if (encrypt_) {
        // Initialize KeyManager with a seed file under storage path and get/create work key
        std::string seedFile = std::filesystem::path(storagePath_) / "seed.bin";
        std::string workKeyFile = std::filesystem::path(storagePath_) / "work_key.bin";
        try {
            auto &km = KeyManager::Instance();
            km.Initialize(seedFile, 32, 100000);
            workKey_ = km.GetOrCreateWorkKey(workKeyFile, 32);
            spdlog::info("Storage: encryption enabled. Work key length: {}", workKey_.size());
            // provide work key to MessageWorker for automatic decryption in reports
            worker_.setWorkKey(workKey_);
        } catch (const std::exception &e) {
            spdlog::error("Failed to initialize KeyManager: {}", e.what());
            encrypt_ = false; // fallback to plain
        }
    }
}

void Storage::store(const std::string& key, const std::string& value) {
    if (!encrypt_) {
        WriteMessage m{-1, key, value};
        worker_.submit(m);
        return;
    }

    // Encrypt value with AES-GCM using workKey_
    try {
        // prepare key and iv
        std::string keyStr(reinterpret_cast<const char*>(workKey_.data()), workKey_.size());
        auto ivVec = GenerateRandomBytes(12); // 96-bit IV
        std::string ivStr(reinterpret_cast<const char*>(ivVec.data()), ivVec.size());

        std::vector<unsigned char> cipher(value.size() + 16);
        unsigned char tag[16] = {0};
        int cipherLen = gcm_encrypt(value, keyStr, ivStr, cipher.data(), tag);
        if (cipherLen <= 0) {
            spdlog::error("Encryption failed for key {}", key);
            return;
        }

        // assemble stored string as: ENC:iv_b64:tag_b64:cipher_b64
        std::string ivB64 = base64Encode(ivVec);
        std::string tagB64 = base64Encode(tag, 16);
        std::string cipherB64 = base64Encode(cipher.data(), static_cast<size_t>(cipherLen));
        std::string stored = "ENC:" + ivB64 + ":" + tagB64 + ":" + cipherB64;

        WriteMessage m{-1, key, stored};
        worker_.submit(m);
    } catch (const std::exception &e) {
        spdlog::error("Encryption error for key {}: {}", key, e.what());
    }
}
