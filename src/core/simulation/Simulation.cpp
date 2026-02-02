#include "Simulation.hpp"
#include "core/graph/SimulationSnapshot.hpp"
#include <iostream>

Simulation::Simulation(std::uint64_t seed) : seed_(seed) {
}

void Simulation::step(std::chrono::milliseconds dt) {
    if (!clock_.shouldStep(dt)) {
        return;
    }
    clock_.consumeStep();
    ++tickCount_;
    this->_applyCommands();
    this->graph_.tick();
}

SimulationSnapshot Simulation::snapshot() const {
    return this->graph_.snapshot();
}

void Simulation::enqueueCommand(SimulationCommand cmd) {
    pending_.push_back(cmd);
}

std::uint64_t Simulation::stateHash() const {
    return tickCount_ ^ seed_;
}

void Simulation::_applyCommands() {
    for (auto& cmd : pending_) {
        std::visit(
            [&](auto&& c) {
                using T = std::decay_t<decltype(c)>;

                if constexpr (std::is_same_v<T, AddStationCmd>) {
                    graph_.addStationAtPosition(c.x, c.y);
                }

                if constexpr (std::is_same_v<T, AddLineCmd>) {
                    graph_.addLine();
                }

                if constexpr (std::is_same_v<T, AddPassengerCmd>) {
                    std::cout << "Spawning passenger at station " << c.stationId
                              << " with destination type " << c.stationId + 1 << std::endl;
                    StationType destType;
                    try {
                        destType = static_cast<StationType>(
                            (static_cast<int>(graph_.getStation(c.stationId)->type) + 1) % 3);

                    } catch (const std::exception& e) {
                        std::cerr << "Exception: " << e.what() << std::endl;
                    }

                    graph_.spawnPassengerAt(c.stationId, destType);
                }
            },
            cmd);
    }
    pending_.clear();
}
