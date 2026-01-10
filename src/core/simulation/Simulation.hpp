#pragma once
#include "../graph/Graph.hpp"
#include <chrono>
#include <cstdint>

class Simulation {
  public:
    explicit Simulation(std::uint64_t seed);

    void step(std::chrono::milliseconds dt);

    std::uint64_t stateHash() const;

  private:
    std::uint64_t tickCount_{0};
    std::uint64_t seed_;

    Graph _graph;
};
