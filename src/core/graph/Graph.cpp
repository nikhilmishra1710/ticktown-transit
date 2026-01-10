#include "Graph.hpp"
#include "Line.hpp"
#include <algorithm>
#include <vector>

std::uint32_t Graph::addStation(StationType type) {
    Station newStation = {this->nextStationId_, type};
    this->stations_[this->nextStationId_] = newStation;
    return this->nextStationId_++;
}

void Graph::removeStation(std::uint32_t stationId) {
    auto stationFind = this->stations_.find(stationId);
    if (stationFind != this->stations_.end()) {
        this->stations_.erase(stationId);
        for (auto& [_, line] : this->lines_) {
            line.stationIds.erase(
                std::remove(line.stationIds.begin(), line.stationIds.end(), stationId),
                line.stationIds.end());
        }
    }
}

std::uint32_t Graph::addLine() {
    Line newLine = {this->nextLineId_, {}};
    this->lines_[this->nextLineId_] = newLine;
    return this->nextLineId_++;
}

void Graph::addStationToLine(std::uint32_t lineId, std::uint32_t stationId) {
    auto lineFind = this->lines_.find(lineId);
    auto stationFind = this->stations_.find(stationId);
    if (lineFind != this->lines_.end() && stationFind != this->stations_.end()) {
        this->lines_[lineId].stationIds.push_back(stationId);
    }
}
void Graph::removeLine(std::uint32_t lineId) {
    auto it = this->lines_.find(lineId);
    if (it != this->lines_.end()) {
        this->lines_.erase(lineId);
    }
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