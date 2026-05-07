#include <gtest/gtest.h>
#include <fstream>

#include "test_helper.h"
#include "../library/license/license_service.h"
#include "../library/license/license_exception.h"

TEST(LicenseServiceTest, ValidLicenseDoesNotThrow) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");
    std::string seed = "seed.txt";

    EXPECT_NO_THROW(LicenseService::ValidateLicense(data, seed));
}

TEST(LicenseServiceTest, CorruptedLicenseThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    // Corromper el contenido
    data[10] ^= 0xFF;

    EXPECT_THROW(
        LicenseService::ValidateLicense(data, "seed.txt"), 
        LicenseException
    );
}

TEST(LicenseServiceTest, InvalidIdThrows) {
    std::vector<uint8_t> data =
        TestHelper::LoadFile("tests/resources/license_wrong_id.der");

    try {
        LicenseService::ValidateLicense(data, "seed.txt");
        FAIL();
    } catch (const LicenseException& e) {
        EXPECT_EQ(e.code(), LicenseError::InvalidId);
    }
}

TEST(LicenseServiceTest, ExpiredLicenseThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license_expired.der");

    EXPECT_THROW(
        LicenseService::ValidateLicense(data, "seed.txt"),
        LicenseException
    );
}
