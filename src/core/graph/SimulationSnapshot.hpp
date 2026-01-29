#pragma once
#include "Train.hpp"
#include "core/graph/Passenger.hpp"
#include "core/graph/StationType.hpp"
#include "id.hpp"
#include <optional>
#include <vector>

struct PassengerView {
    uint32_t id;
    StationId origin;
    StationType destination;
    PassengerState state;
    std::optional<uint32_t> stationId;
    std::optional<uint32_t> trainId;
    std::size_t age;
};

struct TrainView {
    uint32_t id;
    uint32_t lineId;
    uint32_t stationId;
    bool forward;
    std::size_t capacity;
    std::size_t onboard;
    TrainState state;
};

struct StationView {
    uint32_t id;
    StationType type;
    std::size_t waiting;
};

struct SimulationSnapshot {
    uint64_t tick;
    std::vector<StationView> stations;
    std::vector<TrainView> trains;
    std::vector<PassengerView> passengers;
};
