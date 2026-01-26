#pragma once

#include <string>
#include <vector>
#include <mutex>

// KeyManager: Singleton that manages a root key and work keys.
// - Root key is derived using PBKDF2(HMAC-SHA256) from a locally stored random seed (salt/password-style).
// - Only the random seed (not the derived root key) is persisted to disk.
// - Work keys are derived from the root key (or can be randomly generated) and you can rotate them.
// Thread-safe.

class KeyManager {
public:
    // Get the singleton instance
    static KeyManager& Instance();

    // Initialize with a path to store the seed and PBKDF2 iterations.
    // If the seed file exists it will be loaded; otherwise a new seed will be generated and saved.
    void Initialize(const std::string& seedFilePath, size_t rootKeyLenBytes = 32, size_t pbkdf2Iterations = 100000);

    // Returns the derived root key (hex string) - derived on demand and memoized until re-initialize.
    std::string GetRootKeyHex();

    // Returns root key raw bytes (binary)
    std::vector<unsigned char> GetRootKeyRaw();

    // Generate a new work key (binary) and return it. Optionally specify length in bytes.
    std::vector<unsigned char> GenerateWorkKey(size_t lengthBytes = 32);

    // Derive a work key from root key and an identifier (e.g., purpose, version) using HKDF-like derivation with HMAC-SHA256.
    // Returns the derived key bytes.
    std::vector<unsigned char> DeriveWorkKey(const std::string& info, size_t lengthBytes = 32);

    // Get or create a work key that's stored encrypted with the root key using AES-256-GCM.
    // - If the file exists we will try to read and decrypt it. If decryption fails or file not found,
    //   a new random work key will be generated and encrypted + saved to the given path.
    std::vector<unsigned char> GetOrCreateWorkKey(const std::string& workKeyFilePath, size_t lengthBytes = 32);

    // Rotate the seed (generate new seed and persist), forcing re-derivation of root key.
    void RotateSeed();

private:
    KeyManager();
    ~KeyManager();
    KeyManager(const KeyManager&) = delete;
    KeyManager& operator=(const KeyManager&) = delete;

    void ensureInitialized();
    void persistSeed();
    void loadSeedIfExists();

    // Internal helpers for storing work keys encrypted with root key using AES-256-GCM
    bool SaveEncryptedWorkKey(const std::string& path, const std::vector<unsigned char>& workKey);
    bool ReadEncryptedWorkKey(const std::string& path, std::vector<unsigned char>& outWorkKey);

    std::mutex mtx_;
    bool initialized_ = false;
    std::string seedFilePath_;
    size_t pbkdf2Iterations_ = 100000;
    size_t rootKeyLenBytes_ = 32;

    std::vector<unsigned char> seed_; // stored on disk
    std::vector<unsigned char> rootKey_; // derived and cached
};
