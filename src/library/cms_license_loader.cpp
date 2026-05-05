#include "cms_license_loader.h"

#include <openssl/cms.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#include <stdexcept>

// Cargar el certificado con el que se firmó la licencia
X509* loadCertificate(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");

    if (!file) {
        throw std::runtime_error("No se pudo abrir cert.pem");
    }

    // Variable X509 para almacenar el certificado para validar la licencia
    X509* cert = PEM_read_X509(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (!cert) {
        throw std::runtime_error("Error leyendo cert.pem");
    }

    return cert;
}

// Calcula el hash del certificado para compararlos y validar
std::string getFingerprint(X509* cert) {
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    // Se calcula el hash del certificado
    if (!X509_digest(cert, EVP_sha256(), md, &len)) {
        throw std::runtime_error("Error calculando fingerprint");
    }

    std::string result;
    char buffer[3];

    // Se convierte a texto esa cadena hexadecimal
    for (unsigned int i = 0; i < len; i++) {
        sprintf(buffer, "%02X", md[i]);
        result += buffer;
    }

    return result;
}

// Valida y extrae la licencia
CmsResult CmsLicenseLoader::extractLicenseDer(const uint8_t* data, size_t size) {
    // Puntero para openssl
    const unsigned char* p = data;
    // Variable para almacenar a los firmantes
    std::vector<std::string> signerNames;
    // Variable para almacenar todo el contenido de la licencia estructurado
    CmsResult resultStruct;

    // Parser de bytes DER a estructura CMS
    CMS_ContentInfo* cms = d2i_CMS_ContentInfo(nullptr, &p, size);
    if (!cms) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Error parsing CMS");
    }

    // Cargar el certificado válido
    X509* trustedCert = loadCertificate("cert.pem");

    // Calcular la huella
    std::string trustedFingerprint = getFingerprint(trustedCert);

    // Lista de certificados
    STACK_OF(X509)* certs = sk_X509_new_null();
    sk_X509_push(certs, trustedCert);

    // Lista de certificados contenidos en la licencia
    STACK_OF(X509)* certsInCms = CMS_get1_certs(cms);

    bool trustedFound = false;

    // Si hay certificados en la licencia, se itera
    if (certsInCms) {
        for (int i = 0; i < sk_X509_num(certsInCms); i++) {
            // Obtención de un certificado individual
            X509* cert = sk_X509_value(certsInCms, i);

            // Almacenar la información del certificado en signerNames
            char subject[256];
            X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject));
            signerNames.push_back(subject);

            // Ontención la huella del certificado
            std::string fp = getFingerprint(cert);

            // Si coincide, el certificado es correcto 
            if (fp == trustedFingerprint) {
                trustedFound = true;
            }
        }

        // Liberar los certificados
        sk_X509_pop_free(certsInCms, X509_free);
    }
   
    // El certificado no es válido, no se autoriza la licencia
    if (!trustedFound) {
        X509_free(trustedCert);
        CMS_ContentInfo_free(cms);

        throw std::runtime_error("Certificado no autorizado");
    }

    // Comprueba integridas y firma
    BIO* out = BIO_new(BIO_s_mem());

    // Verificación de firma e integridad, se ignora la entidad firmante ya que
    // se utiliza la huella
    int result = CMS_verify(cms, nullptr, nullptr, nullptr, out, CMS_BINARY | CMS_NO_SIGNER_CERT_VERIFY);

    // Si la firma es inválida, se libera la memoria y se lanza un error
    if (result != 1) {
        ERR_print_errors_fp(stderr);

        CMS_ContentInfo_free(cms);
        BIO_free(out);
        X509_free(trustedCert);
        sk_X509_free(certs);

        throw std::runtime_error("Firma digital INVALIDA");
    }

    // Extraer el contenido
    BUF_MEM* mem;
    BIO_get_mem_ptr(out, &mem);

    // Se copia todo el contenido al vector
    std::vector<uint8_t> content(mem->data, mem->data + mem->length);

    // Liberar memoria
    CMS_ContentInfo_free(cms);
    BIO_free(out);
    X509_free(trustedCert);
    sk_X509_free(certs);

    // Guardar contenido y firmantes
    resultStruct.content = std::move(content);
    resultStruct.signers = std::move(signerNames);

    return resultStruct;
}