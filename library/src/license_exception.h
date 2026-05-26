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
    InvalidSignature,
    UnauthorizedCert,
    EmptyId,
    InvalidId,
    Expired,
    InvalidClock,
    Unauthorized
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
