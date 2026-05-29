#pragma once

#include <cstdint>

namespace secenly::internal {

class AntiTamperManager {
public:
    void Validate();

private:
    void CheckTamper();
    uint64_t ComputeSignature(uint64_t time);
    uint64_t LoadLastTime();
    void SaveLastTime(uint64_t time);
};

}
