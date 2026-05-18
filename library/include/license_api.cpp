#include "license_api.h"

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "cms_license_loader.h"
#include "license.h"
#include "license_exception.h"
#include "license_parser.h"
#include "license_service.h"
#include "hardware_manager.h"
#include "license_manager.h"
#include "product_manager.h"

License LicenseAPI::ObtainLicense(
    const std::string& license_path, 
    const std::string& cert_path)
{
    // Crear flujo de entrada de archivo para leerlo
    std::ifstream license_file(license_path, std::ios::binary);
    if (!license_file) {
        throw std::runtime_error("[ERROR] An error happend oppening the certificate file");
    }

    auto data = std::vector<uint8_t>(std::istreambuf_iterator<char>(license_file), std::istreambuf_iterator<char>());
    // Verificar firma CMS
    CmsLicenseLoader loader;
    auto result = loader.ExtractLicenseDer(data.data(), data.size(), cert_path);

    // Parsear licencia
    License lic = ParseLicense(result.content.data(), result.content.size());

    return lic;
}

bool LicenseAPI::ValidateInitial(License& lic, std::string& seed_path) {
    HardwareManager hw;
    ProductManager product;

    product.SetPath(seed_path);

    if (!product.Initialize(hw.GetHwid())) {
        throw std::runtime_error("[ERROR] Seed initialization failed.");
    }

    LicenseManager lic_manager(product.GetProductId());
    // Generar ID esperado
    std::string expected_id = lic_manager.GetLicenseId();
    // Validar licencia
    return LicenseService::ValidateLicenseInitial(lic, expected_id);
}

bool LicenseAPI::ValidateRuntime(License& lic) {
    LicenseService::ValidateRuntime(lic);
    return true;
}