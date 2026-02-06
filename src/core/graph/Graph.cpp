#include "Graph.hpp"
#include "Line.hpp"
#include "Passenger.hpp"
#include "SimulationSnapshot.hpp"
#include "Station.hpp"
#include "StationType.hpp"
#include "Train.hpp"
#include "id.hpp"
#include "passenger_state_machine.hpp"
#include "route_info.hpp"
#include "train_state_machine.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

std::uint32_t Graph::addStation(StationType type) {
    Station newStation = {this->nextStationId_, type, {}};
    this->stations_[this->nextStationId_] = newStation;
    routingCache_.invalidate();
    return this->nextStationId_++;
}

StationId Graph::addStationAtPosition(float x, float y, StationType type) {
    Station newStation = {this->nextStationId_, type, {}, x, y};
    this->stations_[this->nextStationId_] = newStation;
    routingCache_.invalidate();
    return this->nextStationId_++;
}

void Graph::removeStation(std::uint32_t stationId) {
    auto stationFind = this->stations_.find(stationId);
    if (!this->stationExists(stationId)) {
        throw std::logic_error("StationId doesn't exists in removeStation");
    }
    this->stations_.erase(stationId);
    for (auto& [_, line] : this->lines_) {
        line.stationIds.erase(
            std::remove(line.stationIds.begin(), line.stationIds.end(), stationId),
            line.stationIds.end());
    }
    routingCache_.invalidate();
}

std::uint32_t Graph::addLine() {
    Line newLine = {this->nextLineId_, {}};
    this->lines_[this->nextLineId_] = newLine;
    routingCache_.invalidate();
    return this->nextLineId_++;
}

void Graph::addStationToLine(std::uint32_t lineId, std::uint32_t stationId) {
    if (!this->lineExists(lineId) || !this->stationExists(stationId)) {
        throw std::logic_error("StationId or LineId doesn't exists in addStationToLine");
    }
    if (lineContainsStation(lineId, stationId)) {
        throw std::logic_error("Station already exists on line");
    }
    this->lines_[lineId].stationIds.push_back(stationId);
    routingCache_.invalidate();
}

void Graph::addStationToLineAtIndex(std::uint32_t lineId, std::uint32_t stationId,
                                    std::size_t index) {
    if (!this->lineExists(lineId) || !this->stationExists(stationId)) {
        throw std::logic_error("StationId or LineId doesn't exists in addStationToLine");
    }
    if (lineContainsStation(lineId, stationId)) {
        throw std::logic_error("Station already exists on line");
    }
    if (index == SIZE_MAX) {
        index = this->lines_[lineId].stationIds.size();
    }
    this->lines_[lineId].stationIds.insert(this->lines_[lineId].stationIds.begin() + index,
                                           stationId);
    routingCache_.invalidate();
}

void Graph::removeLine(std::uint32_t lineId) {
    if (!this->lineExists(lineId)) {
        throw std::logic_error("Line doesn't exist");
    }
    this->lines_.erase(lineId);
    routingCache_.invalidate();
}

const Station* Graph::getStation(std::uint32_t id) const {
    auto it = this->stations_.find(id); // assuming 'lines' is your map
    if (it != this->stations_.end()) {
        return &(it->second);
    }
    return nullptr;
}

const Line* Graph::getLine(std::uint32_t id) const {
    auto it = this->lines_.find(id); // assuming 'lines' is your map
    if (it != this->lines_.end()) {
        return &(it->second);
    }
    return nullptr;
}

Station& Graph::getMutableStation(std::uint32_t id) {
    auto it = this->stations_.find(id); // assuming 'lines' is your map
    if (it != this->stations_.end()) {
        return it->second;
    }
    throw std::logic_error("Invalid station id in getMutableStation");
}

std::size_t Graph::stationCount() const {
    return stations_.size();
}

std::size_t Graph::lineCount() const {
    return lines_.size();
}

std::uint32_t Graph::completedPassengers() const {
    return this->completedPassengers_;
}

bool Graph::stationExists(std::uint32_t id) const {
    return this->stations_.find(id) != this->stations_.end();
}

bool Graph::lineExists(std::uint32_t id) const {
    return this->lines_.find(id) != this->lines_.end();
}

bool Graph::lineContainsStation(std::uint32_t lineId, std::uint32_t stationId) const {
    if (!this->lineExists(lineId) || !this->stationExists(stationId)) {
        throw std::logic_error("Line or station does not exist");
    }
    const Line* line = this->getLine(lineId);
    return std::find(line->stationIds.begin(), line->stationIds.end(), stationId) !=
           line->stationIds.end();
}

