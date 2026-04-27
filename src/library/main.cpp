#include <iostream>
#include <ctime>
#include <cstdio>

#include "product_identifier.h"
#include "license_manager.h"
#include "hardware_manager.h"

//#include <botan/ber_dec.h>

int main() {
    HardwareManager hwManager;
    ProductIdentifier product;
    LicenseManager lcManager;
    std::string seed_path ;
    std::cout << "Ingrese la ruta para almacenar la semilla: ";
    std::cin >> seed_path;
    product.setPath(seed_path);
    if(!hwManager.initialize()) {
        std::cerr << "No se pudo inicializar el HWID." << std::endl;
        return 1;
    }

    if(!product.initialize()) {
        std::cerr << "No se pudo inicializar la semilla." << std::endl;
        return 1;
    }

    product.setProductId(hwManager.getHwid());

    std::string licenseId = lcManager.generateLicenseId(product.getProductId());

    std::cout << "License ID generado: " << licenseId << std::endl;

    return 0;
}