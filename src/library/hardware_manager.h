#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <string>

class HardwareManager {
public:
    HardwareManager();
    bool initialize();
    std::string getHwid();

private:
    std::string obtainHwid();
    std::string exec(const std::string& cmd);

    std::string hwid;
};

#endif // HARDWARE_MANAGER_H