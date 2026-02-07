#pragma once
#include "core/graph/Graph.hpp"
#include "core/simulation/SimulationCommand.hpp"
#include "core/simulation/SimulationSnapshot.hpp"
#include "core/simulation/TickClock.hpp"
#include "core/world/Polyline.hpp"
#include "core/world/World.hpp"
#include <chrono>
#include <cstdint>

class Simulation {
  public:
    explicit Simulation(std::uint64_t seed);

    void enqueueCommand(SimulationCommand cmd);
    void step(std::chrono::milliseconds dt);

    Polyline getOctilinearPath(Vector2 start, Vector2 end) const;

    std::uint64_t stateHash() const;
    SimulationSnapshot snapshot() const;

  private:
    void _applyCommands();

    std::vector<SimulationCommand> pending_;
    std::uint64_t tickCount_{0};
    std::uint64_t seed_;

    TickClock clock_{std::chrono::milliseconds(1000)};
    Graph graph_;
    World world_;
};
