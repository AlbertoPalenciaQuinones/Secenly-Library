#include "license_manager.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/crypto_utils.h"
#include "../utils/SHA512.h"

// Función única de esta misma clase
namespace {
// Genera el hash de un fragmento del identificador de producto
std::string GenerateHash(const std::string& product_id, size_t inicio, size_t fin) {
    SHA512 sha;
    std::string fragment = product_id.substr(inicio, fin - inicio);
    return sha.hash(fragment); 
}

}

// Inicializa el mánager de licencias generando el identificador de licencia
LicenseManager::LicenseManager(const std::string& product_id) {
    license_id = GenerateLicenseId(product_id);
}

// Genera el identificador de licencia a partir del de producto
std::string LicenseManager::GenerateLicenseId(const std::string& product_id) {
    std::vector<std::string> hash_hex(4);
    std::vector<std::vector<unsigned char>> hash_bytes(4);

    size_t length = product_id.length();

    // Generar 4 hashes de diferentes fragmentos del ProductId
    for (size_t  i = 0; i < 4; i++) {
        size_t inicio = i * length / 4;
        size_t fin = (i + 1) * length / 4;

        hash_hex[i] = GenerateHash(product_id, inicio, fin);
        hash_bytes[i] = CryptoUtils::HexToBytes(hash_hex[i]);
    }
    
    std::vector<unsigned char> mixed = hash_bytes[0];
    // XOR en cascada para mezclar hashes (ofuscación de la integridad)
    for (int i = 1; i < 4; ++i) {
        mixed = CryptoUtils::XorBuffers(mixed, hash_bytes[i]);
    }

    return CryptoUtils::BytesToHex(mixed);
}

std::string LicenseManager::GetLicenseId() const {
    return license_id;
}