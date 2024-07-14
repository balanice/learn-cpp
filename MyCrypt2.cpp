#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/decoder.h>
#include <openssl/core_names.h>

#include <spdlog/spdlog.h>

#include "MyCrypt2.h"

int gcm_encrypt(const std::string &plaintext,
                const std::string &key,
                const std::string &iv,
                unsigned char *cipherText,
                unsigned char *tag)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int cipherTextLen;

    // create and initialize the context.
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        throw std::runtime_error("init cipher context failed!");
    }

    // Initialise the encryption operation.
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Init gcm encrypt operation failed!");
    }

    // set IV length.
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.length(), NULL)) {
        throw std::runtime_error("Set IV length failed.");
    }

    // initialise key and IV.
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, 
                                reinterpret_cast<const unsigned char *>(key.c_str()), 
                                reinterpret_cast<const unsigned char *>(iv.c_str()))) {
        throw std::runtime_error("Init key and IV failed");
    }

    // Provide the message to encrypted, and obtain the encrypted output.
    if (1 != EVP_EncryptUpdate(ctx, cipherText, &len, reinterpret_cast<const unsigned char *>(plaintext.c_str()), plaintext.length())) {
        throw std::runtime_error("encrypt failed");
    }
    cipherTextLen = len;

    // finalise the encryption.
    if (1 != EVP_EncryptFinal_ex(ctx, cipherText+len, &len)) {
        throw std::runtime_error("Finalise encryption error.");
    }
    cipherTextLen += len;

    // Get the tag.
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_GET_TAG, EVP_GCM_TLS_TAG_LEN, tag)) {
        throw std::runtime_error("Get gcm tag error");
    }

    // Clean up.
    EVP_CIPHER_CTX_free(ctx);

    return cipherTextLen;
}

std::string UnsignedCharArrayToHex(const unsigned char* data, size_t dataSize) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (size_t i = 0; i < dataSize; ++i) {
        oss << std::setw(2) << static_cast<unsigned>(data[i]);
    }

    return oss.str();
}

// 将十六进制字符串转换为unsigned char数组
std::vector<unsigned char> HexStringToBytes(const std::string& hexStr) {
    std::vector<unsigned char> bytes;

    for (size_t i = 0; i < hexStr.length(); i += 2) {
        std::string byteString = hexStr.substr(i, 2);
        unsigned int byteValue;
        std::istringstream iss(byteString);
        iss >> std::hex >> byteValue;
        if (!iss.fail()) {
            bytes.push_back(static_cast<unsigned char>(byteValue));
        } else {
            // 错误处理，如遇到非法字符
            throw std::invalid_argument("Invalid hexadecimal string");
        }
    }

    return bytes;
}

int gcm_decrypt(unsigned char *cipherText, int cipherTextLen,
                unsigned char *tag,
                const std::string &key,
                const std::string &iv,
                unsigned char *plainText)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plainTextLen;
    int ret;

    // Initialise context.
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        throw std::runtime_error("Initialise context error");
    }

    // Initialise the decryption operation.
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, 
                                reinterpret_cast<const unsigned char *>(key.c_str()), 
                                reinterpret_cast<const unsigned char *>(iv.c_str()))) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Initialise decryption operation error");
    }

    // Provide the message to be decrypted, and obtain the plaintext output.
    if (1 != EVP_DecryptUpdate(ctx, plainText, &len, cipherText, cipherTextLen)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt error.");
    }
    plainTextLen = len;

    // Set expected tag value.
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, EVP_GCM_TLS_TAG_LEN, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Set tag error.");
    }

    ret = EVP_DecryptFinal_ex(ctx, plainText + len, &len);

    // Clean up.
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        // Sucess.
        plainTextLen += len;
        return plainTextLen;
    } else {
        // verify failed.
        return -1;
    }
}

int RandomBytes(unsigned char *randoBytes, int len)
{
    int initStatus = RAND_poll();
    if (initStatus == 1) {
        int result = RAND_bytes(randoBytes, len);
        return result;
    }

    return -1;
}

