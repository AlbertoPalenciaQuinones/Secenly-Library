#include <gtest/gtest.h>
#include <fstream>

#include "test_helper.h"
#include "src/cms_license_loader.h"
#include "src/license_exception.h"
#include "src/license_parser.h"
#include "src/license_service.h"

using namespace secenly::internal;

// TEST 1 - Al validar una licencia válida, no se expera que se lance la excepción.
TEST(LicenseServiceTest, ValidLicenseDoesNotThrow) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    std::string id_expected = "281eec9156258a845ba5e88549d3ee43d01a6948f16d2a7927a3afbb981bb98361bf525a480a5cd7803a620c466c21bd380bd53961c46242c69f026fd768e616";

    CmsLicenseLoader loader;
    auto result = loader.ExtractLicenseDer(data.data(), data.size(), "tests/resources/cert.pem");

    License lic = ParseLicense(result.content.data(), result.content.size());

    EXPECT_NO_THROW(
        LicenseService::ValidateLicenseInitial(lic.id, lic.expiration_date, id_expected)
    );
}

// TEST 2 - Al validar una licencia utilizando un identificador inválido, se
// espera que salte una excepción.
TEST(LicenseServiceTest, InvalidIdThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    CmsLicenseLoader loader;
    auto result = loader.ExtractLicenseDer(data.data(), data.size(), "tests/resources/cert.pem");

    License lic = ParseLicense(result.content.data(), result.content.size());

    std::string id_wrong = "000000";

    EXPECT_THROW(
        LicenseService::ValidateLicenseInitial(lic.id, lic.expiration_date, id_wrong),
        LicenseException
    );
}

// TEST 3 - Al validar una licencia ya expirada, se espera que salte una 
// excepción indicando que ya ha sido caducada.
TEST(LicenseServiceTest, ExpiredLicenseThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license_expired.der");

    std::string id_expected = "281eec9156258a845ba5e88549d3ee43d01a6948f16d2a7927a3afbb981bb98361bf525a480a5cd7803a620c466c21bd380bd53961c46242c69f026fd768e616";

    CmsLicenseLoader loader;
    auto result = loader.ExtractLicenseDer(data.data(), data.size(), "tests/resources/cert.pem");

    License lic = ParseLicense(result.content.data(), result.content.size());

    EXPECT_THROW(
        LicenseService::ValidateLicenseInitial(lic.id, lic.expiration_date, id_expected),
        LicenseException
    );
}

// TEST 4 - Al leer una licencia con un certificado no confiable, se espera que salte una
// excepción indicando que el certificado no es confiable.
TEST(LicenseServiceTest, UntrustedCertThrows) {
    std::vector<uint8_t> data = TestHelper::LoadFile("tests/resources/license.der");

    CmsLicenseLoader loader;
    EXPECT_THROW(
        loader.ExtractLicenseDer(data.data(), data.size(), "tests/resources/untrusted_cert.pem"),
        LicenseException
    );
}