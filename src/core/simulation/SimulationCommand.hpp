#pragma once
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

using SimulationCommand = std::variant<AddStationCmd, AddLineCmd, AddPassengerCmd>;