#pragma once
#include <cstdint>

enum class StationType { Circle, Square, Triangle };

struct Station {
    std::uint32_t id;
    StationType type;
};
