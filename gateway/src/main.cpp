#include "can_reader.hpp"
#include "logger.hpp"
#include "processor.hpp"
#include "protocol.hpp"
#include "telemetry_queue.hpp"
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    const std::string ifname = (argc > 1) ? argv[1] : "vcan0";
    CanReader reader;
    if (!reader.open(ifname)) {
        std::cerr << "ERROR: cannot open " << ifname << " (vcan0 up? sudo?)\n";
        return 1;
    }
    log(Level::INFO, "Gateway listening on " + ifname);

    TelemetryQueue<std::vector<uint8_t>> queue;

    std::thread producer([&] {
        std::vector<uint8_t> bytes;
        while (reader.read_frame(bytes)) queue.push(bytes);
        queue.close();
    });

    Processor proc;
    std::thread consumer([&] {
        while (auto item = queue.wait_pop()) {
            auto frame = decode(item->data(), item->size());
            if (!frame) { log(Level::WARN, "[DROP] invalid frame (CRC fail/bad length)"); continue; }
            proc.handle(*frame);
        }
    });

    producer.join();
    consumer.join();
    return 0;
}
