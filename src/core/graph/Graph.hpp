#pragma once
#include "Line.hpp"
#include "Station.hpp"
#include "StationType.hpp"
#include "Train.hpp"
#include <unordered_map>
#include <vector>

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

    std::vector<std::uint32_t> adjacentStations(std::uint32_t stationId) const;
    bool canRoute(std::uint32_t fromStationId, StationType destinationType) const;
    bool canPassengerBeServed(std::uint32_t stationId, const Passenger& p) const;
    
    void spawnPassengerAt(std::uint32_t stationId, StationType destination);
    
    void addTrain(std::uint32_t line, std::uint32_t capacity);
    const std::vector<Train>& getTrains() const;

    void tick();

  private:
    void _advanceTrainPosition(Train& t, Line& line);
    void _alightPassengers(Train& t, Station& station);
    void _boardPassengers(Train& t, Station& station);

    std::uint32_t nextStationId_{1};
    std::uint32_t nextLineId_{1};

    std::unordered_map<std::uint32_t, Station> stations_;
    std::unordered_map<std::uint32_t, Line> lines_;
    std::vector<Train> trains_;
};
