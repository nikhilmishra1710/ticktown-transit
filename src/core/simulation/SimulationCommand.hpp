#pragma once
#include <cstddef>
#include <cstdint>
#include <variant>

struct AddStationCmd {
    float x;
    float y;
};

struct AddLineCmd {
    uint32_t lineId;
};

struct AddPassengerCmd {
    uint32_t stationId;
};

struct AddStationToLineCmd {
    uint32_t lineId;
    uint32_t stationId;
    size_t index;
};

struct AddTrainToLineCmd {
    uint32_t lineId;
};

using SimulationCommand = std::variant<AddStationCmd, AddLineCmd, AddPassengerCmd,
                                       AddStationToLineCmd, AddTrainToLineCmd>;