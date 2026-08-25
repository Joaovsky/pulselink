#pragma once
#include <cstdint>

enum class Health : uint8_t { NORMAL = 0, WARNING = 1, FAULT = 2 };

class ControlFsm {
public:
    Health update(int16_t temp, uint16_t vib) {
        if (temp >= 8000 || vib >= 6000)      state_ = Health::FAULT;
        else if (temp >= 6000 || vib >= 3000) state_ = Health::WARNING;
        else                                  state_ = Health::NORMAL;
        return state_;
    }
    Health state() const { return state_; }
private:
    Health state_ = Health::NORMAL;
};
