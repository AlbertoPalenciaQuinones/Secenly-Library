#include "anti_tamper.h"

#include <fstream>
#include <stdexcept>
#include <chrono>
#include <cmath>


#include <iostream>
#include <ctime>
#include <cstdio>
#include <vector>
#include <cstdint>

namespace secenly::internal {

uint64_t AntiTamperManager::GetCurrentTime() {
    return std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now()
    );
}

uint64_t AntiTamperManager::ComputeSignature(uint64_t time) {
    const uint64_t SECRET = 0xA5F1236B9CDE7890ULL;
    return (time ^ SECRET) * 0x9E3779B97F4A7C15ULL;
}

uint64_t AntiTamperManager::LoadLastTime() {
    std::ifstream file("tamper_state.bin", std::ios::binary);

    if (!file) {
        return 0; // primera ejecución
    }

    uint64_t time = 0;
    uint64_t sig = 0;

    file.read(reinterpret_cast<char*>(&time), sizeof(time));
    file.read(reinterpret_cast<char*>(&sig), sizeof(sig));

    if (!file) {
        throw std::runtime_error("[TAMPER] Corrupted state file");
    }

    if (sig != ComputeSignature(time)) {
        throw std::runtime_error("[TAMPER] State file modified");
    }

    return time;
}

void AntiTamperManager::SaveLastTime(uint64_t time) {
    std::ofstream file("tamper_state.bin", std::ios::binary | std::ios::trunc);

    if (!file) {
        throw std::runtime_error("[TAMPER] Cannot write state file");
    }

    uint64_t sig = ComputeSignature(time);

    file.write(reinterpret_cast<const char*>(&time), sizeof(time));
    file.write(reinterpret_cast<const char*>(&sig), sizeof(sig));
}

void AntiTamperManager::CheckTimeTampering() {
    static auto last_system = std::chrono::system_clock::now();
    static auto last_steady = std::chrono::steady_clock::now();

    auto now_system = std::chrono::system_clock::now();
    auto now_steady = std::chrono::steady_clock::now();

    auto shift = std::chrono::duration_cast<std::chrono::seconds>(
        (now_system - last_system) - (now_steady - last_steady)
    );

    if (std::abs(shift.count()) > 2) {
        throw std::runtime_error("[TAMPER] Runtime clock manipulation detected");
    }

    last_system = now_system;
    last_steady = now_steady;
}

void AntiTamperManager::Validate() {
    uint64_t now = GetCurrentTime();

    CheckTimeTampering();

    uint64_t last = LoadLastTime();

    std::cout << "[DEBUG] last: " << last << std::endl;
    std::cout << "[DEBUG] now: " << now << std::endl;

    // Primera ejecución
    if (last == 0) {
        SaveLastTime(now);
        return;
    }

    // Detectar rollback
    if (now < last) {
        throw std::runtime_error("[TAMPER] System clock rollback detected");
    }

    // SOLO avanzar estado (clave)
    if (now > last) {
        SaveLastTime(now);
    }
}

}