void Graph::spawnPassengerAt(std::uint32_t stationId, StationType destination) {
    auto it = stations_.find(stationId);
    if (it == stations_.end()) {
        throw std::logic_error("Invalid stationId in spawnPassenger");
    }

    Station& s = it->second;
    if (s.waitingPassengers.size() >= s.maxCapacity) {
        this->stateFailed();
        return;
    }
    s.waitingPassengers.emplace_back(this->nextPassengerId_++, s.type, destination,
                                     PassengerState::WAITING);
}

std::vector<std::uint32_t> Graph::_adjacentStations(std::uint32_t stationId) const {
    std::vector<std::uint32_t> result;

    for (const auto& [_, line] : lines_) {
        const auto& v = line.stationIds;
        for (size_t i = 0; i < v.size(); ++i) {
            if (v[i] == stationId) {
                if (i > 0)
                    result.push_back(v[i - 1]);
                if (i + 1 < v.size())
                    result.push_back(v[i + 1]);
            }
        }
    }

    return result;
}

bool Graph::canRoute(std::uint32_t fromStationId, StationType destinationType) {
    const RouteInfo& r = routingCache_.get(fromStationId, destinationType, *this);
    return r.reachable;
}

bool Graph::canPassengerBeServed(std::uint32_t stationId, const Passenger& p) {
    return canRoute(stationId, p.destination);
}

std::optional<std::uint32_t> Graph::nextHop(std::uint32_t fromStationId, StationType destType) {
    const RouteInfo& r = routingCache_.get(fromStationId, destType, *this);
    std::cout << "Source: " << fromStationId << " Dest Type: " << destType
              << " Reachable: " << r.reachable << "\nPath: ";
    for (auto it : r.path) {
        std::cout << it << " ";
    }
    std::cout << std::endl;
    if (!r.reachable || r.path.size() < 2) {
        return std::nullopt;
    }

    return r.path[1]; // path[0] == from
}

RouteInfo Graph::computeRoute(StationId source, StationType destinationType) const {
    std::cout << "Started Compute Route source: " << source << " Dest type: " << destinationType
              << std::endl;
    RouteInfo routeInfo = {false, {}};
    if (stations_.at(source).type == destinationType) {
        return routeInfo; // already there; no movement
    }

    std::queue<StationId> q;
    std::unordered_map<StationId, StationId> parent;
    std::unordered_set<StationId> visited;

    std::vector<StationId> path;

    q.push(source);
    visited.insert(source);

    while (!q.empty()) {
        std::uint32_t cur = q.front();
        q.pop();

        // neighbors induced by lines
        for (const auto& [_, line] : lines_) {
            for (size_t i = 0; i < line.stationIds.size(); ++i) {
                if (line.stationIds[i] != cur)
                    continue;

                if (i > 0) {
                    auto n = line.stationIds[i - 1];
                    if (!visited.count(n)) {
                        visited.insert(n);
                        parent[n] = cur;
                        if (stations_.at(n).type == destinationType) {
                            while (parent[n] != source) {
                                routeInfo.path.push_back(n);
                                n = parent[n];
                            }
                            routeInfo.path.push_back(n);
                            routeInfo.path.push_back(source);
                            std::reverse(routeInfo.path.begin(), routeInfo.path.end());
                            routeInfo.reachable = true;
                            std::cout << "Length: " << routeInfo.path.size() << std::endl;
                            return routeInfo;
                        }
                        q.push(n);
                    }
                }

                if (i + 1 < line.stationIds.size()) {
                    auto n = line.stationIds[i + 1];
                    if (!visited.count(n)) {
                        visited.insert(n);
                        parent[n] = cur;
                        if (stations_.at(n).type == destinationType) {
                            while (parent[n] != source) {
                                routeInfo.path.push_back(n);
                                n = parent[n];
                            }
                            routeInfo.path.push_back(n);
                            routeInfo.path.push_back(source);
                            std::reverse(routeInfo.path.begin(), routeInfo.path.end());
                            routeInfo.reachable = true;
                            std::cout << "Length: " << routeInfo.path.size() << std::endl;
                            return routeInfo;
                        }
                        q.push(n);
                    }
                }
            }
        }
    }
    return routeInfo;
}

bool Graph::_canPassengerBoard(const Passenger& p, std::uint32_t stationId, const Train& train) {
    if (p.state != PassengerState::WAITING && p.state != PassengerState::TRANSFERRING)
        return false;
    if (train.onboard.size() >= train.capacity)
        return false;
    bool board = this->_canBoard(p, stationId, train.lineId);
    return board;
}

bool Graph::_canBoard(const Passenger& p, std::uint32_t stationId, std::uint32_t lineId) {
    auto hop = nextHop(stationId, p.destination);
    if (!hop)
        return false;

    const Line& line = lines_.at(lineId);
    return std::find(line.stationIds.begin(), line.stationIds.end(), *hop) != line.stationIds.end();
}

