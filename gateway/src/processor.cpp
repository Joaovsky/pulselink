#include "processor.hpp"
#include "logger.hpp"
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <string>

Health Processor::classify(int16_t temperature, uint16_t vibration) {
    if (temperature >= 8000 || vibration >= 6000) return Health::FAULT;
    if (temperature >= 6000 || vibration >= 3000) return Health::WARNING;
    return Health::NORMAL;
}

static const char* to_str(Health h) {
    switch (h) {
        case Health::NORMAL:  return "NORMAL";
        case Health::WARNING: return "WARNING";
        case Health::FAULT:   return "FAULT";
    }
    return "?";
}

void Processor::handle(const Frame& f) {
    const Health gw = classify(f.temperature, f.vibration);

    char buf[128];
    std::snprintf(buf, sizeof(buf), "node=%u temp=%.2fC vib=%u health=%s",
                  static_cast<unsigned>(f.node_id), f.temperature / 100.0,
                  static_cast<unsigned>(f.vibration), to_str(gw));

    const Level lvl = gw == Health::FAULT ? Level::ERROR
                    : gw == Health::WARNING ? Level::WARN : Level::INFO;
    log(lvl, buf);

    if (gw != f.health)
        log(Level::WARN, std::string("  mismatch: firmware said ") + to_str(f.health)
                         + ", gateway says " + to_str(gw));

    auto it = last_.find(f.node_id);
    if (it != last_.end() && it->second != gw)
        log(lvl, std::string("  transition node=") + std::to_string(f.node_id) + ": "
                 + to_str(it->second) + " -> " + to_str(gw));

    if (dev_fd_ >= 0) {
        std::string line = std::string(buf) + "\n";
        ssize_t w = write(dev_fd_, line.c_str(), line.size());
        (void)w;
    }
                     
    last_[f.node_id] = gw;
}
