#include "can_reader.hpp"
#include "protocol.hpp"
#include "telemetry_queue.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

static const char* to_str(Health h) {
    switch (h) {
        case Health::NORMAL:  return "NORMAL";
        case Health::WARNING: return "WARNING";
        case Health::FAULT:   return "FAULT";
    }
    return "?";
}

int main(int argc, char** argv) {
    const std::string ifname = (argc > 1) ? argv[1] : "vcan0";
    CanReader reader;
    if (!reader.open(ifname)) {
        std::cerr << "ERROR: cannot open " << ifname << " (vcan0 up? sudo?)\n";
        return 1;
    }
    std::cout << "Gateway listening on " << ifname << " ...\n";

    TelemetryQueue<std::vector<uint8_t>> queue;

    // PRODUTOR: lê CAN -> mete bytes crus na fila.
    std::thread producer([&] {
        std::vector<uint8_t> bytes;
        while (reader.read_frame(bytes)) queue.push(bytes);
        queue.close();
    });

    // CONSUMIDOR: tira da fila -> valida CRC -> classifica -> imprime.
    std::thread consumer([&] {
        while (auto item = queue.wait_pop()) {
            auto frame = decode(item->data(), item->size());
            if (!frame) {
                std::cout << "  [DROP] invalid frame (CRC fail or bad length)\n";
                continue;
            }
            std::cout << "  node=" << static_cast<int>(frame->node_id)
                      << " temp=" << frame->temperature / 100.0 << "C"
                      << " vib=" << frame->vibration
                      << " health=" << to_str(frame->health) << '\n';
        }
    });

    producer.join();
    consumer.join();
    return 0;
}
