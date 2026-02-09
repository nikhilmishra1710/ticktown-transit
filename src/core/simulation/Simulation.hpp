#pragma once
#include "core/graph/Graph.hpp"
#include "core/simulation/SimulationCommand.hpp"
#include "core/simulation/SimulationSnapshot.hpp"
#include "core/simulation/TickClock.hpp"
#include "core/world/Polyline.hpp"
#include "core/world/World.hpp"
#include <chrono>
#include <cstdint>
#include <random>
#include <set>

class Simulation {
  public:
    explicit Simulation(std::uint64_t seed);

    void enqueueCommand(SimulationCommand cmd);
    void step(std::chrono::milliseconds dt);
    void addRiver(const std::vector<std::pair<float, float>>& points, float width);

    Polyline getOctilinearPath(Vector2 start, Vector2 end) const;

    std::uint64_t stateHash() const;
    SimulationSnapshot snapshot() const;

  private:
    void _applyCommands();
    std::vector<StationType> getExistingStationTypes() const;

    int availableBridges_ = 3; // Starting bridges
    std::vector<SimulationCommand> pending_;
    std::uint64_t tickCount_{0};
    std::uint64_t seed_;
    std::mt19937 rng_;
    float spawnAccumulator_ = 0.0f;
    float baseSpawnInterval_ = 0.2f; // Seconds between spawns

    TickClock clock_{std::chrono::milliseconds(1000)};
    std::map<std::uint32_t, std::pair<Polyline, float>> rivers_; // Loaded from JSON
    std::set<std::pair<uint32_t, uint32_t>> bridgedEdges_;
    Graph graph_;
    World world_;
};
