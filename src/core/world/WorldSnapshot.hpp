#pragma once
#include "core/world/Polyline.hpp"
#include <cstdint>
#include <map>

struct WorldSnapshot {
    std::map<uint32_t, std::pair<float, float>> stationPositions;
    std::map<std::pair<uint32_t, uint32_t>, Polyline> edgePaths;
    std::map<std::uint32_t, std::pair<float, float>> trainPositions;
};