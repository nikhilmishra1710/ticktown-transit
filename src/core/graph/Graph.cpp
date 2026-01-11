#include "Graph.hpp"
#include "Line.hpp"
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

void Graph::spawnPassenger(std::uint32_t stationId, StationType destination) {
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
