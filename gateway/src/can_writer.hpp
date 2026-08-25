#pragma once
#include <cstdint>
#include <string>
#include <vector>

class CanWriter {
public:
    bool open(const std::string& ifname);
    bool send(uint32_t can_id, const std::vector<uint8_t>& data);
    void close_fd();
private:
    int fd_ = -1;
};