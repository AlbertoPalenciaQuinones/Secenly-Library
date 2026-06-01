#include "hardware_manager.h"

#include <array>
#include <iostream>

#include "license_exception.h"

namespace {
    // Ejecuta el comando de obtención de identificador de hardware.
    std::string Exec(const std::string& cmd) {
        std::array<char, 128> buffer{};
        std::string result;

        FILE* pipe = popen(cmd.c_str(), "r");
        
        if (!pipe) {
            throw secenly::internal::LicenseException(
                secenly::internal::LicenseError::HardwareIdGenerationFailed,
                "[HWID] Failed to execute command for get HWID"
            );
        }

        while (fgets(buffer.data(), buffer.size(), pipe)) {
            result += buffer.data();
        }

        pclose(pipe);

        if (!result.empty() && result.back() == '\n')
            result.pop_back();

        return result;
    }
}

namespace secenly::internal {

// Obtiene automáticamente el HWID al construir el objeto para evitar llamadas 
// manuales posteriores.
HardwareManager::HardwareManager() : hwid(ObtainHwid()) {} 

/*
 * Genera el identificador del hardware.
 *
 * El comportamiento actual es la ejecución de un comando.
 *
 * Este comportamiento puede ser modificado por cualquiera que utilice la 
 * biblioteca con el fin de que sea ajustable a las necesidades de cada usuario.
 *
 * La función cumple con las siguientes necesidades:
 *     - Generación de identificador único
 *     - Distinto comando para Linux y Windows
 * 
 * Debe saber que puede añadir distintos procedimientos escribiendo nuevas
 * funciones, pero todas ellas deben funcionar de forma correcta para no
 * romper el mecanismo de obtención del identificador.
 * 
 * Otro aspecto a tener en cuenta es que la modificación en Secenly-Library
 * de la generación del identificador de hardware crea la necesidad de su
 * modificación en la herramienta Secenly, siempre y cuando se haya optado
 * utilizarla a la hora de generar licencias de software.
 */
std::string HardwareManager::ObtainHwid() {
    std::string id;
    #ifdef __linux__
        // Se usa machine-id en Linux porque es persistente entre reinicios
        id = Exec("cat /etc/machine-id");
    #endif
    #ifdef _WIN32
        id = Exec("powershell -Command \"(Get-CimInstance Win32_ComputerSystemProduct).UUID\"");
    #endif
    return id;
}

const std::string HardwareManager::GetHwid() const {
    return hwid;
}

}