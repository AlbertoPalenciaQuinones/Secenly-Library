#include <gtest/gtest.h>
#include "../library/license/license_parser.h"

TEST(ParserTest, InvalidSequenceThrows) {
    std::vector<uint8_t> invalid = {0x01, 0x02, 0x03};

    EXPECT_THROW(
        ParseLicense(invalid.data(), invalid.size()),
        std::runtime_error
    );
}

TEST(ParserTest, TrailingDataThrows) {
    std::vector<uint8_t> data = {
        0x30, 0x06,
        0x0C, 0x01, 'A',
        0x0C, 0x01, 'B',
        0xFF // basura extra
    };

    EXPECT_THROW(
        ParseLicense(data.data(), data.size()),
        std::runtime_error
    );
}
