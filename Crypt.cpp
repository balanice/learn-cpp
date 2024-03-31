#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/err.h>
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>

#include "Crypt.h"

// OpenSSL 3.x 中的AES-256 GCM 加密
std::string AESGCMEncrypt_Openssl3(const std::string& plaintext, const std::string& key, const std::string& iv) {
    // 验证输入尺寸
    if (key.size() != 32 || iv.size() != 12) {
        throw std::runtime_error("Invalid key or IV size for AES-256 GCM");
    }

    // 创建OSSL_LIB_CTX和EVP_CIPHER_CTX
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    // 加载算法
    EVP_CIPHER *cipher = EVP_CIPHER_fetch(libctx, "AES-256-GCM", NULL);
    if (!cipher) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Failed to fetch AES-256-GCM cipher");
    }

    // 将密钥和IV从std::string转换为unsigned char数组
    unsigned char key_[32], iv_[12];
    memcpy(key_, key.data(), key.size());
    memcpy(iv_, iv.data(), iv.size());

    // 初始化加密上下文
    if (!EVP_EncryptInit_ex2(ctx, cipher, NULL, NULL, NULL)) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed to initialize encryption context");
    }

    // 设置密钥和IV
    if (!EVP_CIPHER_CTX_set_key_length(ctx, 32) ||
        !EVP_EncryptInit_ex(ctx, NULL, NULL, key_, iv_)) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed to set key and IV in GCM mode");
    }

    // 分配缓冲区用于加密后数据和附加验证数据（如果有）
    size_t ciphertext_len = plaintext.size() + EVP_GCM_TLS_TAG_LEN;
    std::vector<unsigned char> ciphertext(ciphertext_len);

    // 初始化输出大小
    int outlen = 0;

    // 执行加密操作
    if (!EVP_EncryptUpdate(ctx, ciphertext.data(), &outlen, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size())) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed during encryption update");
    }
    ciphertext_len -= EVP_GCM_TLS_TAG_LEN; // 标签暂时不计入长度

    // 完成加密并获取标签
    if (!EVP_EncryptFinal_ex(ctx, ciphertext.data() + outlen, &outlen) ||
        !EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, EVP_GCM_TLS_TAG_LEN, ciphertext.data() + ciphertext_len)) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed during encryption finalization or retrieving the GCM tag");
    }
    ciphertext_len += outlen;

    // 清理
    EVP_CIPHER_free(cipher);
    EVP_CIPHER_CTX_free(ctx);
    OSSL_LIB_CTX_free(libctx);

    // 合并密文和标签
    ciphertext.resize(ciphertext_len);
    return std::string(reinterpret_cast<char*>(ciphertext.data()), ciphertext_len);
}

// OpenSSL 3.x 中的AES-256 GCM 解密
std::string AESGCMDecrypt_Openssl3(const std::string& ciphertextWithTag, const std::string& key, const std::string& iv) {
    // 分割密文和标签
    size_t ciphertext_len = ciphertextWithTag.size() - EVP_GCM_TLS_TAG_LEN;
    const char* ciphertext_ptr = ciphertextWithTag.data();
    const char* tag_ptr = ciphertextWithTag.data() + ciphertext_len;
    std::string ciphertext(ciphertext_ptr, ciphertext_len);
    std::string tag(tag_ptr, EVP_GCM_TLS_TAG_LEN);

    // 验证输入尺寸和转换数据
    if (key.size() != 32 || iv.size() != 12 || tag.size() != EVP_GCM_TLS_TAG_LEN) {
        throw std::runtime_error("Invalid key, IV or tag size for AES-256 GCM");
    }

    // 创建和初始化上下文
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER *cipher = EVP_CIPHER_fetch(libctx, "AES-256-GCM", NULL);
    if (!cipher) {
        ERR_print_errors_fp(stderr);
        OSSL_LIB_CTX_free(libctx);
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to fetch AES-256-GCM cipher");
    }

    // 设置密钥和IV
    unsigned char key_[32], iv_[12], tag_[EVP_GCM_TLS_TAG_LEN];
    memcpy(key_, key.data(), key.size());
    memcpy(iv_, iv.data(), iv.size());
    memcpy(tag_, tag.data(), tag.size());

    if (!EVP_DecryptInit_ex2(ctx, cipher, NULL, NULL, NULL) ||
        !EVP_CIPHER_CTX_set_key_length(ctx, 32) ||
        !EVP_DecryptInit_ex(ctx, NULL, NULL, key_, iv_)) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed to initialize decryption context");
    }

    // 设置标签以供验证
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, EVP_GCM_TLS_TAG_LEN, tag_)) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed to set GCM tag for decryption");
    }

    // 分配缓冲区用于解密后数据
    std::vector<unsigned char> decrypted(ciphertext_len);

    // 初始化输出大小
    int outlen = 0;

    // 执行解密操作
    if (!EVP_DecryptUpdate(ctx, decrypted.data(), &outlen, reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size()) ||
        !EVP_DecryptFinal_ex(ctx, decrypted.data() + outlen, &outlen)) {
        EVP_CIPHER_free(cipher);
        EVP_CIPHER_CTX_free(ctx);
        OSSL_LIB_CTX_free(libctx);
        throw std::runtime_error("Failed during decryption or authentication failed");
    }

    // 清理
    EVP_CIPHER_free(cipher);
    EVP_CIPHER_CTX_free(ctx);
    OSSL_LIB_CTX_free(libctx);

    // 返回解密后的明文
    decrypted.resize(outlen);
    return std::string(reinterpret_cast<char*>(decrypted.data()), decrypted.size());
}

void testCrypt() {
    // 示例密钥、IV 和待加密数据
    std::string key = "a2V5a2V5a2V5a2V5a2V5a2V5a2V5a2V5";
    std::string iv = "dGltZWNvbnRl";
    std::string plaintext = "Hello, world!";
    std::cout << "Original plaintext: " << plaintext << std::endl;

    // 加密数据
    std::string encrypted = AESGCMEncrypt_Openssl3(plaintext, key, iv);
    std::cout << "Encrypted text: " << encrypted << std::endl;

    // 解密数据
    std::string decrypted = AESGCMDecrypt_Openssl3(encrypted, key, iv);
    std::cout << "Decrypted text: " << decrypted << std::endl;
}