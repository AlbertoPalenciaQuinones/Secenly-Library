#include <gtest/gtest.h>

#include <fstream>

#include "include/license_api.h"
#include "src/license_exception.h"

// TEST 1 - Simula todo el flujo de validación de la licencia con un archivo
// válido. Esta verificación incluye que se obtiene la licencia correctamente,
// que se valida inicialmente sin lanzar excepciones y que se valida en tiempo
// de ejecución sin lanzar excepciones. Es todo el flujo que aparece en la
// simulación del software propietario del ejemplo.
TEST(SystemTest, FullFlowWorksWithValidLicense) {
    std::remove("tamper_state.bin");

    std::string seed_path = "tests/resources/seed.dat";
    std::string license_path = "tests/resources/license.der";
    std::string cert_path = "tests/resources/cert.pem";
    
    EXPECT_NO_THROW({
        auto lic = LicenseAPI::ObtainLicense(
            license_path,
            cert_path
        );

        LicenseAPI::ValidateInitial(lic, seed_path);
        LicenseAPI::ValidateRuntime(lic);
    });
}



