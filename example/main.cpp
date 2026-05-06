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
        std::string seed_path ;
        std::cout << "Ingrese la ruta para almacenar la semilla: ";
        std::cin >> seed_path;

        // Obtener la ruta del archivo de licencia
        std::string license_path;
        std::cout << "\nIntroduce la ruta del archivo de licencia (DER CMS): ";
        std::cin >> license_path;

        // Crear flujo de entrada de archivo para leerlo
        std::ifstream file(license_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("No se pudo abrir el archivo");
        }

        // Guardar todos los datos del archivo
        auto data = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        std::cout << "\n[INFO] Archivo cargado correctamente\n";
        std::cout << "[INFO] Tamaño: " << data.size() << " bytes\n";

        LicenseService::ValidateLicense(data, seed_path);

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] Fallo procesando licencia:\n";
        std::cerr << e.what() << std::endl;
    }

    return 0;
}