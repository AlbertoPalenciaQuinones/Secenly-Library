#include <iostream>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <vector>
#include <cstdint>

#include "product_identifier.h"
#include "license_manager.h"
#include "hardware_manager.h"

#include "cms_license_loader.h"
#include "license_parser.h"


//#include <botan/ber_dec.h>

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("No se pudo abrir el archivo");
    }

    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

std::string timePointToString(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    std::tm* tm = std::gmtime(&t); // UTC

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", tm);

    return std::string(buffer);
}

int main() {
    HardwareManager hwManager;
    ProductIdentifier product;
    LicenseManager lcManager;
    std::string seed_path ;
    std::cout << "Ingrese la ruta para almacenar la semilla: ";
    std::cin >> seed_path;
    product.setPath(seed_path);
    if(!hwManager.initialize()) {
        std::cerr << "No se pudo inicializar el HWID." << std::endl;
        return 1;
    }

    if(!product.initialize()) {
        std::cerr << "No se pudo inicializar la semilla." << std::endl;
        return 1;
    }

    int seed_size = product.readSeedBytes().size();
    std::cout << "Semilla leída con éxito. Tamaño: " << seed_size << " bytes." << std::endl;

    product.setProductId(hwManager.getHwid());

<<<<<<< HEAD
    try {
        std::string license_path;
        std::cout << "\nIntroduce la ruta del archivo de licencia (DER CMS): ";
        std::cin >> license_path;
=======
    std::cout << "Product ID: " << product.getProductId() << std::endl;

    std::string licenseId = lcManager.generateLicenseId(product.getProductId());
>>>>>>> f308f880cc35494dab97d042423a2a86eafdbaf0

        auto data = readFile(license_path);

        std::cout << "\n[INFO] Archivo cargado correctamente\n";
        std::cout << "[INFO] Tamaño: " << data.size() << " bytes\n";

        CmsLicenseLoader loader;

        auto result = loader.extractLicenseDer(data.data(), data.size());

        // ===============================
        //      INFORMACIÓN DE FIRMA
        // ===============================
        std::cout << "\n=== INFORMACIÓN DE FIRMA ===\n";

        if (result.signers.empty()) {
            std::cout << "No se encontraron certificados en el CMS\n";
        } else {
            for (const auto& signer : result.signers) {
                std::cout << "Certificado: " << signer << std::endl;
            }
        }

        // ===============================
        //        PARSEAR LICENCIA
        // ===============================
        License lic = parseLicense(
            result.content.data(),
            result.content.size()
        );

        std::cout << "\n=== LICENCIA ===\n";
        std::cout << "ID:           " << lic.id << std::endl;
        std::cout << "Creación:     " << timePointToString(lic.creation_date) << std::endl;
        std::cout << "Expiración:   " << timePointToString(lic.expiration_date) << std::endl;
        std::cout << "Último uso:   " << timePointToString(lic.last_use_date) << std::endl;
        std::cout << "Heartbeat:    " << lic.heartbeat_interval << std::endl;
        std::cout << "Notas:        " << lic.notes << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] Fallo procesando licencia:\n";
        std::cerr << e.what() << std::endl;
    }

    return 0;
}