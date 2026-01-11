#pragma once
#include "Line.hpp"
#include "Station.hpp"
#include "StationType.hpp"
#include <unordered_map>

class Graph {
  public:
    std::uint32_t addStation(StationType type);
    void removeStation(std::uint32_t stationId);

    std::uint32_t addLine();
    void addStationToLine(std::uint32_t lineId, std::uint32_t stationId);
    void removeLine(std::uint32_t lineId);

    const Station* getStation(std::uint32_t id) const;
    const Line* getLine(std::uint32_t id) const;

    std::size_t stationCount() const;
    std::size_t lineCount() const;

    bool stationExists(std::uint32_t id) const;
    bool lineExists(std::uint32_t id) const;
    bool lineContainsStation(std::uint32_t lineId, std::uint32_t stationId) const;

    void spawnPassenger(std::uint32_t stationId, StationType destination);
    std::vector<std::uint32_t> adjacentStations(std::uint32_t stationId) const;
    bool canRoute(std::uint32_t fromStationId, StationType destinationType) const;
    bool canPassengerBeServed(std::uint32_t stationId, const Passenger& p) const;

  private:
    std::uint32_t nextStationId_{1};
    std::uint32_t nextLineId_{1};

    std::unordered_map<std::uint32_t, Station> stations_;
    std::unordered_map<std::uint32_t, Line> lines_;
};
