#include <cstdio>
#include <string>
#include "hardware_manager.h"
#include <array>

HardwareManager::HardwareManager() {}

bool HardwareManager::initialize() {
    hwid = obtainHwid();
    return !hwid.empty();
}

std::string HardwareManager::exec(const std::string& cmd) {
    std::array<char, 128> buffer{};
    std::string result;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe)) {
        result += buffer.data();
    }

    pclose(pipe);

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

std::string HardwareManager::obtainHwid() {
    //----------------------------------------
    //PROCEDIMIENTO DE OBTENCIÓN DE HWID
    std::string hwid;
    #ifdef __linux__
        hwid = exec("cat /etc/machine-id");
    #endif
    #ifdef _WIN32
        hwid = exec("(Get-CimInstance Win32_ComputerSystemProduct).UUID");
    #endif
    #ifdef _WIN64
        hwid = exec("(Get-CimInstance Win32_ComputerSystemProduct).UUID");
    #endif
    //FIN DE PROCEDIMIENTO OBTENCIÓN DE HWID
    //----------------------------------------
    return hwid;
}

std::string HardwareManager::getHwid() {
    return hwid;
}