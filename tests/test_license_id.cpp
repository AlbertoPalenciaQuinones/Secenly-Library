#include <gtest/gtest.h>

#include "library/license_manager.h"

// TEST 1 - Al generar 2 identificadores de licencia de un mismo identificador
// de producto, se generan 2 identificador de licencia iguales. De esta forma, 
// se comprueba la secuencia de hashes y la operación XOR.
TEST(LicenseIdentifierTest, GeneratesStableLicense) {
    LicenseManager manager;

    std::string productId = "test_product";
    
    std::string id1 = manager.generateLicenseId(productId);
    std::string id2 = manager.generateLicenseId(productId);

    EXPECT_EQ(id1, id2);
}

// TEST 2 - Se generan distintos identificadores de licencia para distintos ids 
// de producto. Se comprueba solo cambiando un caracter del identificador.
TEST(LicenseIdentifierTest, GeneratesDifferentLicensesForDifferentProducts) {
    LicenseManager manager;

    std::string productId1 = "test_product_1";
    std::string productId2 = "test_product_2";
    
    std::string id1 = manager.generateLicenseId(productId1);
    std::string id2 = manager.generateLicenseId(productId2);

    EXPECT_NE(id1, id2);
}

