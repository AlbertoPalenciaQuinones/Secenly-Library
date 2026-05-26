#include "license_service.h"

#include <ctime>
#include <iostream>

#include "anti_tamper.h"
#include "license.h"
#include "license_exception.h"

namespace secenly::internal {

std::atomic<bool> valid{true};

bool LicenseService::CheckExpiration(const std::chrono::system_clock::time_point& expiration_date) {
    bool expirates = false;
    auto now = std::chrono::system_clock::now();
    if (now >= expiration_date)
        expirates = true;
    
    return expirates;
}

void LicenseService::ValidateRuntime(const License& lic) {
    static AntiTamperManager tamper;

    // Anti‑tamper
    tamper.Validate();

    // Validación licencia
    if (CheckExpiration(lic.expiration_date)) {
        throw LicenseException(LicenseError::Expired, 
            "[ERROR] The license is expired. Clossing application..."
        );
    }
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
void LicenseService::ValidateLicenseInitial(License& lic, const std::string& expected_id) {
    static AntiTamperManager tamper;

    // VALIDACIONES DE CAMPOS
    // Validación 1 - Identificador de la licencia vacío
    if (lic.id.empty()) 
        throw LicenseException(LicenseError::EmptyId, 
            "[ERROR] The license ID is empty. Clossing application..."
        );

    // Validación 2 - Identificador de licencia inválido
    if (lic.id != expected_id)
        throw LicenseException(LicenseError::InvalidId,
            "[ERROR] The license ID is invalid. Clossing application..."
        );

    // Validación 3 - La licencia ha expirado
    if (CheckExpiration(lic.expiration_date))      
        throw LicenseException(LicenseError::Expired, 
            "[ERROR] The license is expired. Clossing application..."
        );
        
    // Validación 4 - Validación anti-tamper
    tamper.Validate();
}

}