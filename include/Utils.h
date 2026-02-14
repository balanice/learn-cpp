#ifndef _LEARN_CPP_UTILS_H
#define _LEARN_CPP_UTILS_H

#include <string>
#include <vector>

namespace Util {

bool Exists(const std::string &file);

unsigned int GetCores();

// Hex / Base64 helpers
/**
 * Convert binary data to a lowercase hexadecimal string.
 *
 * - Produces two hex characters per input byte (no 0x prefix).
 * - Useful for logging, diagnostics and key/material display.
 *
 * @param data pointer to binary data
 * @param len  length of data in bytes
 * @return lowercase hex string (empty if len == 0)
 */
std::string BytesToHex(const unsigned char* data, size_t len);

/**
 * Overload that accepts a std::vector of bytes.
 */
std::string BytesToHex(const std::vector<unsigned char>& v);

/**
 * Base64-encode binary data using the standard (RFC 4648) encoding with no
 * line breaks. Returns an empty string for zero-length input.
 *
 * Implementation note: this uses OpenSSL's EVP_EncodeBlock internally.
 *
 * @param data pointer to binary data
 * @param len  length of data in bytes
 * @return base64-encoded string
 */
std::string Base64Encode(const unsigned char* data, size_t len);

/**
 * Overload that accepts a std::vector of bytes.
 */
std::string Base64Encode(const std::vector<unsigned char>& v);

} // namespace Util

#endif