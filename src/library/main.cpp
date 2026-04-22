#include <iostream>
#include <ctime>

#include "product_identifier.h"
#include "license_manager.h"
#include "hardware_manager.h"

//#include "license.h"
//#include <botan/ber_dec.h>

int main() {
    HardwareManager hwManager;
    ProductIdentifier product;
    LicenseManager lcManager;
    if(!hwManager.initialize()) {
        std::cerr << "No se pudo inicializar el HWID." << std::endl;
        return 1;
    }
    
    if(!product.initialize()) {
        std::cerr << "No se pudo inicializar la semilla." << std::endl;
        return 1;
    }

    if(!product.setProductId(hwManager.getHwid())) {
        std::cerr << "No se pudo establecer el ProductId." << std::endl;
        return 1;
    }
    std::string licenseId = lcManager.generateLicenseId(product.getProductId());

    std::cout << "License ID generado: " << licenseId << std::endl;

    return 0;
}