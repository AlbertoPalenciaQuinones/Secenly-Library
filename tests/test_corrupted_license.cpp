#include <gtest/gtest.h>

#include "test_helper.h"
#include "src/cms_license_loader.h"
#include "src/license_exception.h"
#include "src/license_parser.h"
#include "src/license_service.h"

using namespace secenly::internal;

// TEST 1 - Al validar una licencia y corromper su contenido, se espera que se
// lanze una excepción indicando el error.
TEST(LicenseCorruptedTest, CorruptedLicenseThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    // Corromper el contenido
    data[10] ^= 0xFF;

    std::string id_expected = "281eec9156258a845ba5e88549d3ee43d01a6948f16d2a7927a3afbb981bb98361bf525a480a5cd7803a620c466c21bd380bd53961c46242c69f026fd768e616";

    CmsLicenseLoader loader;
    EXPECT_THROW(
        auto result = loader.ExtractLicenseDer(data.data(), data.size(), "tests/resources/cert.pem");,
        LicenseException
    );
}