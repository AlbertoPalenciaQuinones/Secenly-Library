#ifndef LICENSE_API_H
#define LICENSE_API_H

#include <cstdint>
#include <string>
#include <vector>

#include "src/license.h"

class LicenseAPI {
public:
    static License ObtainLicense(
        const std::string& data,
        const std::string& cert_path);

    static void ValidateInitial(License& lic, std::string& seed_path);

    static bool ValidateRuntime(License& lic);
};

#endif // LICENSE_API_H