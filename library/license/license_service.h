#ifndef LICENSE_SERVICE_H
#define LICENSE_SERVICE_H

#include <cstdint>
#include <string>
#include <vector>

class LicenseService {
public:
    static std::string GenerateLicenseId(const std::string& path);
    void HeartbeatAlgorithm(int& beat);
    static void ValidateLicense(const std::vector<uint8_t>& data, 
        const std::string& seed_path, 
        const std::string& cert_path
    );
};

#endif // LICENSE_SERVICE_H