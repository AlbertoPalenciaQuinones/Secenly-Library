#pragma once

#include <string>
#include <stdexcept>

namespace secenly::internal {

/**
 * Define el nombre de las distintas excepciones.
 *
 * Se pueden añadir las excepciones que se necesiten únicamente añadiendo 
 * su nombre a la enumeración.
 * 
 * El nombre de la excepción nueva será decisión del usuario que por alguna
 * necesidad, debe añadirla. No debe tocar absolutamente nada más de la clase.
 */
enum class LicenseError {
    // Inicialización (hardware_manager y product_manager)
    HardwareIdGenerationFailed, 
    SeedFileNotFound,

    // Firma y certificado (cms_license_loader)
    CertificateLoadError,
    CmsParseError,
    InvalidSignature,
    UnauthorizedCert,

    // Parseo de la licencia (license_parser)
    DERFormatError,
    ASN1TypeError,
    ASN1ValueError,

    // Validación de licencia  (license_service)
    Expired,
    EmptyId,
    InvalidId,

    // Mecanismo anti-tampering (anti_tamper)
    CorruptedState,
    FileIOError,
    RuntimeTamperingDetected,
    ClockRollbackDetected,

    // API pública (license_api)
    InternalError,
    SeedInitializationFailed
};

class LicenseException : public std::runtime_error {
    
public:
    LicenseException(LicenseError code, const std::string& msg)
        : std::runtime_error(msg), code_(code) {}

    LicenseError code() const { return code_; }

private:
    LicenseError code_;
};

}

