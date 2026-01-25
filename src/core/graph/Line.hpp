#pragma once
#include <cstdint>
#include <vector>

using LineId = std::uint32_t;

struct Line {
    LineId id;
    std::vector<std::uint32_t> stationIds;
};
