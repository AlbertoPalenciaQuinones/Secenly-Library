#include "license_service.h"

#include <cstdio>
#include <ctime>
#include <iostream>
#include <vector>

#include "cms_license_loader.h"
#include "license.h"
#include "license_exception.h"
#include "license_parser.h"
#include "../manager/hardware_manager.h"
#include "../manager/license_manager.h"
#include "../manager/product_manager.h"

// Función para ejecutar todo el flujo de creación del identificador de licencia.
// Orquesta el flujo completo de generación del LicenseId
std::string LicenseService::GenerateLicenseId(const std::string& seed_path) {
    HardwareManager hardware;
    ProductManager product;

    // Configurar ruta de la semilla
    product.SetPath(seed_path);
    // Obtener hwid para realizar comprobaciones y establecer id del producto
    std::string hwid = hardware.GetHwid();

    if (hwid.empty()) {
        throw std::runtime_error("HWID initialization failed");
    }

    if (!product.Initialize(hwid)) {
        throw std::runtime_error("Seed initialization failed");
    }

    LicenseManager lc_manager(product.GetProductId());
    return lc_manager.GetLicenseId();
}

std::string timePointToString(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    std::tm* tm = std::gmtime(&t); 

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", tm);

    return std::string(buffer);
}

bool CheckExpiration(const std::chrono::system_clock::time_point& expiration_date) {
    bool expirates = false;
    auto now = std::chrono::system_clock::now();
    if (now >= expiration_date)
        expirates = true;
    
    return expirates;
}

void AntiTamper() {
    
}

// Función que actualiza el último uso de la licencia y comprueba expiración por latido
void LicenseService::HeartbeatAlgorithm(int& beat) {

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
void LicenseService::ValidateLicense(const std::vector<uint8_t>& data, const std::string& seed_path, const std::string& cert_path) {
    // Identificador de licencia esperado
    std::string lic_id = LicenseService::GenerateLicenseId(seed_path);
    
    // Objeto cargador de licencia
    CmsLicenseLoader loader;

    // Extraer el bloque de datos de la licencia para manejar
    auto result = loader.ExtractLicenseDer(data.data(), data.size(), cert_path);

    // Parsear licencia
    License lic = ParseLicense(result.content.data(), result.content.size());

    std::cout << "\n=== LICENCIA ===\n";
    std::cout << "ID:           " << lic.id << std::endl;
    std::cout << "Creación:     " << timePointToString(lic.creation_date) << std::endl;
    std::cout << "Expiración:   " << timePointToString(lic.expiration_date) << std::endl;
    std::cout << "Heartbeat:    " << lic.heartbeat_interval << std::endl;
    std::cout << "Notas:        " << lic.notes << std::endl;

    //VALIDACIONES DE CAMPOS
    // Validación 1 - Identificador de la licencia vacío
    if (lic.id.empty()) 
        throw LicenseException(LicenseError::EmptyId, "License ID is empty");

    // Validación 2 - Identificador de licencia inválido
    if (lic.id != lic_id)
        throw LicenseException(LicenseError::InvalidId,"Invalid license ID");

    // Validación 3 - La licencia ha expirado
    if (CheckExpiration(lic.expiration_date))      
        throw LicenseException(LicenseError::Expired, "License is expired");

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

    std::cout << "\n=== LICENCIA ===\n";
    std::cout << "ID:           " << lic.id << std::endl;
    std::cout << "Creación:     " << timePointToString(lic.creation_date) << std::endl;
    std::cout << "Expiración:   " << timePointToString(lic.expiration_date) << std::endl;
    std::cout << "Heartbeat:    " << lic.heartbeat_interval << std::endl;
    std::cout << "Notas:        " << lic.notes << std::endl;
*/