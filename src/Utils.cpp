#include "Utils.h"

#include <sys/stat.h>
#include <thread>
#include <sstream>
#include <iomanip>
#include <vector>
#include <openssl/evp.h>

namespace Util {

bool Exists(const std::string &file)
{
    struct stat st;
    return stat(file.c_str(), &st) == 0;
}

unsigned int GetCores()
{
    return std::thread::hardware_concurrency();
}

/**
 * Convert binary data to a lowercase hexadecimal string.
 * See declaration in Utils.h for details.
 */
std::string BytesToHex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        oss << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return oss.str();
}

/**
 * Vector overload for BytesToHex.
 */
std::string BytesToHex(const std::vector<unsigned char>& v) {
    return BytesToHex(v.data(), v.size());
}

/**
 * Base64-encode binary data (RFC 4648, no line breaks).
 * Returns an empty string for zero-length input. Uses OpenSSL EVP_EncodeBlock.
 */
std::string Base64Encode(const unsigned char* data, size_t len) {
    if (len == 0) return std::string();
    size_t outlen = 4 * ((len + 2) / 3);
    std::vector<unsigned char> out(outlen + 1);
    int olen = EVP_EncodeBlock(out.data(), data, static_cast<int>(len));
    if (olen < 0) return std::string();
    return std::string(reinterpret_cast<char*>(out.data()), static_cast<size_t>(olen));
}

/**
 * Vector overload for Base64Encode.
 */
std::string Base64Encode(const std::vector<unsigned char>& v) {
    return Base64Encode(v.data(), v.size());
}

} // namespace Util