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
    b.push_back(f.node_id);
    b.push_back(static_cast<uint8_t>(f.temperature >> 8));
    b.push_back(static_cast<uint8_t>(f.temperature & 0xFF));
    b.push_back(static_cast<uint8_t>(f.vibration >> 8));
    b.push_back(static_cast<uint8_t>(f.vibration & 0xFF));
    b.push_back(static_cast<uint8_t>(f.health));
    const uint16_t c = crc16(b.data(), b.size());
    b.push_back(static_cast<uint8_t>(c >> 8));
    b.push_back(static_cast<uint8_t>(c & 0xFF));
    return b; // 8 bytes: 6 payload + 2 CRC
}

std::optional<Frame> decode(const uint8_t* data, std::size_t len) {
    if (len != 8) return std::nullopt;

    const uint16_t got =
        static_cast<uint16_t>(data[6] << 8 | data[7]);

    if (crc16(data, 6) != got)
        return std::nullopt;

    Frame f;
    f.node_id     = data[0];
    f.temperature = static_cast<int16_t>(data[1] << 8 | data[2]);
    f.vibration   = static_cast<uint16_t>(data[3] << 8 | data[4]);
    f.health      = static_cast<Health>(data[5]);

    return f;
}
