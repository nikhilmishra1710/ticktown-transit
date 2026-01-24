#include "Graph.hpp"
#include "Line.hpp"
#include "Passenger.hpp"
#include "Station.hpp"
#include "StationType.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>

std::uint32_t Graph::addStation(StationType type) {
    Station newStation = {this->nextStationId_, type};
    this->stations_[this->nextStationId_] = newStation;
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
}

std::uint32_t Graph::addLine() {
    Line newLine = {this->nextLineId_, {}};
    this->lines_[this->nextLineId_] = newLine;
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
}
void Graph::removeLine(std::uint32_t lineId) {
    if (!this->lineExists(lineId)) {
        throw std::logic_error("Line doesn't exist");
    }
    this->lines_.erase(lineId);
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
    s.waitingPassengers.emplace_back(this->nextPassengerId_++, s.type, destination, WAITING);
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

bool Graph::canRoute(std::uint32_t fromStationId, StationType destinationType) const {
    auto startIt = stations_.find(fromStationId);
    if (startIt == stations_.end()) {
        throw std::logic_error("Invalid fromStationId in canRoute");
    }

    std::queue<std::uint32_t> q;
    std::unordered_set<std::uint32_t> visited;

    q.push(fromStationId);
    visited.insert(fromStationId);

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        const Station& s = stations_.at(current);
        if (s.type == destinationType) {
            return true;
        }

        for (auto neighbor : this->_adjacentStations(current)) {
            if (!visited.contains(neighbor)) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    return false;
}

bool Graph::canPassengerBeServed(std::uint32_t stationId, const Passenger& p) const {
    return canRoute(stationId, p.destination);
}

std::optional<std::uint32_t> Graph::nextHop(std::uint32_t fromStationId,
                                            StationType destType) const {
    if (stations_.at(fromStationId).type == destType) {
        return std::nullopt; // already there; no movement
    }

    std::queue<std::uint32_t> q;
    std::unordered_map<std::uint32_t, std::uint32_t> parent;
    std::unordered_set<std::uint32_t> visited;

    q.push(fromStationId);
    visited.insert(fromStationId);

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
                        if (stations_.at(n).type == destType) {
                            // reconstruct first hop
                            while (parent[n] != fromStationId)
                                n = parent[n];
                            return n;
                        }
                        q.push(n);
                    }
                }

                if (i + 1 < line.stationIds.size()) {
                    auto n = line.stationIds[i + 1];
                    if (!visited.count(n)) {
                        visited.insert(n);
                        parent[n] = cur;
                        if (stations_.at(n).type == destType) {
                            while (parent[n] != fromStationId)
                                n = parent[n];
                            return n;
                        }
                        q.push(n);
                    }
                }
            }
        }
    }
    return std::nullopt;
}

bool Graph::_canPassengerBoard(const Passenger& p, std::uint32_t stationId,
                               const Train& train) const {
    if (p.state != PassengerState::WAITING)
        return false;
    if (train.onboard.size() >= train.capacity)
        return false;
    bool board = this->_canBoard(p, stationId, train.lineId);
    return board;
}

bool Graph::_canBoard(const Passenger& p, std::uint32_t stationId, std::uint32_t lineId) const {
    auto hop = nextHop(stationId, p.destination);
    if (!hop)
        return false;

    const Line& line = lines_.at(lineId);
    return std::find(line.stationIds.begin(), line.stationIds.end(), *hop) != line.stationIds.end();
}

void Graph::addTrain(std::uint32_t lineId, std::uint32_t capacity) {
    if (!this->lineExists(lineId)) {
        throw std::logic_error("Line doesn't exist in addTrain");
    }
    if (capacity <= 0) {
        throw std::logic_error("Capacity below 0");
    }
    const Line* line = this->getLine(lineId);
    if (line->stationIds.size() <= 0) {
        throw std::logic_error("Cannot add train to line with no stations");
    }
    Train t = {lineId, 0, 1, {}, capacity};
    this->trains_.push_back(t);
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

    if (t.direction == 1 && t.stationIndex == last) {
        t.direction = -1;
    } else if (t.direction == -1 && t.stationIndex == 0) {
        t.direction = 1;
    }

    t.stationIndex += t.direction;
}

void Graph::_alightPassengers(Train& train, Station& station) {
    for (auto passenger = train.onboard.begin(); passenger != train.onboard.end();) {
        bool done =
            station.type == passenger->destination || station.id == passenger->targetStationId;

        if (!done) {
            ++passenger;
            continue;
        }

        // clear travel commitment
        passenger->currentLineId.reset();
        passenger->targetStationId.reset();
        passenger->state = PassengerState::WAITING;

        if (station.type != passenger->destination) {
            station.waitingPassengers.push_back(*passenger); // transfer
        } else {
            this->completedPassengers_++;
        }

        passenger = train.onboard.erase(passenger);
    }
}

void Graph::_boardPassengers(Train& train, Station& station) {
    std::vector<Passenger>& waiting = station.waitingPassengers;

    auto score = [&](const Passenger& p) {
        switch (boardingPolicy_) {
        case BoardingPolicy::FIFO:
            return p.waitingTicks;

        case BoardingPolicy::AGING_PRIORITY:
            return p.waitingTicks;

        case BoardingPolicy::SHORTEST_REMAINING_HOPS:
            return SIZE_MAX - estimateRemainingHops(station.id, p.destination);
        }
        return std::size_t{0};
    };

    std::stable_sort(waiting.begin(), waiting.end(),
                     [&](const Passenger& a, const Passenger& b) { return score(a) > score(b); });
    for (auto it = waiting.begin(); it != waiting.end() && train.onboard.size() < train.capacity;) {

        if (!this->_canPassengerBoard(*it, station.id, train)) {
            ++it;
            continue;
        }
        std::optional<std::uint32_t> hop = this->nextHop(station.id, it->destination);

        Passenger p = *it;
        p.targetStationId = *hop;
        p.currentLineId = train.lineId;
        p.state = ONTRAIN;
        if (p.lastStationId == *hop) {
            std::logic_error("Passenger is oscilating!");
        }
        p.lastStationId = station.id;
        train.onboard.push_back(p);
        it = waiting.erase(it);
    }
}

void Graph::_assertInvariants() const {
    std::unordered_set<const Passenger*> seen;

    for (const auto& [_, st] : stations_) {
        for (const auto& p : st.waitingPassengers) {
            assert(!p.currentLineId.has_value());
            assert(seen.insert(&p).second);
        }
    }

    for (const auto& t : trains_) {
        for (const auto& p : t.onboard) {
            assert(p.currentLineId.has_value());
            assert(p.currentLineId == t.lineId);
            assert(seen.insert(&p).second);
        }
    }
}

void Graph::_ageWaitingPassengers() {
    for (auto& [_, station] : stations_) {
        for (auto& p : station.waitingPassengers) {
            ++p.waitingTicks;
        }
    }
}

void Graph::setBoardingPolicy(BoardingPolicy p) {
    this->boardingPolicy_ = p;
}

void Graph::tick() {
    if (this->failed_)
        return;
    for (Train& t : this->trains_) {
        Line& line = this->lines_.at(t.lineId);
        std::uint32_t stationId = line.stationIds[t.stationIndex];
        Station& station = this->stations_.at(stationId);

        this->_alightPassengers(t, station);
        this->_boardPassengers(t, station);

        this->_advanceTrainPosition(t, line);

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