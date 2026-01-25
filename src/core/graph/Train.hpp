#pragma once
#include "Passenger.hpp"
#include "id.hpp"
#include <cstddef>
#include <vector>

struct Train {
    TrainId trainId;
    LineId lineId;
    std::size_t stationIndex;
    int direction;
    std::vector<Passenger> onboard;
    std::size_t capacity;
};