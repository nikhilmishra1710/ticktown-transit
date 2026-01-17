#include "Graph.hpp"
#include "Line.hpp"
#include "Passenger.hpp"
#include "Station.hpp"
#include "StationType.hpp"
#include <algorithm>
#include <iostream>
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

std::size_t Graph::stationCount() const {
    return stations_.size();
}

std::size_t Graph::lineCount() const {
    return lines_.size();
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
    s.waitingPassengers.emplace_back(s.type, destination);
}

std::vector<std::uint32_t> Graph::adjacentStations(std::uint32_t stationId) const {
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

        for (auto neighbor : adjacentStations(current)) {
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
    std::vector<Passenger>& onboard = train.onboard;
    if (onboard.size() <= 0)
        return;
    std::cout << "Alighting passenger\nBefore: " << onboard.size() << std::endl;
    onboard.erase(std::remove_if(onboard.begin(), onboard.end(),
                                 [&](const Passenger& p) { return p.destination == station.type; }),
                  onboard.end());
    std::cout << "After: " << train.onboard.size() << std::endl;
}

void Graph::_boardPassengers(Train& train, Station& station) {
    std::vector<Passenger>& waiting = station.waitingPassengers;

    for (auto it = waiting.begin(); it < waiting.end() && train.onboard.size() < train.capacity;) {
        std::cout << "Station.type: " << station.type << " Destination: " << it->destination
                  << "canRoute: " << this->canRoute(station.id, it->destination) << std::endl;
        if (this->canRoute(station.id, it->destination)) {
            train.onboard.push_back(*it);
            std::cout << "Adding passenger to train: " << train.onboard.size() << std::endl;
            it = waiting.erase(it);
        } else {
            ++it;
        }
    }
}

void Graph::tick() {
    std::cout << "Advancing graph 1 tick" << std::endl;
    for (Train& t : this->trains_) {
        Line& line = this->lines_.at(t.lineId);
        std::uint32_t stationId = line.stationIds[t.stationIndex];
        Station& station = this->stations_.at(stationId);

        std::cout << "Before alighting passengers: " << t.onboard.size() << std::endl;
        this->_alightPassengers(t, station);
        std::cout << "After alighting passengers: " << t.onboard.size() << std::endl;
        this->_boardPassengers(t, station);
        std::cout << "After Boarding passengers: " << t.onboard.size() << std::endl;

        std::cout << "Initial station idx: " << t.stationIndex << std::endl;
        this->_advanceTrainPosition(t, line);
        std::cout << "Final station idx: " << t.stationIndex << std::endl;
    }
}