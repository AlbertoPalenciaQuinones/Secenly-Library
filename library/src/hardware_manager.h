#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <string>

class HardwareManager {
public:
    HardwareManager();
    const std::string GetHwid() const;

private:
    std::string ObtainHwid();

    std::string hwid;
};

#endif // HARDWARE_MANAGER_H