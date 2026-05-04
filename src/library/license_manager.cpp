#include <string>
#include <iostream>
#include <vector>
#include <iomanip>

#include "license_manager.h"
#include "../sha/SHA512.h"

//antigravity

LicenseManager::LicenseManager() {}

std::string LicenseManager::generateLicenseId(const std::string& productId) {
    std::vector<std::string> hashHex(4);
    std::vector<std::vector<unsigned char>> hashBytes(4);
    std::string licenseId;

    size_t length = productId.length();

    // Generar 4 hashes de diferentes fragmentos del ProductId
    for (int i = 0; i < 4; i++) {
        size_t inicio = i * length / 4;
        size_t fin = (i + 1) * length / 4;

        hashHex[i] = generateHash(productId, inicio, fin);
        hashBytes[i] = hexToBytes(hashHex[i]);
    }
    
    // XOR en cascada - método de mezcla
    std::vector<unsigned char> mixed = hashBytes[0];
    for (int i = 1; i < 4; ++i) {
        mixed = xorBuffers(mixed, hashBytes[i]);
    }

    return bytesToHex(mixed);
}


std::string LicenseManager::generateHash(const std::string& productId, size_t inicio, size_t fin) {
    SHA512 sha;
    std::string fragment = productId.substr(inicio, fin - inicio);
    return sha.hash(fragment); 
}


unsigned char LicenseManager::hexCharToValue(char c) {
    // Convierte un carácter hexadecimal a su valor numérico
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Carácter no hexadecimal");
}


std::vector<unsigned char> LicenseManager::hexToBytes(const std::string& hex) {
    if (hex.size() % 2 != 0)
        throw std::invalid_argument("Hex inválido (longitud impar)");

    std::vector<unsigned char> bytes;
    bytes.reserve(hex.size() / 2);

    for (size_t i = 0; i < hex.size(); i += 2) {
        unsigned char high = hexCharToValue(hex[i]);
        unsigned char low  = hexCharToValue(hex[i + 1]);
        bytes.push_back((high << 4) | low);
    }

    return bytes;
}


std::string LicenseManager::bytesToHex(const std::vector<unsigned char>& bytes) {
    std::ostringstream oss;
    for (unsigned char b : bytes) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    return oss.str();
}

std::vector<unsigned char> LicenseManager::xorBuffers(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b) {
    size_t n = std::min(a.size(), b.size());
    std::vector<unsigned char> result(n);

    for (size_t i = 0; i < n; ++i) {
        result[i] = a[i] ^ b[i];
    }

    return result;
}

