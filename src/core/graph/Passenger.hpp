#pragma once
#include "StationType.hpp"
#include "id.hpp"
#include <cstdint>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <vector>

enum class PassengerState { WAITING, ON_TRAIN, TRANSFERRING, COMPLETED };

using PassengerId = std::uint32_t;

struct Passenger {
    PassengerId passengerId;
    StationId source;
    StationType destination;
    PassengerState state;

    std::optional<TrainId> train;
    std::optional<StationId> station;

    std::vector<StationId> committedRoute;
    std::size_t routeIndex = 0;

    std::size_t age = 0;
    std::optional<StationId> nextHop;

    std::uint32_t lastStationId = UINT32_MAX;
    std::optional<std::uint32_t> currentLineId;
    std::optional<std::uint32_t> targetStationId;

    Passenger(std::uint32_t id, StationType o, StationType d, PassengerState s)
        : source(id), passengerId(id), station(o), destination(d), state(s) {
        if (o == d) {
            throw std::logic_error("Passenger origin == destination");
        }
    }
};

inline std::string to_string(PassengerState s) {
    switch (s) {
    case PassengerState::WAITING:
        return "WAITING";
    case PassengerState::ON_TRAIN:
        return "ON_TRAIN";
    case PassengerState::TRANSFERRING:
        return "TRANSFERRING";
    case PassengerState::COMPLETED:
        return "COMPLETED";
    default:
        return "UNKNOWN";
    }
}

// Inside or after your Passenger struct:
inline std::ostream& operator<<(std::ostream& os, const Passenger& p) {
    os << "[Passenger " << p.passengerId << "]\n"
       << "  Source Station ID: " << p.source << "\n"
       << "  Target Type:       " << static_cast<int>(p.destination) << "\n"
       << "  State:             " << to_string(p.state) << "\n";

    // Handling Optionals
    if (p.station)
        os << "  At Station:        " << *p.station << "\n";
    if (p.train)
        os << "  On Train:          " << *p.train << "\n";
    if (p.nextHop)
        os << "  Next Hop:          " << *p.nextHop << "\n";

    // Handling the Route Vector
    if (!p.committedRoute.empty()) {
        os << "  Route:             ";
        for (size_t i = 0; i < p.committedRoute.size(); ++i) {
            os << p.committedRoute[i] << (i == p.routeIndex ? "*" : "")
               << (i < p.committedRoute.size() - 1 ? " -> " : "");
        }
        os << "\n";
    }

    os << "  Age:               " << p.age << " ticks\n"
       << "---------------------------";
    return os;
}
