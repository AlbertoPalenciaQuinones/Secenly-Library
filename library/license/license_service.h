#ifndef LICENSE_SERVICE_H
#define LICENSE_SERVICE_H

#include <cstdint>
#include <string>
#include <vector>

class LicenseService {
public:
    static std::string GenerateLicenseId(const std::string& path);
    void HeartbeatAlgorithm(int& beat);
    static void ValidateLicense(const std::vector<uint8_t>& data, std::string& seed_path);
};

#endif // LICENSE_SERVICE_H