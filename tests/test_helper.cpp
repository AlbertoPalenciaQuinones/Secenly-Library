#include "test_helper.h"

#include <cstdint>
#include <fstream>
#include <vector>

std::vector<uint8_t> TestHelper::LoadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}