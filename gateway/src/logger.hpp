#pragma once
#include <iostream>
#include <string>

enum class Level { INFO, WARN, ERROR };

inline void log(Level lvl, const std::string& msg) {
    const char* tag = lvl == Level::ERROR ? "[ERROR]"
                    : lvl == Level::WARN  ? "[WARN] " : "[INFO] ";
    std::cout << tag << ' ' << msg << '\n';
}
