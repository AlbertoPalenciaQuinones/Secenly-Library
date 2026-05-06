#include "hardware_manager.h"

#include <array>
#include <cstdio>
#include <string>

// Obtiene automáticamente el HWID al construir el objeto para evitar llamadas 
// manuales posteriores.
HardwareManager::HardwareManager() : hwid(ObtainHwid()) {} 

// Ejecuta el comando de obtención de identificador de hardware.
std::string Exec(const std::string& cmd) {
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

// Ejecuta un comando u otro de obtención del hwid dependiendo del SO.
std::string HardwareManager::ObtainHwid() {
    //----------------------------------------
    //PROCEDIMIENTO DE OBTENCIÓN DE HWID
    std::string id;
    #ifdef __linux__
        // Se usa machine-id en Linux porque es persistente entre reinicios
        id = Exec("cat /etc/machine-id");
    #endif
    #ifdef _WIN32
        id = Exec("(Get-CimInstance Win32_ComputerSystemProduct).UUID");
    #endif
    #ifdef _WIN64
        id = Exec("(Get-CimInstance Win32_ComputerSystemProduct).UUID");
    #endif
    //FIN DE PROCEDIMIENTO OBTENCIÓN DE HWID
    //----------------------------------------
    return id;
}

std::string HardwareManager::GetHwid() const {
    return hwid;
}