#pragma once
#include "StationType.hpp"
#include <optional>
#include <stdexcept>

enum PassengerState { WAITING, ONTRAIN };

struct Passenger {
    StationType origin;
    StationType destination;
    PassengerState state;

    std::optional<std::uint32_t> currentLineId;
    std::optional<std::uint32_t> targetStationId;

    Passenger(StationType o, StationType d, PassengerState s)
        : origin(o), destination(d), state(s) {
        if (o == d) {
            throw std::logic_error("Passenger origin == destination");
        }
    }
};