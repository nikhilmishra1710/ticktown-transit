#include "Simulation.hpp"

Simulation::Simulation(std::uint64_t seed) : seed_(seed) {}

void Simulation::step(std::chrono::milliseconds) {
    ++tickCount_;
}

std::uint64_t Simulation::stateHash() const {
    return tickCount_ ^ seed_;
}
