#include "license_api.h"

#include <fstream>

#include "src/cms_license_loader.h"
#include "src/license.h"
#include "src/license_exception.h"
#include "src/license_parser.h"
#include "src/license_service.h"
#include "src/hardware_manager.h"
#include "src/license_manager.h"
#include "src/product_manager.h"

//=============================================================================
//========= ESTA CLASE ES LA API PÚBLICA DE LAS FUNCIONES UTILIZABLES =========
//=============================================================================
//====== Esta clase orquestra el flujo de las funciones principales de la =====
//============ biblioteca (obtención de la licencia y validaciones) ===========
//=============================================================================

using namespace secenly::internal;

// Obtener licencia a partir de un archivo de licencia y un certificado
License LicenseAPI::ObtainLicense(
    const std::string& license_path, 
    const std::string& cert_path)
{
    // Crear flujo de entrada de archivo para leerlo
    std::ifstream license_file(license_path, std::ios::binary);
    if (!license_file) {
        throw std::runtime_error("[ERROR] An error happend oppening the license file");
    }

    auto data = std::vector<uint8_t>(
        std::istreambuf_iterator<char>(license_file), 
        std::istreambuf_iterator<char>()
    );
    
    try {
        // Extraer el contenido de la licencia utilizando el cargador CMS
        CmsLicenseLoader loader;

        auto result = loader.ExtractLicenseDer(data.data(), data.size(), cert_path);
        License lic = ParseLicense(result.content.data(), result.content.size());

        return lic;

    } catch (const LicenseException& e) {
        throw; 

    } catch (const std::exception& e) {
        throw LicenseException(
            LicenseError::InternalError,
            "[ERROR] Failed to obtain license"
        );
    }
}

// Validación inicial al abrir el software propietario
bool LicenseAPI::ValidateInitial(License& lic, std::string& seed_path) {
    try {
        HardwareManager hw;
        ProductManager product;

        product.SetPath(seed_path);

        if (!product.Initialize(hw.GetHwid())) {
            throw LicenseException(
                LicenseError::SeedInitializationFailed,
                "[ERROR] Seed initialization failed."
            );
        }

        LicenseManager lic_manager(product.GetProductId());
        std::string expected_id = lic_manager.GetLicenseId();

        LicenseService::ValidateLicenseInitial(lic.id, lic.expiration_date, expected_id);
        return true;

    } catch (const LicenseException&) {
        throw;
    } catch (...) {
        throw LicenseException(
            LicenseError::InternalError,
            "[ERROR] Initial validation failed"
        );
    }
}

// Validación periódica en tiempo de ejecución
bool LicenseAPI::ValidateRuntime(const License& lic) {
    try {
        LicenseService::ValidateRuntime(lic.expiration_date);
        return true;

    } catch (const LicenseException&) {
        throw;
    } catch (...) {
        throw LicenseException(
            LicenseError::InternalError,
            "[ERROR] Runtime validation failed"
        );
    }
}