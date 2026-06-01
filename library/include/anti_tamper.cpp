#include "anti_tamper.h"

#include <chrono>
#include <fstream>
#include <stdexcept>

#include "license_exception.h"

namespace secenly::internal {

// Cálculo de una firma simple para detectar modificaciones del estado
uint64_t AntiTamperManager::ComputeSignature(uint64_t time) {
    const uint64_t SECRET = 0xA5F1236B9CDE7890ULL;
    // Mezcla el tiempo con una constante secreta y un número primo grande
    return (time ^ SECRET) * 0x9E3779B97F4A7C15ULL;
}

// Cargar el último tiempo conocido para detectar rollbacks
uint64_t AntiTamperManager::LoadLastTime() {
    std::ifstream file("tamper_state.bin", std::ios::binary);

    // Primera ejecución
    if (!file) {
        return 0; 
    }

    uint64_t time = 0;
    uint64_t sig = 0;

    // Leer el tiempo y la firma del archivo
    file.read(reinterpret_cast<char*>(&time), sizeof(time));
    file.read(reinterpret_cast<char*>(&sig), sizeof(sig));

    // Verificar el archivo de huella
    if (!file) {
        throw LicenseException(
            LicenseError::CorruptedState,
            "[TAMPER] Corrupted state file. Clossing application..."
        );

    }

    // Verificar la firma para detectar modificaciones del archivo
    if (sig != ComputeSignature(time)) {
        throw LicenseException(
            LicenseError::CorruptedState,
            "[TAMPER] State file modified. Clossing application..."
        );
    }

    return time;
}

// Guardar el último tiempo conocido para detectar rollbacks
void AntiTamperManager::SaveLastTime(uint64_t time) {
    // Guardar el tiempo y su firma en un archivo binario
    std::ofstream file("tamper_state.bin", std::ios::binary | std::ios::trunc);

    if (!file) {
        throw LicenseException(
            LicenseError::FileIOError,
            "[TAMPER] Failed to save state"
        );
    }

    uint64_t sig = ComputeSignature(time);

    // Escribir el tiempo y la firma en el archivo
    file.write(reinterpret_cast<const char*>(&time), sizeof(time));
    file.write(reinterpret_cast<const char*>(&sig), sizeof(sig));
}

/*
 * Osquestación de manipulación en tiempo de ejecución (anti-tamper, rollback)
 *
 * El comportamiento actual es comparando el desplazamiento entre el reloj del
 * sistema y un reloj monotónico. Si el desplazamiento es mayor a 2 segundos, se
 * considera una manipulación del reloj. Este método es simple pero efectivo para
 * detectar intentos de manipulación del tiempo en tiempo de ejecución.
 *
 * Este comportamiento puede ser modificado por cualquiera que utilice la 
 * biblioteca con el fin de que sea ajustable a las necesidades de cada usuario.
 *
 * La función cumple con las siguientes necesidades:
 *     - Utilización de reloj del sistema (manipulable)
 *     - Utilización de reloj monotónico (no manipulable)
 *     - Detección de desplazamientos significativos entre ambos relojes
 *     - Detección de manipulación mientras corre el proceso
 * 
 * Debe saber que puede añadir distintos procedimientos escribiendo nuevas
 * funciones, pero todas ellas deben funcionar de forma correcta para no
 * romper el mecanismo de detección de manipulación en tiempo de ejecución.
 * 
 * Las funciones de validación en tiempo de ejecución y de campos de la licencia
 * son meramente orientativas. El usuario puede modificar el comportamiento.
 */
void AntiTamperManager::CheckTamper() {
    static auto last_system = std::chrono::system_clock::now();
    static auto last_steady = std::chrono::steady_clock::now();

    auto now_system = std::chrono::system_clock::now();
    auto now_steady = std::chrono::steady_clock::now();

    auto shift = std::chrono::duration_cast<std::chrono::seconds>(
        (now_system - last_system) - (now_steady - last_steady)
    );

    if (std::abs(shift.count()) > 2) {
        throw LicenseException(
            LicenseError::RuntimeTamperingDetected,
            "[TAMPER] Runtime clock manipulation detected. Clossing application..."
        );
    }

    last_system = now_system;
    last_steady = now_steady;
}

/*
 * Función principal de validación anti-tamper
 *
 * El comportamiento actual es utilizar la función del mecanismo anti-tamper
 * mientras corre el proceso y comparar/generar la huella de tiempo para
 * detectar rollbacks. Este método es simple pero efectivo para detectar intentos
 * de manipulación del reloj en tiempo de ejecución.
 *
 * Este comportamiento puede ser modificado por cualquiera que utilice la 
 * biblioteca con el fin de que sea ajustable a las necesidades de cada usuario.
 *
 * La función cumple con las siguientes necesidades:
 *     - Orquestación de la validación en tiempo de ejecución
 *     - Llamada a la función de generar huella de tiempo
 *     - Detección de rollbacks comparando el tiempo actual con el último tiempo
 *     - Actualización del último tiempo conocido si el tiempo actual es mayor
 * 
 * Debe saber que puede añadir distintos procedimientos escribiendo nuevas
 * funciones, pero todas ellas deben funcionar de forma correcta para no
 * romper el mecanismo de detección de manipulación en tiempo de ejecución.
 * 
 * Las funciones de validación en tiempo de ejecución y de campos de la licencia
 * son meramente orientativas. El usuario puede modificar el comportamiento.
 */
void AntiTamperManager::Validate() {
    uint64_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Detección de manipulación del reloj en tiempo de ejecución
    CheckTamper();

    uint64_t last = LoadLastTime();

    // Primera ejecución
    if (last == 0) {
        SaveLastTime(now);
        return;
    }

    // Detectar rollback
    if (now < last) {
        throw LicenseException(
            LicenseError::ClockRollbackDetected,
            "[TAMPER] System clock rollback detected. Clossing application..."
        );
    }

    // Actualizar el último tiempo conocido si el tiempo actual es mayor
    if (now > last) {
        SaveLastTime(now);
    }
}

}