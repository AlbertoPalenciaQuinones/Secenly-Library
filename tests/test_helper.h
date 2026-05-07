#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <cstdint>
#include <string>
#include <vector>

class TestHelper {
public:
    static std::vector<uint8_t> LoadFile(const std::string& path);
};

#endif // TEST_HELPER_H