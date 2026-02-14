#include "KeyManager.h"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/kdf.h>

#include <spdlog/spdlog.h>

static std::string toHex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex;
    for (size_t i = 0; i < len; ++i) {
        oss << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return oss.str();
}

static std::vector<unsigned char> GenerateRandomBytes(size_t length) {
    std::vector<unsigned char> buf(length);
    if (1 != RAND_bytes(buf.data(), (int)length)) {
        throw std::runtime_error("RAND_bytes failed");
    }
    return buf;
}

KeyManager::KeyManager() {}
KeyManager::~KeyManager() {}

KeyManager& KeyManager::Instance() {
    static KeyManager instance;
    return instance;
}

void KeyManager::Initialize(const std::string& seedFilePath, size_t rootKeyLenBytes, size_t pbkdf2Iterations) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (initialized_) return;

    seedFilePath_ = seedFilePath;
    rootKeyLenBytes_ = rootKeyLenBytes;
    pbkdf2Iterations_ = pbkdf2Iterations;

    loadSeedIfExists();

    if (seed_.empty()) {
        // generate 16 bytes seed by default
        seed_ = GenerateRandomBytes(16);
        persistSeed();
        spdlog::info("Generated new seed and persisted to {}", seedFilePath_);
    } else {
        spdlog::info("Loaded seed from {}", seedFilePath_);
    }

    // derive root key now
    rootKey_.assign(rootKeyLenBytes_, 0);
    int res = PKCS5_PBKDF2_HMAC(
        reinterpret_cast<const char*>(seed_.data()), (int)seed_.size(),
        seed_.data(), (int)seed_.size(),
        (int)pbkdf2Iterations_,
        EVP_sha256(), (int)rootKeyLenBytes_, rootKey_.data());

    if (res != 1) {
        throw std::runtime_error("PBKDF2 derivation failed in Initialize");
    }

    spdlog::info("Initialized KeyManager: rootKeyLen={} iterations={}", rootKeyLenBytes_, pbkdf2Iterations_);

    initialized_ = true;
}

void KeyManager::ensureInitialized() {
    if (!initialized_) throw std::runtime_error("KeyManager not initialized. Call Initialize() first.");
}

void KeyManager::persistSeed() {
    std::ofstream out(seedFilePath_, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open seed file for writing: " + seedFilePath_);
    }
    // store raw bytes
    out.write(reinterpret_cast<const char*>(seed_.data()), (std::streamsize)seed_.size());
    out.close();
}

void KeyManager::loadSeedIfExists() {
    std::ifstream in(seedFilePath_, std::ios::binary);
    if (!in.is_open()) return; // no existing seed

    std::vector<unsigned char> buf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (!buf.empty()) seed_ = std::move(buf);
}

std::string KeyManager::GetRootKeyHex() {
    std::lock_guard<std::mutex> lk(mtx_);
    ensureInitialized();
    return toHex(rootKey_.data(), rootKey_.size());
}

std::vector<unsigned char> KeyManager::GetRootKeyRaw() {
    std::lock_guard<std::mutex> lk(mtx_);
    ensureInitialized();
    return rootKey_;
}

std::vector<unsigned char> KeyManager::GenerateWorkKey(size_t lengthBytes) {
    // Default: generate random work key
    return GenerateRandomBytes(lengthBytes);
}

std::vector<unsigned char> KeyManager::DeriveWorkKey(const std::string& info, size_t lengthBytes) {
    std::lock_guard<std::mutex> lk(mtx_);
    ensureInitialized();

    std::vector<unsigned char> out(lengthBytes);

    // We'll use HKDF with SHA256 via OpenSSL KDF API
    EVP_KDF_CTX *kctx = nullptr;
    OSSL_PARAM params[5];

    kctx = EVP_KDF_CTX_new(EVP_KDF_fetch(NULL, "HKDF", NULL));
    if (!kctx) throw std::runtime_error("EVP_KDF_CTX_new failed");

    params[0] = OSSL_PARAM_construct_octet_string("salt", seed_.data(), (int)seed_.size());
    params[1] = OSSL_PARAM_construct_octet_string("key", rootKey_.data(), (int)rootKey_.size());
    params[2] = OSSL_PARAM_construct_octet_string("info", const_cast<char*>(info.data()), (int)info.size());
    params[3] = OSSL_PARAM_construct_utf8_string("digest", const_cast<char*>("SHA256"), 0);
    params[4] = OSSL_PARAM_construct_end();

    if (1 != EVP_KDF_derive(kctx, out.data(), out.size(), params)) {
        EVP_KDF_CTX_free(kctx);
        throw std::runtime_error("HKDF derive failed");
    }

    EVP_KDF_CTX_free(kctx);
    return out;
}

