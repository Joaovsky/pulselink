#pragma once
#include <cstdint>
#include <string>
#include <vector>

// Lê frames de um barramento SocketCAN (ex.: vcan0).
class CanReader {
public:
    bool open(const std::string& ifname);   // true se ligou
    // Bloqueia até chegar um frame; devolve os bytes de dados. false em erro.
    bool read_frame(std::vector<uint8_t>& out);
    void close_fd();
private:
    int fd_ = -1;
};

