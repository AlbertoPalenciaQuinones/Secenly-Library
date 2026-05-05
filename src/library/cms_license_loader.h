#ifndef CMS_LICENSE_LOADER_H
#define CMS_LICENSE_LOADER_H

#include <vector>
#include <cstdint>
#include <cstddef>
#include <string>

struct CmsResult {
    std::vector<uint8_t> content;
    std::vector<std::string> signers;
};

class CmsLicenseLoader {
public:
    CmsResult extractLicenseDer(const uint8_t* data, size_t size);
};

#endif // CMS_LICENSE_LOADER_H