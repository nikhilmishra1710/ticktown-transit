#pragma once
#include "Passenger.hpp"
#include "id.hpp"
#include <cstddef>
#include <vector>

enum class TrainState { ALIGHTING, BOARDING, MOVING };

struct Train {
    TrainId trainId;
    LineId lineId;
    StationId currentStationId;
    StationId nextStationId;
    TrainState state;
    std::size_t stationIndex;
    
    int direction;
    std::vector<Passenger> onboard;
    std::size_t capacity;
    
    float progress = 0.0f;
    float speed = 1.0f;
};