#pragma once
#include "Passenger.hpp"
#include "StationType.hpp"
#include "id.hpp"
#include <vector>

struct Station {
    StationId id;
    StationType type;
    std::vector<Passenger> waitingPassengers;

    float x = 0.0f;
    float y = 0.0f;
    std::size_t maxCapacity = 6;
};