// Internal helper: HKDF derive using an explicit root + salt without taking the KeyManager mutex.
// This lets callers that already hold `mtx_` derive keys safely (no double-lock).
static std::vector<unsigned char> HkdfDeriveFromRoot(const std::vector<unsigned char>& root,
                                                     const std::vector<unsigned char>& salt,
                                                     const std::string& info, size_t lengthBytes) {
    std::vector<unsigned char> out(lengthBytes);
    EVP_KDF_CTX *kctx = EVP_KDF_CTX_new(EVP_KDF_fetch(NULL, "HKDF", NULL));
    if (!kctx) throw std::runtime_error("EVP_KDF_CTX_new failed");

    OSSL_PARAM params[5];
    params[0] = OSSL_PARAM_construct_octet_string("salt", const_cast<unsigned char*>(salt.data()), (int)salt.size());
    params[1] = OSSL_PARAM_construct_octet_string("key", const_cast<unsigned char*>(root.data()), (int)root.size());
    params[2] = OSSL_PARAM_construct_octet_string("info", const_cast<char*>(info.data()), (int)info.size());
    params[3] = OSSL_PARAM_construct_utf8_string("digest", const_cast<char*>("SHA256"), 0);
    params[4] = OSSL_PARAM_construct_end();

    if (1 != EVP_KDF_derive(kctx, out.data(), out.size(), params)) {
        EVP_KDF_CTX_free(kctx);
        throw std::runtime_error("HKDF derive failed");
    }

    EVP_KDF_CTX_free(kctx);
    return out;
}

// --- AES-GCM helpers and encrypted file storage for work keys ---
static bool aesGcmEncrypt(const std::vector<unsigned char>& key32,
                          const std::vector<unsigned char>& plaintext,
                          std::vector<unsigned char>& iv,
                          std::vector<unsigned char>& ciphertext,
                          std::vector<unsigned char>& tag) {
    const size_t ivlen = KeyManager::kGcmIvLen; // recommended IV length for GCM
    iv = GenerateRandomBytes(ivlen);
    tag.assign(KeyManager::kGcmTagLen, 0);
    ciphertext.assign(plaintext.size(), 0);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) { EVP_CIPHER_CTX_free(ctx); return false; }
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)ivlen, NULL)) { EVP_CIPHER_CTX_free(ctx); return false; }
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key32.data(), iv.data())) { EVP_CIPHER_CTX_free(ctx); return false; }

    int len = 0;
    if (!plaintext.empty()) {
        if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), (int)plaintext.size())) { EVP_CIPHER_CTX_free(ctx); return false; }
    }
    int len2 = 0;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len2)) { EVP_CIPHER_CTX_free(ctx); return false; }
    ciphertext.resize(len + len2);

    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, (int)KeyManager::kGcmTagLen, tag.data())) { EVP_CIPHER_CTX_free(ctx); return false; }

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

static bool aesGcmDecrypt(const std::vector<unsigned char>& key32,
                          const std::vector<unsigned char>& iv,
                          const std::vector<unsigned char>& tag,
                          const std::vector<unsigned char>& ciphertext,
                          std::vector<unsigned char>& out_plain) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) { EVP_CIPHER_CTX_free(ctx); return false; }
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)iv.size(), NULL)) { EVP_CIPHER_CTX_free(ctx); return false; }
    if (1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key32.data(), iv.data())) { EVP_CIPHER_CTX_free(ctx); return false; }

    out_plain.assign(ciphertext.size(), 0);
    int len1 = 0;
    if (!ciphertext.empty()) {
        if (1 != EVP_DecryptUpdate(ctx, out_plain.data(), &len1, ciphertext.data(), (int)ciphertext.size())) { EVP_CIPHER_CTX_free(ctx); return false; }
    }

    // set expected tag
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, (int)tag.size(), const_cast<unsigned char*>(tag.data()))) { EVP_CIPHER_CTX_free(ctx); return false; }

    int len2 = 0;
    int ret = EVP_DecryptFinal_ex(ctx, out_plain.data() + len1, &len2);
    if (ret <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    out_plain.resize(len1 + len2);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool KeyManager::SaveEncryptedWorkKey(const std::string& path, const std::vector<unsigned char>& workKey) {
    // derive AES-256 key from `rootKey_` using HKDF with an explicit purpose label
    // - purpose separation (info) prevents accidental key reuse across domains
    // - use internal no-lock helper because caller already holds the mutex
    auto aesKey = HkdfDeriveFromRoot(rootKey_, seed_, "storage-encryption", KeyManager::kSha256Len);
    std::vector<unsigned char> iv, ciphertext, tag;
    bool ok = aesGcmEncrypt(aesKey, workKey, iv, ciphertext, tag);
    // cleanse derived key from memory immediately
    OPENSSL_cleanse(aesKey.data(), aesKey.size());
    if (!ok) {
        spdlog::error("aesGcmEncrypt failed");
        return false;
    }

    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        spdlog::error("Failed to open work key file for writing: {}", path);
        return false;
    }

    // Layout: [iv (12)] [tag (16)] [ciphertext]
    out.write(reinterpret_cast<const char*>(iv.data()), (std::streamsize)iv.size());
    out.write(reinterpret_cast<const char*>(tag.data()), (std::streamsize)tag.size());
    out.write(reinterpret_cast<const char*>(ciphertext.data()), (std::streamsize)ciphertext.size());
    out.close();
    return true;
}

