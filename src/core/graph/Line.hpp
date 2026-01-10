#pragma once
#include <cstdint>
#include <vector>

struct Line {
    std::uint32_t id;
    std::vector<std::uint32_t> stationIds;
};
