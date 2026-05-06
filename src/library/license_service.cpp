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

namespace {

}

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
