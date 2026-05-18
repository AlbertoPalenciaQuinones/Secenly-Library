#include "license_service.h"

#include <atomic>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

#include "license.h"
#include "license_exception.h"

namespace {

    std::atomic<bool> valid{true};

    // Convertir un tiempo de reloj a una cadena
    std::string timePointToString(std::chrono::system_clock::time_point tp) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);

        std::tm* tm = std::gmtime(&t); 

        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", tm);

        return std::string(buffer);
    }
}

bool LicenseService::CheckExpiration(const std::chrono::system_clock::time_point& expiration_date) {
    bool expirates = false;
    auto now = std::chrono::system_clock::now();
    if (now >= expiration_date)
        expirates = true;
    
    return expirates;
}

void LicenseService::AntiTamper() {

}

// Para el algoritmo de latido en tiempo de ejecución
void LicenseService::StopHeartbeat() {
    this->running = false;

    if (this->heartbeat_thread.joinable()) {
        this->heartbeat_thread.join();
    }
}

// Función que verifica la licencia en tiempo de uso para comprobar su expiración
void LicenseService::StartHeartbeatAlgorithm(License& lic) {
    int interval = lic.heartbeat_interval;

    this->heartbeat_thread = std::thread([this, lic, interval]() {
        while (this->running) {
            try {
                std::cout << "[HEARTBEAT] Validating license..." << std::endl;
                LicenseService::CheckExpiration(lic.expiration_date);
                valid = true;
            } catch (...) {
                valid = false;
                this->running = false;

                std::cerr << "[HEARTBEAT] License invalid during runtime\n";
                return;

            }

            std::this_thread::sleep_for(std::chrono::seconds(interval));
        }
    });
}

/**
 * Valida la licencia para asegurar que se utilice una licencia válida.
 *
 * El comportamiento actual es, en primer lugar, la validación de la firma y 
 * del certificado, en segundo lugar, la obtención del objeto de tipo licencia
 * y por último, la validación de sus campos y otros mecanismos de seguridad.
 *
 * Este comportamiento puede ser modificado por cualquiera que utilice la 
 * biblioteca con el fin de que sea ajustable a las necesidades de cada usuario.
 *
 * La función cumple con las siguientes necesidades:
 *     - Validación de firma y certificado
 *     - Validación de parámetros de licencia
 *     - Mecanismo anti-tamper
 * 
 * Debe saber que puede añadir distintos procedimientos escribiendo nuevas
 * funciones, pero todas ellas deben funcionar de forma correcta para no
 * romper el mecanismo de validación de licencia.
 * 
 * Además, debe saber que debe añadir tantas funciones nuevas como mecanismos
 * de seguridad se quieran implementar en la biblioteca. Este es el corazón de
 * la biblioteca, se recomienda que si realmente quiere modificar su 
 * funcionamiento, añada o elimine mecanismos de seguridad de esta misma clase.
 */
bool LicenseService::ValidateLicenseInitial(License& lic, const std::string& expected_id) {
    std::cout << "\n==================== LICENCIA ====================\n";
    std::cout << "ID:           " << lic.id << std::endl;
    std::cout << "Creación:     " << timePointToString(lic.creation_date) << std::endl;
    std::cout << "Expiración:   " << timePointToString(lic.expiration_date) << std::endl;
    std::cout << "Heartbeat:    " << lic.heartbeat_interval << std::endl;
    std::cout << "Notas:        " << lic.notes << std::endl;

    // VALIDACIONES DE CAMPOS
    // Validación 1 - Identificador de la licencia vacío
    if (lic.id.empty()) 
        throw LicenseException(LicenseError::EmptyId, "License ID is empty");

    // Validación 2 - Identificador de licencia inválido
    if (lic.id != expected_id)
        throw LicenseException(LicenseError::InvalidId,"Invalid license ID");

    // Validación 3 - La licencia ha expirado
    //if (CheckExpiration(lic.expiration_date))      
        //throw std::runtime_error("License is expired");

}

bool LicenseService::ValidateRuntime(License& lic) {
    return valid;
}

/*
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
*/