void Graph::addTrain(std::uint32_t lineId, std::uint32_t capacity, float speed,
                     TrainState initialState) {
    if (!this->lineExists(lineId)) {
        throw std::logic_error("Line doesn't exist in addTrain");
    }
    if (capacity <= 0) {
        throw std::logic_error("Capacity below 0");
    }
    const Line* line = this->getLine(lineId);
    if (line->stationIds.size() < 2) {
        throw std::logic_error("Cannot add train to line with no stations");
    }
    Train t = {this->nextTrainId_++,
               lineId,
               line->stationIds[0],
               line->stationIds[1],
               initialState,
               0,
               1,
               {},
               capacity,
               0.0f,
               speed};
    this->trains_.push_back(t);
}

void Graph::startTrain(std::uint32_t trainId) {
    auto it = std::find_if(trains_.begin(), trains_.end(),
                           [trainId](const Train& t) { return t.trainId == trainId; });
    if (it == trains_.end()) {
        throw std::logic_error("Train doesn't exist in startTrain");
    }
    Train& t = *it;
    TrainFSM::idleToAlighting(t);
}

const std::vector<Train>& Graph::getTrains() const {
    return this->trains_;
}

std::size_t Graph::estimateRemainingHops(std::uint32_t fromStationId,
                                         StationType destination) const {
    std::queue<std::uint32_t> q;
    std::unordered_map<std::uint32_t, std::size_t> dist;

    q.push(fromStationId);
    dist[fromStationId] = 0;

    while (!q.empty()) {
        auto s = q.front();
        q.pop();

        if (stations_.at(s).type == destination) {
            return dist[s];
        }

        for (auto n : this->_adjacentStations(s)) {
            if (!dist.contains(n)) {
                dist[n] = dist[s] + 1;
                q.push(n);
            }
        }
    }
    return SIZE_MAX; // unreachable
}

void Graph::_advanceTrainPosition(Train& t, Line& line) {
    const std::size_t last = line.stationIds.size() - 1;

    if (last == 0)
        return;

    t.progress += t.speed;
    if (t.progress < 1.0f) {
        return;
    }
    t.stationIndex += t.direction;
    t.currentStationId = t.nextStationId;

    if (t.direction == 1 && t.stationIndex == last) {
        t.direction = -1;
    } else if (t.direction == -1 && t.stationIndex == 0) {
        t.direction = 1;
    }

    t.nextStationId = line.stationIds[t.stationIndex + t.direction];
    std::cout << "Train " << t.trainId << " arrived at station " << t.currentStationId
              << " next station " << t.nextStationId << " progress: " << t.progress
              << " Current state: " << (int) t.state << std::endl;
    TrainFSM::movingToAlighting(t);
}

void Graph::_alightPassengers(Train& train, Station& station) {
    for (auto it = train.onboard.begin(); it != train.onboard.end();) {
        Passenger& passenger = *it;

        if (passenger.nextHop != station.id) {
            throw std::logic_error("Passenger route desync during alight");
        }

        if (station.type == passenger.destination) {
            it = train.onboard.erase(it);
            PassengerFSM::onTrainToCompleted(passenger);
            this->completedPassengers_++;
            continue;
        }

        std::optional hop = this->nextHop(station.id, passenger.destination);
        if (!hop.has_value()) {
            throw std::logic_error("Passenger has no where to go");
        }
        StationId nextStation = *hop;
        passenger.nextHop = nextStation;

        // Case 2: train continues along route → STAY ON TRAIN
        if (this->lineContainsStation(train.lineId, nextStation)) {
            ++it;
            continue;
        }

        // Case 3: transfer required → ALIGHT
        PassengerFSM::onTrainToTransferring(passenger, station.id);
        station.waitingPassengers.push_back(passenger);
        it = train.onboard.erase(it);
    }

    TrainFSM::alightingToBoarding(train);
}

