#include <gtest/gtest.h>

#include "../library/manager/license_manager.h"

// TEST 1 - Al generar 2 identificadores de licencia de un mismo identificador
// de producto, se generan 2 identificador de licencia iguales. De esta forma, 
// se comprueba la secuencia de hashes y la operación XOR.
TEST(LicenseIdentifierTest, GeneratesStableLicense) {
    std::string product_id = "test_product";

    LicenseManager lc_manager(product_id);
    
    std::string id1 = lc_manager.GetLicenseId();
    std::string id2 = lc_manager.GetLicenseId();

    EXPECT_EQ(id1, id2);
}

// TEST 2 - Se generan distintos identificadores de licencia para distintos ids 
// de producto. Se comprueba solo cambiando un caracter del identificador.
TEST(LicenseIdentifierTest, GeneratesDifferentLicensesForDifferentProducts) {
    std::string product_id1 = "test_product_1";
    std::string product_id2 = "test_product_2";

    LicenseManager lc_manager1(product_id1);
    LicenseManager lc_manager2(product_id2);
    
    std::string id1 = lc_manager1.GetLicenseId();
    std::string id2 = lc_manager2.GetLicenseId();

    EXPECT_NE(id1, id2);
}

