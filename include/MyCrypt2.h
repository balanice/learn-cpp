#ifndef MY_CRYPT2_H
#define MY_CRYPT2_H

#include <string>
#include <vector>
#include "KeyManager.h"

int gcm_encrypt(const std::string &plaintext,
                const std::string &key,
                const std::string &iv,
                unsigned char *cipherText,
                unsigned char *tag);

// Decrypt AES-256-GCM encrypted data.
// Returns plaintext length on success, -1 on authentication failure.
int gcm_decrypt(unsigned char *cipherText, int cipherTextLen,
                unsigned char *tag,
                const std::string &key,
                const std::string &iv,
                unsigned char *plainText);

void testCrypt2();

int RandomBytes(unsigned char *randomBytes, int len);

void ReadRsaPubkey();

// Generate random bytes and return as vector
std::vector<unsigned char> GenerateRandomBytes(size_t length);

// Generate root key from random bytes using PBKDF2
std::string GenerateRootKey(size_t keyLength = KeyManager::kDefaultRootKeyLen, size_t iterations = KeyManager::kDefaultPbkdf2Iterations);

// Save hex string to file
bool SaveHexToFile(const std::string& hexString, const std::string& filename);

// Generate and save root key to file using PBKDF2
bool GenerateAndSaveRootKey(const std::string& filename, size_t keyLength = KeyManager::kDefaultRootKeyLen, size_t iterations = KeyManager::kDefaultPbkdf2Iterations);

#endif