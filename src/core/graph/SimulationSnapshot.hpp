#pragma once
#include "Passenger.hpp"
#include "StationType.hpp"
#include "Train.hpp"
#include "id.hpp"
#include <optional>
#include <vector>

struct PassengerView {
    uint32_t id;
    StationId origin;
    StationType destination;
    PassengerState state;
    std::optional<StationId> stationId;
    std::optional<TrainId> trainId;
    std::size_t age;
};

struct TrainView {
    uint32_t id;
    uint32_t lineId;
    uint32_t stationId;
    uint32_t nextStationId;
    bool forward;
    std::size_t capacity;
    std::size_t onboard;
    TrainState state;
    float progress;
    std::vector<PassengerView> passengers;
};

struct StationView {
    uint32_t id;
    StationType type;
    std::size_t waiting;
    std::vector<PassengerView> passengers;
};

struct LineView {
    uint32_t id;
    std::vector<uint32_t> stationIds;
};

struct GraphSnapshot {
    uint64_t tick;
    std::vector<StationView> stations;
    std::vector<TrainView> trains;
    std::vector<PassengerView> passengers;
    std::vector<LineView> lines;
    std::uint32_t score;
};