bool KeyManager::ReadEncryptedWorkKey(const std::string& path, std::vector<unsigned char>& outWorkKey) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    std::vector<unsigned char> buf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    spdlog::debug("ReadEncryptedWorkKey: raw buf size={}", buf.size());
    if (buf.size() < KeyManager::kGcmIvLen + KeyManager::kGcmTagLen) return false;

    std::vector<unsigned char> iv(buf.begin(), buf.begin() + KeyManager::kGcmIvLen);
    std::vector<unsigned char> tag(buf.begin() + KeyManager::kGcmIvLen, buf.begin() + KeyManager::kGcmIvLen + KeyManager::kGcmTagLen);
    std::vector<unsigned char> ciphertext(buf.begin() + KeyManager::kGcmIvLen + KeyManager::kGcmTagLen, buf.end());

    spdlog::debug("ReadEncryptedWorkKey: iv={}, tag={}, ciphertext={}", iv.size(), tag.size(), ciphertext.size());

    // derive same AES-256 key via HKDF (purpose-labeled). use no-lock helper to avoid deadlock.
    auto aesKey = HkdfDeriveFromRoot(rootKey_, seed_, "storage-encryption", KeyManager::kSha256Len);
    std::vector<unsigned char> plain;
    bool ok = aesGcmDecrypt(aesKey, iv, tag, ciphertext, plain);
    // cleanse derived key from memory immediately
    OPENSSL_cleanse(aesKey.data(), aesKey.size());
    if (!ok) {
        spdlog::error("Failed to decrypt work key from {}", path);
        return false;
    }

    spdlog::debug("ReadEncryptedWorkKey: decrypted plain size={}", plain.size());

    outWorkKey = std::move(plain);
    return true;
}

std::vector<unsigned char> KeyManager::GetOrCreateWorkKey(const std::string& workKeyFilePath, size_t lengthBytes) {
    std::lock_guard<std::mutex> lk(mtx_);
    ensureInitialized();

    std::vector<unsigned char> work;
    spdlog::info("GetOrCreateWorkKey: attempting to read encrypted work key from {}", workKeyFilePath);
    bool readOk = ReadEncryptedWorkKey(workKeyFilePath, work);
    spdlog::info("GetOrCreateWorkKey: ReadEncryptedWorkKey returned {}", readOk);
    if (readOk) {
        spdlog::info("GetOrCreateWorkKey: read work size={} (requested={})", work.size(), lengthBytes);
        if (work.size() == lengthBytes) {
            spdlog::info("Loaded and decrypted work key from {}", workKeyFilePath);
            return work;
        }
        spdlog::warn("Work key length mismatch ({} != {}), regenerating.", work.size(), lengthBytes);
    } else {
        spdlog::info("No valid encrypted work key found at {}. Generating a new one.", workKeyFilePath);
    }

    work = GenerateRandomBytes(lengthBytes);
    if (!SaveEncryptedWorkKey(workKeyFilePath, work)) {
        spdlog::error("Failed to save encrypted work key to {}", workKeyFilePath);
    } else {
        spdlog::info("Saved encrypted work key to {}", workKeyFilePath);
    }

    return work;
}

void KeyManager::RotateSeed() {
    std::lock_guard<std::mutex> lk(mtx_);
    seed_ = GenerateRandomBytes(16);
    persistSeed();

    // re-derive root key
    rootKey_.assign(rootKeyLenBytes_, 0);
    int res = PKCS5_PBKDF2_HMAC(
        reinterpret_cast<const char*>(seed_.data()), (int)seed_.size(),
        seed_.data(), (int)seed_.size(),
        (int)pbkdf2Iterations_,
        EVP_sha256(), (int)rootKeyLenBytes_, rootKey_.data());

    if (res != 1) {
        throw std::runtime_error("PBKDF2 derivation failed in RotateSeed");
    }

    spdlog::info("Seed rotated and persisted to {}", seedFilePath_);
}
