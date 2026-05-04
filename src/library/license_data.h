/*
#define LICENSE_H

#include <string>
#include <vector>
#include <ctime>
#include <cstdint>

class License {
public:
    std::string id;
    std::string product;

    std::time_t created_at;
    std::time_t expires_at;
    std::time_t last_use_at;

    std::string hwid;
    int heartbeat_interval;

    std::string hash_algorithm;
    std::string notes;

    License();

    bool generate_binary(const std::string& filename);

private:
    std::vector<uint8_t> serialize();
};
*/