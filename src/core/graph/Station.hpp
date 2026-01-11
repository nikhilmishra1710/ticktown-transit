#pragma once
#include "StationType.hpp"
#include "Passenger.hpp"
#include <cstdint>
#include <vector>

struct Station {
    std::uint32_t id;
    StationType type;
    std::vector<Passenger> waitingPassengers;
};
