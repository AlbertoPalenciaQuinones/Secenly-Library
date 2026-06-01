#include "license_api.h"

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "src/cms_license_loader.h"
#include "src/license.h"
#include "src/license_exception.h"
#include "src/license_parser.h"
#include "src/license_service.h"
#include "src/hardware_manager.h"
#include "src/license_manager.h"
#include "src/product_manager.h"

using namespace secenly::internal;

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

void LicenseAPI::ValidateInitial(License& lic, std::string& seed_path) {
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
    LicenseService::ValidateLicenseInitial(lic, expected_id);
}

bool LicenseAPI::ValidateRuntime(License& lic) {
    LicenseService::ValidateRuntime(lic);
    return true;
}