#pragma once
#include <cstdint>

struct LevelConfig {
    uint32_t initialStations;
    uint32_t initialLines;
    uint32_t initialTrainsPerLine;
    uint64_t randomSeed;
    uint32_t initialPassengers;
};
