#include "crypto_utils.h"

#include <iomanip>
#include <string>

namespace {
    // Convierte un carácter hexadecimal en su valor numérico (0–15).
    unsigned char HexCharToValue(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw std::invalid_argument("CryptUtils: Invalid hexadecimal character");
    }

}

// Convertir hexadecimal a bytes, ej: "4A3F" -> "[0x4A], [0x3F]".
std::vector<unsigned char> CryptoUtils::HexToBytes(const std::string& hex) {
    if (hex.size() % 2 != 0)
        throw std::invalid_argument("CryptUtils: Invalid hexadecimal (odd length)");

    std::vector<unsigned char> bytes;
    bytes.reserve(hex.size() / 2);

    // Convierte cada byte a hexadecimal de 2 dígitos, rellenando con ceros si
    // es necesario.
    for (size_t i = 0; i < hex.size(); i += 2) {
        unsigned char high = HexCharToValue(hex[i]);
        unsigned char low  = HexCharToValue(hex[i + 1]);
        bytes.push_back((high << 4) | low);
    }

    return bytes;
}

// Convertir bytes a una cadena hexadecimal.
std::string CryptoUtils::BytesToHex(const std::vector<unsigned char>& bytes) {
    std::ostringstream oss;
    oss << std::hex;
    // Por cada byte del vector: convierte a hexadecimal -> asegura 2 caracteres
    // -> rellena con ceros
    for (unsigned char b : bytes) {
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }

    return oss.str();
}

// Aplicar operación XOR entre 2 buffers.
std::vector<unsigned char> CryptoUtils::XorBuffers(
    const std::vector<unsigned char>& a, 
    const std::vector<unsigned char>& b
) {  
    if (a.size() != b.size()) {
        throw std::invalid_argument("CryptUtils: Buffers must have same size");
    }
    
    size_t n = a.size();
    std::vector<unsigned char> result(n);
    // Para cada posición de los buffers, se aplica la operación "^".
    for (size_t i = 0; i < n; ++i) {
        result[i] = a[i] ^ b[i];
    }

    return result;
}