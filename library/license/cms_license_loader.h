#ifndef CMS_LICENSE_LOADER_H
#define CMS_LICENSE_LOADER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct CmsResult {
    std::vector<uint8_t> content;
    std::vector<std::string> signers;
};

class CmsLicenseLoader {
public:
    CmsResult ExtractLicenseDer(const uint8_t* data, size_t size, const std::string& cert_path);
};

#endif // CMS_LICENSE_LOADER_H