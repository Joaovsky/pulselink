#include "can_writer.hpp"
#include "protocol.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char** argv) {
    const std::string ifname = (argc > 1) ? argv[1] : "vcan0";
    CanWriter w;
    if (!w.open(ifname)) { std::cerr << "cannot open " << ifname << " (sudo?)\n"; return 1; }
    std::cout << "Injecting telemetry on " << ifname << " (Ctrl+C to stop)\n";

    int16_t temp = 2000;                       // 20.00C
    while (true) {
        const uint16_t vib = static_cast<uint16_t>(temp / 4);
        Frame f{1, temp, vib, Health::NORMAL};  // sensor "burro": reporta sempre NORMAL
        w.send(0x123, encode(f));
        temp = static_cast<int16_t>(temp + 250);       // aquece 2.5C por ciclo
        if (temp > 9000) temp = 2000;                  // recomeça
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
