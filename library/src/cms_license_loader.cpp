#include "cms_license_loader.h"

#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "license_exception.h"
#include "utils/crypto_utils.h"

#include <openssl/bio.h>
#include <openssl/cms.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

namespace {
    // Cargar el certificado con el que se firmó la licencia
    X509* LoadCertificate(const std::string& path) {
        FILE* file = fopen(path.c_str(), "r");

        // Variable X509 para almacenar el certificado para validar la licencia
        X509* cert = PEM_read_X509(file, nullptr, nullptr, nullptr);
        fclose(file);

        if (!cert) {
            throw std::runtime_error("[ERROR] The file reading failed.");
        }

        return cert;
    }

    // Calcula el hash del certificado para compararlos y validar
    std::string GetFingerprint(X509* cert) {
        unsigned char md[EVP_MAX_MD_SIZE];
        unsigned int len = 0;

        // Se calcula el hash del certificado
        if (!X509_digest(cert, EVP_sha256(), md, &len)) {
            throw std::runtime_error("[ERROR] The process of calculating fingerprint failed.");
        }

        std::string result;

        // Se devuelve convertido a hexadecimal    
        return CryptoUtils::BytesToHex(
            std::vector<uint8_t>(md, md + len)
        );
    }

    // Parsear el CMS 
    CMS_ContentInfo* ParseCms(const uint8_t* data, size_t size) {
        const unsigned char* p = data;

        CMS_ContentInfo* cms = d2i_CMS_ContentInfo(nullptr, &p, size);
        if (!cms) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("[ERROR] The process of parsing CMS failed.");
        }

        return cms;
    }

    // Libera los recursos asociados al CMS y certificado
    void Cleanup(CMS_ContentInfo* cms, X509* cert) {
        CMS_ContentInfo_free(cms);
        X509_free(cert);
    }

}

namespace secenly::internal {

// Verificar que el firmante es de confianza
bool CmsLicenseLoader::IsTrustedSigner(
    CMS_ContentInfo* cms, 
    X509* trusted_cert, 
    std::vector<std::string>& signer_names
) {
    std::string trusted_fp = GetFingerprint(trusted_cert);

    STACK_OF(X509)* certs_in_cms = CMS_get1_certs(cms);
    if (!certs_in_cms) {
        return false;
    }

    bool trusted_found = false;

    for (int i = 0; i < sk_X509_num(certs_in_cms); ++i) {
        X509* cert = sk_X509_value(certs_in_cms, i);

        // Obtener nombre del firmante
        char subject[256];
        X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject));
        signer_names.push_back(subject);

        // Comparar fingerprint
        if (GetFingerprint(cert) == trusted_fp) {
            trusted_found = true;
        }
    }

    sk_X509_pop_free(certs_in_cms, X509_free);

    return trusted_found;
}

// Verificar firma y extraer contenido 
std::vector<uint8_t> CmsLicenseLoader::VerifyAndExtractContent(CMS_ContentInfo* cms) {
    BIO* out = BIO_new(BIO_s_mem());

    int result = CMS_verify(cms, nullptr, nullptr, nullptr, 
        out, CMS_BINARY | CMS_NO_SIGNER_CERT_VERIFY);

    if (result != 1) {
        ERR_print_errors_fp(stderr);
        BIO_free(out);
        throw LicenseException(LicenseError::InvalidSignature, 
            "[ERROR] The signature is invalid."
        );
    }
    std::vector<uint8_t> content;

    BUF_MEM* mem;
    BIO_get_mem_ptr(out, &mem);

    content.assign(
        reinterpret_cast<uint8_t*>(mem->data),
        reinterpret_cast<uint8_t*>(mem->data + mem->length)
    );

    BIO_free(out);

    return content;
}

// Extraer la licencia del archivo DER
CmsResult CmsLicenseLoader::ExtractLicenseDer(
    const uint8_t* data, 
    size_t size, 
    const std::string& cert_path
) {
    std::vector<std::string> signer_names;
    CmsResult result_struct;

    CMS_ContentInfo* cms = ParseCms(data, size);
    X509* trusted_cert = LoadCertificate(cert_path);

    if (!IsTrustedSigner(cms, trusted_cert, signer_names)) {
        Cleanup(cms, trusted_cert);
        throw LicenseException(LicenseError::UnauthorizedCert, 
            "[ERROR] The certificate is unauthorized."
        );
    }

    std::vector<uint8_t> content = VerifyAndExtractContent(cms);

    Cleanup(cms, trusted_cert);

    result_struct.content = std::move(content);
    result_struct.signers = std::move(signer_names);

    return result_struct;
}

}