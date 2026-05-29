#include "product_manager.h"

#include <filesystem>
#include <fstream>

#include "license_exception.h"

namespace fs = std::filesystem;

namespace secenly::internal {

/**
 * Inicializador de la semilla y el identificador de producto.
 *
 * Genera un archivo de semilla aleatorio si no existe en la ruta oportuna. El
 * identificador de producto es una concatenación de hwid + semilla. No se hace 
 * dentro del constructor ya que pueda fallar debido a la gran carga de trabajo.
 *
 * Este comportamiento puede ser modificado por cualquiera que utilice la 
 * biblioteca con el fin de que sea ajustable a las necesidades de cada usuario.
 *
 * La función cumple con las siguientes necesidades:
 *     - Verificar que existe archivo de semilla
 *     - Generarlo si no existe (aleatoriamente)
 *     - Generar identificador de producto
 * 
 * Debe saber que puede añadir distintos procedimientos escribiendo nuevas
 * funciones, pero todas ellas deben funcionar de forma correcta para no
 * romper el mecanismo de generación de identificador de producto.
 * 
 * Otro aspecto a tener en cuenta es que la modificación en Secenly-Library
 * de la generación del identificador de producto crea la necesidad de su
 * modificación en la herramienta Secenly, siempre y cuando se haya optado
 * utilizarla a la hora de generar licencias de software.
 */
bool ProductManager::Initialize(const std::string& hwid) {
    // Comprueba de que exista el directorio donde se creará la semilla
    if (!fs::exists(path)) {
        throw LicenseException(LicenseError::SeedFileNotFound, 
            "[ERROR] Seed file not found"
        );
    }

    // Asegura que existe el archivo de semilla en disco
    if (!HasSeed()) {
        throw LicenseException(LicenseError::SeedFileNotFound, 
            "[ERROR] Seed file not found"
        );
    }

    seed = ReadSeedBytes();
    std::string seed_string = hwid;

    // Se concatena el identificador de hardware con los bytes de la semilla
    for (unsigned char byte : seed) {
        seed_string += std::to_string(byte);
    }
    product_id = seed_string;
    return !seed.empty();
}

// Comprueba que exista una semilla en la ruta
bool ProductManager::HasSeed() const {
    return fs::exists(fs::path(path));
}

// Leer los bytes del archivo de semilla
std::vector<unsigned char> ProductManager::ReadSeedBytes() const {
    fs::path seed_file = fs::path(path);

    // Obtiene el archivo de semilla
    std::ifstream file(seed_file, std::ios::binary);
    
    if (!file) {
        throw LicenseException(LicenseError::SeedFileNotFound, 
            "[ERROR] Failed to read seed file"
        );
    }

    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(file), 
        std::istreambuf_iterator<char>()
    );
}

const std::string& ProductManager::GetProductId() const {
    return product_id;
}

void ProductManager::SetPath(const std::string& p) {
    path = fs::path(p).string();
}

}