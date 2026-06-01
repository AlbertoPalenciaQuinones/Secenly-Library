#ifndef LICENSE_PARSER_H
#define LICENSE_PARSER_H

#include "license.h"

namespace secenly::internal {

License ParseLicense(const uint8_t* data, size_t size);

}

#endif // LICENSE_PARSER_H