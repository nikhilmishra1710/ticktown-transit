#pragma once
#include <unordered_map>
#include <vector>
#include "core/graph/id.hpp"
#include "raylib.h"

struct WorldStation {
    StationId id;
    Vector2 position;
};

struct WorldLine {
    uint32_t lineId;
    std::vector<StationId> stations;
};

class World {
public:
    StationId addStation(Vector2 pos);
    void addStationToLine(uint32_t lineId, StationId station);

    const WorldStation& station(StationId id) const;
    float distance(StationId a, StationId b) const;

private:
    std::unordered_map<StationId, WorldStation> stations_;
};
