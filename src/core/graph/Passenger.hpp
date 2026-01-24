#pragma once
#include "StationType.hpp"
#include <cstdint>
#include <optional>
#include <stdexcept>

enum PassengerState { WAITING, ONTRAIN };

struct Passenger {
    std::uint32_t passengerId;
    StationType origin;
    StationType destination;
    PassengerState state;
    std::uint32_t lastStationId = UINT32_MAX;

    std::size_t waitingTicks = 0;
    std::optional<std::uint32_t> currentLineId;
    std::optional<std::uint32_t> targetStationId;

    Passenger(std::uint32_t id, StationType o, StationType d, PassengerState s)
        : passengerId(id), origin(o), destination(d), state(s) {
        if (o == d) {
            throw std::logic_error("Passenger origin == destination");
        }
    }
};