#ifndef LICENSE_IDENTIFIER_H
#define LICENSE_IDENTIFIER_H

#include <string>

namespace secenly::internal {

class LicenseManager {
public:
    LicenseManager(const std::string& product_id);

    const std::string GetLicenseId() const;

private:
    std::string GenerateLicenseId(const std::string& product_id);

    std::string license_id;
};

}

#endif // LICENSE_IDENTIFIER_H