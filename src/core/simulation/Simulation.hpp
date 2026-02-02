#pragma once
#include "core/graph/Graph.hpp"
#include "core/graph/SimulationSnapshot.hpp"
#include "core/simulation/SimulationCommand.hpp"
#include "core/simulation/TickClock.hpp"
#include <chrono>
#include <cstdint>

class Simulation {
  public:
    explicit Simulation(std::uint64_t seed);

    void enqueueCommand(SimulationCommand cmd);
    void step(std::chrono::milliseconds dt);

    std::uint64_t stateHash() const;
    SimulationSnapshot snapshot() const;
  private:
    void _applyCommands();

    std::vector<SimulationCommand> pending_;
    std::uint64_t tickCount_{0};
    std::uint64_t seed_;

    TickClock clock_{std::chrono::milliseconds(1000)};
    Graph graph_;
};
