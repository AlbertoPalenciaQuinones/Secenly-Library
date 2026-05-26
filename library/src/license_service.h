#ifndef LICENSE_SERVICE_H
#define LICENSE_SERVICE_H

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

#include "license.h"

namespace secenly::internal {

class LicenseService {
public:
    static void ValidateLicenseInitial(License& lic, const std::string& expected_id);

    static void ValidateRuntime(const License& lic);

private:
    std::atomic<bool> running{false};
    std::thread heartbeat_thread;

    static bool CheckExpiration(const std::chrono::system_clock::time_point& expiration_date);

    void StopHeartbeat();
    void StartHeartbeatAlgorithm(License& lic);

    static void AntiTamper();
};

}

#endif // LICENSE_SERVICE_H