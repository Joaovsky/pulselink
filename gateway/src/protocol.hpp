#pragma once
#include <cstdint>
#include <array>
#include <optional>
#include <vector>

// Health status classificado pelo firmware.
enum class Health : uint8_t { NORMAL = 0, WARNING = 1, FAULT = 2 };

// O frame de telemetria (payload que vai nos dados do CAN).
struct Frame {
    uint8_t node_id;      // que máquina/nó
    int16_t  temperature;  // ºC * 100
    uint16_t vibration;    // unidade arbitrária
    Health   health;
};

// CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF). Mesmo esquema do DTx.
uint16_t crc16(const uint8_t* data, std::size_t len);

// Serializa Frame -> bytes (com CRC no fim).
std::vector<uint8_t> encode(const Frame& f);

// Desserializa bytes -> Frame. nullopt se o CRC não bater.
std::optional<Frame> decode(const uint8_t* data, std::size_t len);
