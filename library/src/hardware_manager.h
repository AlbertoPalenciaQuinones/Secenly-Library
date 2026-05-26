#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <string>

namespace secenly::internal {

class HardwareManager {
public:
    HardwareManager();
    const std::string GetHwid() const;

private:
    std::string ObtainHwid();

    std::string hwid;
};

}

#endif // HARDWARE_MANAGER_H