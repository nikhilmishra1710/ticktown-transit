#include "core/world/World.hpp"
#include "core/world/WorldGeometry.hpp"
#include <iostream>
#include <stdexcept>

void World::updateEdge(uint32_t idA, uint32_t idB, bool needsBridge, Polyline path) {
    auto key = std::make_pair(std::min(idA, idB), std::max(idA, idB));
    if (needsBridge) {
        edgePaths[key].bridge = true;
        edgePaths[key] = path;
    } else {
        edgePaths[key].bridge = false;
        edgePaths[key] = path;
    }
    std::cout << "Updated edge between stations " << idA << " and " << idB << std::endl;
}

std::pair<float, float> World::getPositionOnEdge(uint32_t idA, uint32_t idB, float progress,
                                                 bool forward) const {
    auto key = std::make_pair(std::min(idA, idB), std::max(idA, idB));
    const auto& path = edgePaths.at(key);

    float targetDist = (forward ? progress : (1.0f - progress)) * path.totalLength;

    float accumulated = 0.0f;
    for (size_t i = 0; i < path.points.size() - 1; ++i) {
        float segLen = Vector2Distance(path.points[i], path.points[i + 1]);
        if (accumulated + segLen >= targetDist) {
            float segProgress = (targetDist - accumulated) / segLen;
            Vector2 pos = Vector2Lerp(path.points[i], path.points[i + 1], segProgress);
            return std::make_pair(pos.x, pos.y);
        }
        accumulated += segLen;
    }
    Vector2 pos = path.points.back();
    return std::make_pair(pos.x, pos.y);
}

void World::setStationPosition(uint32_t stationId, Vector2 pos) {
    stationPositions[stationId] = std::make_pair(pos.x, pos.y);
}

Vector2 World::getStationPosition(uint32_t stationId) const {
    auto it = stationPositions.find(stationId);
    if (it == stationPositions.end()) {
        throw std::logic_error("Station position not found");
    }
    return {it->second.first, it->second.second};
}

WorldSnapshot World::snapshot() const {
    WorldSnapshot snap;
    snap.edgePaths = this->edgePaths;
    snap.stationPositions = this->stationPositions;
    return snap;
}