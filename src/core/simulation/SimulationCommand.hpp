#pragma once
#include "core/graph/StationType.hpp"
#include <cstddef>
#include <cstdint>
#include <variant>

struct AddStationCmd {
    float x;
    float y;
    StationType type = StationType::CIRCLE;
};

struct AddLineCmd {
    uint32_t lineId;
};

struct AddPassengerCmd {
    uint32_t stationId;
    StationType destinationType;
};

struct AddStationToLineCmd {
    uint32_t lineId;
    uint32_t stationId;
    uint32_t startStationId;
    size_t index;
};

struct AddTrainToLineCmd {
    uint32_t lineId;
};

using SimulationCommand = std::variant<AddStationCmd, AddLineCmd, AddPassengerCmd,
                                       AddStationToLineCmd, AddTrainToLineCmd>;