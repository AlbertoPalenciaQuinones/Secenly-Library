#ifndef LICENSE_MANAGER_H
#define LICENSE_MANAGER_H

class LicenseManager {
public:
    LicenseManager();
    std::string generateLicenseId(const std::string& licenseId);

private:
    std::string generateHash(const std::string& productId, size_t inicio, size_t fin);
    unsigned char hexCharToValue(char c);
    std::vector<unsigned char> hexToBytes(const std::string& hex);
    std::string bytesToHex(const std::vector<unsigned char>& bytes);
    std::vector<unsigned char> xorBuffers(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b);
};

#endif // LICENSE_MANAGER_H


