#include <atomic>
#include <iostream>
#include <thread>

#include "license_api.h"

//=============================================================================
//=========== ESTA CLASE ES UNA "SIMULACIÓN" DE SOFTWARE PROPIETARIO ==========
//=============================================================================
//==== Esta clase servirá para que usted pueda observar como se comporta la ===
//======= biblioteca, que métodos son utilizables y como sería el flujo. ======
//=============================================================================

std::atomic<bool> running = true;

// Convertir un tiempo de reloj a una cadena
std::string timePointToString(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    std::tm* tm = std::gmtime(&t); 

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", tm);

    return std::string(buffer);
}

int main() {
    // Obtener la ruta de la semilla
    std::string seed_path;
    std::cout << "[INPUT] Enter the path to your seed file: ";
    std::cin >> seed_path;

    // Obtener la ruta del archivo de licencia
    std::string license_path;
    std::cout << "[INPUT] Enter the path to your license file: ";
    std::cin >> license_path;

    // Obtener la ruta del archivo de certificado
    std::string cert_path;
    std::cout << "[INPUT] Enter the path to your certificate file: ";
    std::cin >> cert_path;

    // Guardar todos los datos del archivo
    License license = LicenseAPI::ObtainLicense(license_path, cert_path);

    std::cout << "\n==================== LICENCIA ====================\n";
    std::cout << "ID:           " << license.id << std::endl;
    std::cout << "Creation:     " << timePointToString(license.creation_date) << std::endl;
    std::cout << "Expiration:   " << timePointToString(license.expiration_date) << std::endl;
    std::cout << "Heartbeat:    " << license.heartbeat_interval << std::endl;
    std::cout << "Notes:        " << license.notes << std::endl;

    // Validación inicial al abrir el software propietario
    try {
        LicenseAPI::ValidateInitial(license, seed_path);
        std::cout << "[INFO] License params are valid.\n";
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
    }

    // SIMULACIÓN DE SOFTWARE PROPIETARIO CORRIENDO
    std::cout << "[INFO] Application running...\n";

    // Validación periódica en tiempo de ejecución   
    std::thread t([&license]() {
        while (running) {
            try {
                LicenseAPI::ValidateRuntime(license);
                std::cout << "[INFO] License valid. Next check in " 
                          << license.heartbeat_interval << " seconds.\n";
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
                running = false;
            }

            std::this_thread::sleep_for(
                std::chrono::seconds(license.heartbeat_interval)
            );
        }
    });
    t.join();

    // mantener proceso vivo
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}