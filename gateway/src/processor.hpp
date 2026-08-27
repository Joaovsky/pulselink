#pragma once
#include "protocol.hpp"
#include <cstdint>
#include <unordered_map>

class Processor {
public:
    static Health classify(int16_t temperature, uint16_t vibration);
    void set_device_fd(int fd) { dev_fd_ = fd; }
    void handle(const Frame& f);
private:
    std::unordered_map<uint8_t, Health> last_;
    int dev_fd_ = -1;
};
