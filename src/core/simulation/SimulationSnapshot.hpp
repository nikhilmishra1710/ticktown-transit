#include "core/graph/SimulationSnapshot.hpp"
#include "core/world/Polyline.hpp"
#include <cstdint>
#include <map>

struct SimulationSnapshot {
    uint64_t tick;
    std::vector<StationView> stations;
    std::vector<TrainView> trains;
    std::vector<PassengerView> passengers;
    std::vector<LineView> lines;
    std::map<uint32_t, std::pair<float, float>> stationPositions;
    std::map<uint32_t, std::vector<Polyline>> linePaths;
    std::map<std::pair<uint32_t, uint32_t>, Polyline> edgePaths;
    std::map<std::uint32_t, std::pair<float, float>> trainPositions;
    std::uint32_t score;
};