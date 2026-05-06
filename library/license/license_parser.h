#ifndef LICENSE_PARSER_H
#define LICENSE_PARSER_H

#include <cstddef>
#include <cstdint>

#include "license.h"

License ParseLicense(const uint8_t* data, size_t size);

#endif // LICENSE_PARSER_H