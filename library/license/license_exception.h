#pragma once

#include <string>
#include <stdexcept>

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
