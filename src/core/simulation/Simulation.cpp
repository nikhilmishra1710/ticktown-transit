#include "Simulation.hpp"
#include "core/graph/Graph.hpp"
#include "core/graph/SimulationSnapshot.hpp"
#include "core/graph/StationType.hpp"
#include "core/world/WorldGeometry.hpp"
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

Polyline Simulation::getOctilinearPath(Vector2 start, Vector2 end) const {
    return WorldGeometry::getOctilinearPath(start, end);
}

SimulationSnapshot Simulation::snapshot() const {
    SimulationSnapshot snap;
    GraphSnapshot graphSnap = this->graph_.snapshot();
    WorldSnapshot worldSnap = this->world_.snapshot();
    snap.tick = this->tickCount_;
    snap.stations = graphSnap.stations;
    snap.trains = graphSnap.trains;
    snap.passengers = graphSnap.passengers;
    snap.lines = graphSnap.lines;
    snap.score = graphSnap.score;
    snap.stationPositions = worldSnap.stationPositions;
    for (auto& train : graphSnap.trains) {
        if (train.state == TrainState::MOVING) {
            auto it1 = worldSnap.stationPositions.find(train.stationId);
            auto it2 = worldSnap.stationPositions.find(train.nextStationId);
            if (it1 != worldSnap.stationPositions.end() &&
                it2 != worldSnap.stationPositions.end()) {
                snap.trainPositions[train.id] = world_.getPositionOnEdge(
                    train.stationId, train.nextStationId, train.progress, train.forward);
            }
        } else {
            auto it = worldSnap.stationPositions.find(train.stationId);
            if (it != worldSnap.stationPositions.end()) {
                snap.trainPositions[train.id] = it->second;
            }
        }
    }

    for (auto& line : graphSnap.lines) {
        for (size_t i = 0; i + 1 < line.stationIds.size(); ++i) {
            auto key = std::make_pair(std::min(line.stationIds[i], line.stationIds[i + 1]),
                                      std::max(line.stationIds[i], line.stationIds[i + 1]));
            snap.linePaths[line.id].push_back(worldSnap.edgePaths.at(key));
        }
    }
    return snap;
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
                    auto id = graph_.addStationAtPosition(c.x, c.y);
                    world_.setStationPosition(id, {c.x, c.y});
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

                if constexpr (std::is_same_v<T, AddStationToLineCmd>) {
                    std::cout << "Adding station " << c.stationId << " to line " << c.lineId
                              << std::endl;
                    graph_.addStationToLineAtIndex(c.lineId, c.stationId, c.index);
                    if (c.startStationId != 0) {
                        Vector2 posA = world_.getStationPosition(c.startStationId);
                        Vector2 posB = world_.getStationPosition(c.stationId);
                        world_.updateEdge(c.startStationId, c.stationId, posA, posB);
                    }
                }

                if constexpr (std::is_same_v<T, AddTrainToLineCmd>) {
                    std::cout << "Adding train to line " << c.lineId << std::endl;
                    graph_.addTrain(c.lineId, 10, 0.1f);
                }
            },
            cmd);
    }
    pending_.clear();
}
