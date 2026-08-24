#include "protocol.hpp"

uint16_t crc16(const uint8_t* data, std::size_t len) {
    uint16_t crc = 0xFFFF;
    for (std::size_t i = 0; i < len; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int b = 0; b < 8; ++b)
            crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                                 : static_cast<uint16_t>(crc << 1);
    }
    return crc;
}

std::vector<uint8_t> encode(const Frame& f) {
    std::vector<uint8_t> b;
    b.push_back(static_cast<uint8_t>(f.node_id >> 8));
    b.push_back(static_cast<uint8_t>(f.node_id & 0xFF));
    b.push_back(static_cast<uint8_t>(f.temperature >> 8));
    b.push_back(static_cast<uint8_t>(f.temperature & 0xFF));
    b.push_back(static_cast<uint8_t>(f.vibration >> 8));
    b.push_back(static_cast<uint8_t>(f.vibration & 0xFF));
    b.push_back(static_cast<uint8_t>(f.health));
    const uint16_t c = crc16(b.data(), b.size());
    b.push_back(static_cast<uint8_t>(c >> 8));
    b.push_back(static_cast<uint8_t>(c & 0xFF));
    return b;                              // 9 bytes: 7 payload + 2 CRC
}

std::optional<Frame> decode(const uint8_t* data, std::size_t len) {
    if (len != 9) return std::nullopt;                 // tamanho errado
    const uint16_t got = static_cast<uint16_t>(data[7] << 8 | data[8]);
    if (crc16(data, 7) != got) return std::nullopt;    // CRC não bate
    Frame f;
    f.node_id     = static_cast<uint16_t>(data[0] << 8 | data[1]);
    f.temperature = static_cast<int16_t>(data[2] << 8 | data[3]);
    f.vibration   = static_cast<uint16_t>(data[4] << 8 | data[5]);
    f.health      = static_cast<Health>(data[6]);
    return f;
}
