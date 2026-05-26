#ifndef CMS_LICENSE_LOADER_H
#define CMS_LICENSE_LOADER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <openssl/cms.h>
#include <openssl/x509.h>

namespace secenly::internal {

struct CmsResult {
    std::vector<uint8_t> content;
    std::vector<std::string> signers;
};

class CmsLicenseLoader {
public:
    CmsResult ExtractLicenseDer(
        const uint8_t* data, 
        size_t size, 
        const std::string& cert_path
    );

private:
    static bool IsTrustedSigner(
        CMS_ContentInfo* cms, X509* trusted_cert, 
        std::vector<std::string>& signer_names
    );
    std::vector<uint8_t> VerifyAndExtractContent(CMS_ContentInfo* cms);
};

}

#endif // CMS_LICENSE_LOADER_H