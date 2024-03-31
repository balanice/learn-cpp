#ifndef MY_CRYPT2_H
#define MY_CRYPT2_H

#include <string>

int gcm_encrypt(const std::string &plaintext,
                const std::string &key,
                const std::string &iv,
                unsigned char *cipherText,
                unsigned char *tag);

void testCrypt2();
#endif