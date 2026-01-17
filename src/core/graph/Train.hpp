#pragma once
#include "Passenger.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

struct Train {
    std::uint32_t lineId;
    std::size_t stationIndex;
    int direction;
    std::vector<Passenger> onboard;
    std::size_t capacity;
};