void testCrypt2()
{
    ReadRsaPubkey();
    unsigned char randomBytes[32] = { 0 };
    int r = RandomBytes(randomBytes, 32);
    if (r == 1) {
        auto s = UnsignedCharArrayToHex(randomBytes, 32);
        std::cout << "Random: " << s << std::endl;
    }
    // 示例密钥、IV 和待加密数据
    std::string key = "a2V5a2V5a2V5a2V5a2V5a2V5a2V5a2V51";
    std::string iv = "dGltZWNvbnRl";
    std::string plaintext = "Hello, world!";
    unsigned char cipherText[100] = { 0 };
    unsigned char pText[100] = { 0 };
    unsigned char tag[100] = { 0 };
    std::cout << "Original plaintext: " << plaintext << std::endl;

    // 加密数据
    int cipherTextLength = gcm_encrypt(plaintext, key, iv, cipherText, tag);
    std::cout << "cipherTextLength: " << cipherTextLength << std::endl;
    auto tagHex = UnsignedCharArrayToHex(tag, 16);
    std::cout << "tag: " << tagHex << std::endl;
    auto cipherHex = UnsignedCharArrayToHex(cipherText, cipherTextLength);
    std::cout << "Encrypted text: " << cipherHex << std::endl;

    auto v = HexStringToBytes(cipherHex);
    unsigned char* ptr = new unsigned char[v.size()];
    std::copy(v.begin(), v.end(), ptr);

    int ret = gcm_decrypt(ptr, v.size(), tag, key, iv, pText);
    if (ret != -1) {
        std::cout << "Decrypted text: " << pText << std::endl;
        std::cout << "Decrypted text length: " << ret << std::endl;
    }

    delete[] ptr;
}

EVP_PKEY *get_key(OSSL_LIB_CTX *libctx, const std::string& key, const char* propq)
{
    OSSL_DECODER_CTX *dctx = NULL;
    EVP_PKEY *pkey = NULL;
    int selection;
    const unsigned char *data;
    size_t data_len;

    selection = EVP_PKEY_PUBLIC_KEY;
    data = (const unsigned char*)key.data();
    data_len = key.length();
    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "PEM", NULL, "RSA",
                                         selection, libctx, propq);
    (void)OSSL_DECODER_from_data(dctx, &data, &data_len);
    OSSL_DECODER_CTX_free(dctx);
    return pkey;
}

void ReadRsaPubkey()
{
    std::string key = "-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEAwDpa2EOEu7vCx88mVXzLHxdw1Yn0Hm7gkUEdnzdXzPenbL4NVLoj\n6lxtX2UQ10wZLQfHuv5elaBn9jkCxpZgb9zkD3hKqmVLJI6HXRi2OECEnOp0Edus\ncrCr3N/FXvf7ALT4i9LeUlfmUmTB+kIR7VpyPY2Pg88DvA5n7QXQRRqRu8d3NBXF\nZhB9nZd8Zb8XyTStKLwwd11e7JV4vUTiA3heoNnSsBEeLN1DWdQNjCqq35AFP2yd\nM1pncM4Zjhbyv0z0l776vCJyS4/iS78qund4i1dxp2l4gDoDuTd4Nck6oN/HC9Ba\nnuIqrv/RP0Sw01Dij7g59nVxYA1aN8cvxQIBAw==\n-----END RSA PUBLIC KEY-----\n";

    // 添加必要的换行符和头尾标识，如果公钥字符串中还没有的话
    // std::string formattedPublicKey = "-----BEGIN PUBLIC KEY-----\n";
    // for (size_t i = 0; i < key.size(); i += 64) {
    //     formattedPublicKey += key.substr(i, 64);
    //     formattedPublicKey += "\n";
    // }
    // formattedPublicKey += "-----END PUBLIC KEY-----\n";
    // spdlog::debug("formatted: {}", formattedPublicKey);
    OSSL_LIB_CTX *ctx = nullptr;
    const char *propq = NULL;
    auto rsa = get_key(ctx, key, propq);
    if (rsa == NULL) {
        spdlog::error("Failed to load RSA public key");
    } else {
        spdlog::debug("Sucess to load RSA public key");
    }

}