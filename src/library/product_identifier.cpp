#include <filesystem>
#include <fstream>
#include <random>

#include "product_identifier.h"

namespace fs = std::filesystem;

ProductIdentifier::ProductIdentifier() {}

bool ProductIdentifier::initialize() {
    // Si no tiene semilla ya creada, se genera una nueva
    if (!hasSeed()) {
        if (!createSeed()) {
            return false;
        }
    }
    seed = readSeedBytes();
    return !seed.empty();
}

bool ProductIdentifier::hasSeed() {
    if (!fs::exists(path) || !fs::is_directory(path)) {
        fs::create_directory(path);
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry)) {
            return true;
        }
    }
    return false;
}

bool ProductIdentifier::createSeed() {
    if (hasSeed()) {
        return true;
    }

    fs::path seedFile = fs::path(path) / "seed.dat";
    std::ofstream file(seedFile, std::ios::binary);
    if (!file) return false;

    // Preparar datos aleatorios para escribir en el archivo
    std::random_device random;
    std::mt19937 gen(random());
    std::uniform_int_distribution<uint32_t> dist;

    // Se escriben 64 valores aleatorios de 4 bytes cada uno, total = 256 bytes
    for (int i = 0; i < 64; ++i) {
        uint32_t value = dist(gen);
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    return true;
}

// Lee el contenido del archivo de semilla y lo devuelve como vector de bytes
std::vector<unsigned char> ProductIdentifier::readSeedBytes() {
    fs::path dir(path);

    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return {};
    }

    // Buscar el único archivo dentro
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (fs::is_regular_file(entry)) {
            std::ifstream file(entry.path(), std::ios::binary);
            if (!file) return {};

            // Leer todo el archivo en bytes
            return std::vector<unsigned char>(
                std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>()
            );
        }
    }

    // No había ningún archivo
    return {};
}

// Establece el ProductId combinando la semilla y el hwid
void ProductIdentifier::setProductId(const std::string& hwid) {
    std::string seedString = hwid;
    for (unsigned char byte : seed) {
        seedString += std::to_string(byte);
    }
    product_id = seedString;
}

// Getter para el ProductId
std::string ProductIdentifier::getProductId() const {
    return product_id;
}

void ProductIdentifier::setPath(const std::string& p) {
    path = p + "/seed";
}