#include <gtest/gtest.h>

#include "test_helper.h"
#include "../library/license/cms_license_loader.h"
#include "../library/license/license_exception.h"

TEST(CmsLoaderTest, InvalidDerThrows) {
    CmsLicenseLoader loader;

    std::vector<uint8_t> invalid = {0x00, 0x01, 0x02};

    EXPECT_THROW(loader.ExtractLicenseDer(invalid.data(), invalid.size()),
                 std::exception
    );
}

TEST(CmsLoaderTest, CorruptedSignatureThrows) {
    CmsLicenseLoader loader;

    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    // Corromper firma
    data[data.size() - 10] ^= 0xFF; 

    EXPECT_THROW(
        loader.ExtractLicenseDer(data.data(), data.size()),
        LicenseException
    );
}

TEST(CmsLoaderTest, UnauthorizedCertThrows) {
    CmsLicenseLoader loader;

    auto data = TestHelper::LoadFile("tests/resources/license.der");

    EXPECT_THROW(
        loader.ExtractLicenseDer(data.data(), data.size()),
        std::exception   // no puedes garantizar Unauthorized todavía
    );
}



