#include <gtest/gtest.h>
#include <fstream>

#include "test_helper.h"
#include "../library/license/license_service.h"
#include "../library/license/license_exception.h"

// TEST 1 - Al validar una licencia válida, no se expera que se lance la excepción.
TEST(LicenseServiceTest, ValidLicenseDoesNotThrow) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    EXPECT_NO_THROW(
        LicenseService::ValidateLicense(data, "seed.txt", "tests/resources/license.der")
    );
}

// TEST 2 - Al validar una licencia y corromper su contenido, se espera que se
// lanze una excepción indicando el error.
TEST(LicenseServiceTest, CorruptedLicenseThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    // Corromper el contenido
    data[10] ^= 0xFF;

    EXPECT_THROW(
        LicenseService::ValidateLicense(data, "seed.txt", "tests/resources/license.der"), 
        LicenseException
    );
}

// TEST 3 - Al validar una licencia utilizando un identificador inválido, se
// espera que salte una excepción.
TEST(LicenseServiceTest, InvalidIdThrows) {
    std::vector<uint8_t> data =
        TestHelper::LoadFile("tests/resources/license_wrong_id.der");

    try {
        LicenseService::ValidateLicense(data, "seed.txt", "tests/resources/license.der");
        FAIL();
    } catch (const LicenseException& e) {
        EXPECT_EQ(e.code(), LicenseError::InvalidId);
    }
}

// TEST 4 - Al validar una licencia ya expirada, se espera que salte una 
// excepción indicando que ya ha sido caducada.
TEST(LicenseServiceTest, ExpiredLicenseThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license_expired.der");

    EXPECT_THROW(
        LicenseService::ValidateLicense(data, "seed.txt", "tests/resources/license.der"),
        LicenseException
    );
}
