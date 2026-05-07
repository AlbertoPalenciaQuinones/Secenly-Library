#include <iostream>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <vector>
#include <cstdint>

#include "../library/license/license_service.h"

//=============================================================================
//=========== ESTA CLASE ES UNA "SIMULACIÓN" DE SOFTWARE PROPIETARIO ==========
//=============================================================================
//==== Esta clase servirá para que usted pueda observar como se comporta la ===
//======= biblioteca, que métodos son utilizables y como sería el flujo. ======
//=============================================================================

int main() {
    try {
        // Obtener la ruta de la semilla
        std::string seed_path ;
        std::cout << "[INPUT] Enter the path to your seed file: ";
        std::cin >> seed_path;

        // Obtener la ruta del archivo de licencia
        std::string license_path;
        std::cout << "[INPUT] Enter the path to your license file: ";
        std::cin >> license_path;

        std::ifstream license_file(license_path, std::ios::binary);
        if (!license_file) {
            throw std::runtime_error("[ERROR] An error happend oppening the license file");
        }

        // Obtener la ruta del archivo de certificado
        std::string cert_path;
        std::cout << "[INPUT] Enter the path to your certificate file: ";
        std::cin >> cert_path;

        // Crear flujo de entrada de archivo para leerlo
        std::ifstream cert_file(cert_path, std::ios::binary);
        if (!cert_file) {
            throw std::runtime_error("[ERROR] An error happend oppening the certificate file");
        }

        // Guardar todos los datos del archivo
        auto data = std::vector<uint8_t>(std::istreambuf_iterator<char>(license_file), std::istreambuf_iterator<char>());
        std::cout << "\n[INFO] License loaded correctly!\n";
        std::cout << "[INFO] Size: " << data.size() << " bytes\n";

        LicenseService::ValidateLicense(data, seed_path, cert_path);

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] Error procesing the license:\n";
        std::cerr << e.what() << std::endl;
    }

    //LicenseService::ValidateLicense(data, seed_path, cert_path);

    return 0;
}