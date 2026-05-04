#ifndef LICENSE_PARSER_H
#define LICENSE_PARSER_H

#include "license.h"
#include <cstdint>
#include <cstddef>

struct DerReader;

License parseLicense(const uint8_t* data, size_t size);

class LicenseParser {

private:
    static std::string readUtf8String(DerReader& r);
    static int32_t readInteger(DerReader& r);
    static std::string readGeneralizedTime(DerReader& r);
    static std::chrono::system_clock::time_point parseGeneralizedTime(const std::string& str);

    struct DerReader {};
};

#endif // LICENSE_PARSER_H