#include <gtest/gtest.h>

#include "test_helper.h"
#include "src/cms_license_loader.h"
#include "src/license_exception.h"

using namespace secenly::internal;

// TEST 1 - Se extrae la licencia de un archivo der inválido.
TEST(CmsLoaderTest, InvalidDerThrows) {
    CmsLicenseLoader loader;

    std::vector<uint8_t> invalid = {0x00, 0x01, 0x02};

    EXPECT_THROW(
        loader.ExtractLicenseDer(invalid.data(), invalid.size(), "tests/resources/license.der"),
        std::exception
    );
}

// TEST 2 - Se extrae la licencia del archivo der, el cual contiene una firma
// corrupta.
TEST(CmsLoaderTest, CorruptedSignatureThrows) {
    CmsLicenseLoader loader;

    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    // Corromper firma
    data[data.size() - 10] ^= 0xFF; 

    EXPECT_THROW(
        loader.ExtractLicenseDer(data.data(), data.size(), "tests/resources/license.der"),
        LicenseException
    );
}



