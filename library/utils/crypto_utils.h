#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <string>
#include <vector>

class CryptoUtils {
public:
    static std::vector<unsigned char> HexToBytes(const std::string& hex);
    static std::string BytesToHex(const std::vector<unsigned char>& bytes);
    static std::vector<unsigned char> XorBuffers(
        const std::vector<unsigned char>& a,
        const std::vector<unsigned char>& b
    );
};

#endif
