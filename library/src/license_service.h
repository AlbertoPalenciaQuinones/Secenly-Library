#ifndef LICENSE_SERVICE_H
#define LICENSE_SERVICE_H

#include <atomic>
#include <string>
#include <thread>

namespace secenly::internal {

class LicenseService {
public:
    static void ValidateLicenseInitial(std::string license_id, const std::chrono::system_clock::time_point& expiration_date, const std::string& expected_id);

    static void ValidateRuntime(const std::chrono::system_clock::time_point& expiration_date);

private:
    static bool CheckExpiration(const std::chrono::system_clock::time_point& expiration_date);
};

}

#endif // LICENSE_SERVICE_H