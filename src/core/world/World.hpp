#include "core/world/Polyline.hpp"
#include "core/world/WorldSnapshot.hpp"
#include <cstdint>
#include <map>

class World {
  public:
    std::map<std::pair<uint32_t, uint32_t>, Polyline> edgePaths;
    std::map<uint32_t, std::pair<float, float>> stationPositions;

    void setStationPosition(uint32_t stationId, Vector2 pos);
    Vector2 getStationPosition(uint32_t stationId) const;

    void updateEdge(uint32_t idA, uint32_t idB, bool needsBridge, Polyline path);
    std::pair<float, float> getPositionOnEdge(uint32_t idA, uint32_t idB, float progress,
                                              bool forward) const;

    WorldSnapshot snapshot() const;
};