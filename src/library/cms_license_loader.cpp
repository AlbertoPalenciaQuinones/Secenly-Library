#include "cms_license_loader.h"

#include <openssl/cms.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#include <stdexcept>

X509* loadCertificate(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");

    if (!file) {
        throw std::runtime_error("No se pudo abrir cert.pem");
    }

    X509* cert = PEM_read_X509(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (!cert) {
        throw std::runtime_error("Error leyendo cert.pem");
    }

    return cert;
}

std::string getFingerprint(X509* cert) {
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    if (!X509_digest(cert, EVP_sha256(), md, &len)) {
        throw std::runtime_error("Error calculando fingerprint");
    }

    std::string result;
    char buffer[3];

    for (unsigned int i = 0; i < len; i++) {
        sprintf(buffer, "%02X", md[i]);
        result += buffer;
    }

    return result;
}

CmsResult CmsLicenseLoader::extractLicenseDer(const uint8_t* data, size_t size) {
    const unsigned char* p = data;

    CMS_ContentInfo* cms = d2i_CMS_ContentInfo(nullptr, &p, size);
    if (!cms) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error parsing CMS");
    }

    // cargar certificado trusted
    X509* trustedCert = loadCertificate("cert.pem");

    std::string trustedFingerprint = getFingerprint(trustedCert);

    STACK_OF(X509)* certs = sk_X509_new_null();
    sk_X509_push(certs, trustedCert);

    // recoger firmantes (SIN imprimir)
    std::vector<std::string> signerNames;

    STACK_OF(X509)* certsInCms = CMS_get1_certs(cms);

    bool trustedFound = false;

    if (certsInCms) {
        for (int i = 0; i < sk_X509_num(certsInCms); i++) {
            X509* cert = sk_X509_value(certsInCms, i);

            // guardar sujeto (para devolver info)
            char subject[256];
            X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject));
            signerNames.push_back(subject);

            // comparar fingerprint
            std::string fp = getFingerprint(cert);

            if (fp == trustedFingerprint) {
                trustedFound = true;
            }
        }

        sk_X509_pop_free(certsInCms, X509_free);
    }
   
    if (!trustedFound) {
        X509_free(trustedCert);
        CMS_ContentInfo_free(cms);

        throw std::runtime_error("Certificado no autorizado");
    }

    BIO* out = BIO_new(BIO_s_mem());

    // VALIDACIÓN REAL
    int result = CMS_verify(
        cms,
        nullptr,
        nullptr,
        nullptr,
        out,
        CMS_BINARY | CMS_NO_SIGNER_CERT_VERIFY
    );

    if (result != 1) {
        ERR_print_errors_fp(stderr);

        CMS_ContentInfo_free(cms);
        BIO_free(out);
        X509_free(trustedCert);
        sk_X509_free(certs);

        throw std::runtime_error("Firma digital INVALIDA");
    }

    // extraer contenido
    BUF_MEM* mem;
    BIO_get_mem_ptr(out, &mem);

    std::vector<uint8_t> content(mem->data, mem->data + mem->length);

    // liberar memoria
    CMS_ContentInfo_free(cms);
    BIO_free(out);
    X509_free(trustedCert);
    sk_X509_free(certs);

    // devolver resultado estructurado
    CmsResult resultStruct;
    resultStruct.content = std::move(content);
    resultStruct.signers = std::move(signerNames);

    return resultStruct;
}