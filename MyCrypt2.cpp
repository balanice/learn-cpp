#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <openssl/evp.h>

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
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_GET_TAG, 16, tag)) {
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
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag)) {
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

void testCrypt2()
{
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