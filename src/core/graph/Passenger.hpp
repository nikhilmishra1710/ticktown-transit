#pragma once
#include "StationType.hpp"
#include "id.hpp"
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

enum class PassengerState { WAITING, ON_TRAIN, TRANSFERRING, COMPLETED };

using PassengerId = std::uint32_t;

struct Passenger {
    PassengerId passengerId;
    StationId source;
    StationType destination;
    PassengerState state;

    std::optional<TrainId> train;
    std::optional<StationId> station;

    std::vector<StationId> committedRoute;
    std::size_t routeIndex = 0;

    std::size_t age = 0;
    std::optional<StationId> nextHop;

    std::uint32_t lastStationId = UINT32_MAX;
    std::optional<std::uint32_t> currentLineId;
    std::optional<std::uint32_t> targetStationId;

    Passenger(std::uint32_t id, StationType o, StationType d, PassengerState s)
        : source(id), passengerId(id), station(o), destination(d), state(s) {
        if (o == d) {
            throw std::logic_error("Passenger origin == destination");
        }
    }
};