void Graph::_boardPassengers(Train& train, Station& station) {
    std::vector<Passenger>& waiting = station.waitingPassengers;

    auto score = [&](const Passenger& p) {
        switch (boardingPolicy_) {
        case BoardingPolicy::FIFO:
            return p.age;

        case BoardingPolicy::AGING_PRIORITY:
            return p.age;

        case BoardingPolicy::SHORTEST_REMAINING_HOPS:
            return SIZE_MAX - estimateRemainingHops(station.id, p.destination);
        }
        return std::size_t{0};
    };

    std::stable_sort(waiting.begin(), waiting.end(),
                     [&](const Passenger& a, const Passenger& b) { return score(a) > score(b); });
    std::cout << "Train id: " << train.trainId << std::endl;
    for (auto it = waiting.begin(); it != waiting.end() && train.onboard.size() < train.capacity;) {

        Passenger& passenger = *it;
        bool canBoard = this->_canPassengerBoard(passenger, station.id, train);
        std::cout << "Passenger id: " << passenger.passengerId << " can board: " << canBoard
                  << " source: " << station.id << " dest type: " << passenger.destination
                  << std::endl;
        if (!canBoard) {
            ++it;
            continue;
        }

        if (!passenger.nextHop.has_value()) {
            passenger.nextHop = this->nextHop(station.id, passenger.destination);
        }

        // Must have a next step
        if (!passenger.nextHop.has_value()) {
            ++it;
            continue;
        }

        if (passenger.state == PassengerState::WAITING)
            PassengerFSM::waitingToOnTrain(passenger, train.trainId);
        else
            PassengerFSM::transferringToOnTrain(passenger, train.trainId);
        train.onboard.push_back(passenger);
        it = waiting.erase(it);
    }

    TrainFSM::boardingToMoving(train);
}

void Graph::_assertPassengerInvariants(const Passenger& p) const {

    switch (p.state) {
    case PassengerState::WAITING:
        assert(p.station.has_value());
        assert(!p.train.has_value());
        break;
    case PassengerState::ON_TRAIN:
        assert(p.train.has_value());
        assert(!p.station.has_value());
        break;
    case PassengerState::TRANSFERRING:
        assert(p.station.has_value());
        break;
    case PassengerState::COMPLETED:
        assert(!p.train.has_value());
        assert(!p.station.has_value());
        break;
    }
}

void Graph::_assertInvariants() const {
    std::unordered_set<const Passenger*> seen;

    for (const auto& [_, st] : stations_) {
        for (const auto& p : st.waitingPassengers) {
            this->_assertPassengerInvariants(p);
        }
    }

    for (const auto& t : trains_) {
        for (const auto& p : t.onboard) {
            this->_assertPassengerInvariants(p);
            assert(seen.insert(&p).second);
        }
    }
}

void Graph::_ageWaitingPassengers() {
    for (auto& [_, station] : stations_) {
        for (auto& p : station.waitingPassengers) {
            ++p.age;
        }
    }
}

void Graph::setBoardingPolicy(BoardingPolicy p) {
    this->boardingPolicy_ = p;
}

void Graph::tick() {
    if (this->failed_)
        return;
    this->tick_++;
    for (Train& t : this->trains_) {
        Line& line = this->lines_.at(t.lineId);
        std::uint32_t stationId = line.stationIds[t.stationIndex];
        Station& station = this->stations_.at(stationId);

        switch (t.state) {
        case TrainState::ALIGHTING:
            this->_alightPassengers(t, station);
            break;
        case TrainState::BOARDING:
            this->_boardPassengers(t, station);
            break;
        case TrainState::MOVING:
            this->_advanceTrainPosition(t, line);
            break;
        case TrainState::IDLE:
            break;
        default:
            throw std::logic_error("Invalid Train State " + std::to_string(t.trainId));
        }

        this->_assertInvariants();
    }
    this->_ageWaitingPassengers();
}

void Graph::stateFailed() {
    this->failed_ = true;
}

bool Graph::isFailed() const {
    return this->failed_;
}

SimulationSnapshot Graph::snapshot() const {
    SimulationSnapshot snap;
    snap.tick = this->tick_;
    snap.score = this->completedPassengers_;

    for (const auto& [id, s] : stations_) {
        StationView stationView = {id, s.type, s.waitingPassengers.size(), {}};
        for (const auto& p : s.waitingPassengers) {
            stationView.passengers.push_back(
                {p.passengerId, p.source, p.destination, p.state, id, std::nullopt, p.age});
            snap.passengers.push_back(
                {p.passengerId, p.source, p.destination, p.state, id, std::nullopt, p.age});
        }
        snap.stations.push_back(stationView);
        snap.stationPositions.emplace(id, std::make_pair(std::make_pair(s.x, s.y), stationView));
    }

    for (const auto& t : trains_) {
        TrainView trainView = {t.trainId,
                               t.lineId,
                               t.currentStationId,
                               t.nextStationId,
                               t.direction == 1 ? true : false,
                               t.capacity,
                               t.onboard.size(),
                               t.state,
                               t.progress,
                               {}};
        for (const auto& p : t.onboard) {
            trainView.passengers.push_back(
                {p.passengerId, p.source, p.destination, p.state, std::nullopt, t.trainId, p.age});
            snap.passengers.push_back(
                {p.passengerId, p.source, p.destination, p.state, std::nullopt, t.trainId, p.age});
        }
        snap.trains.push_back(trainView);
    }

    for (const auto& [id, line] : lines_) {
        LineView lineView = {id, line.stationIds};
        snap.lines.push_back(lineView);
    }

    return snap;
}
