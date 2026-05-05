#ifndef LICENSE_H
#define LICENSE_H

#include <string>
#include <cstdint>
#include <chrono>

struct License {
    std::string id;
    std::chrono::system_clock::time_point creation_date;
    std::chrono::system_clock::time_point expiration_date;
    std::chrono::system_clock::time_point last_use_date;
    int32_t heartbeat_interval;
    std::string notes;
};

#endif // LICENSE_H