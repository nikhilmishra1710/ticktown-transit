#pragma once
#include "Line.hpp"
#include "Passenger.hpp"
#include "SimulationSnapshot.hpp"
#include "Station.hpp"
#include "StationType.hpp"
#include "Train.hpp"
#include "route_info.hpp"
#include "routing_cache.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

enum BoardingPolicy { FIFO, SHORTEST_REMAINING_HOPS, AGING_PRIORITY };

class Graph {
  public:
    std::uint32_t addStation(StationType type);
    std::uint32_t addStationAtPosition(float x, float y, StationType type = StationType::Circle);
    void removeStation(std::uint32_t stationId);

    std::uint32_t addLine();
    void addStationToLine(std::uint32_t lineId, std::uint32_t stationId);
    void removeLine(std::uint32_t lineId);

    const Station* getStation(std::uint32_t id) const;
    const Line* getLine(std::uint32_t id) const;

    Station& getMutableStation(std::uint32_t id);

    std::size_t stationCount() const;
    std::size_t lineCount() const;

    std::uint32_t completedPassengers() const;

    bool stationExists(std::uint32_t id) const;
    bool lineExists(std::uint32_t id) const;
    bool lineContainsStation(std::uint32_t lineId, std::uint32_t stationId) const;

    std::size_t estimateRemainingHops(std::uint32_t fromStationId, StationType destination) const;
    bool canRoute(std::uint32_t fromStationId, StationType destinationType);
    bool canPassengerBeServed(std::uint32_t stationId, const Passenger& p);
    std::optional<std::uint32_t> nextHop(std::uint32_t fromStationId, StationType destType);
    RouteInfo computeRoute(StationId source, StationType destination) const;

    void spawnPassengerAt(std::uint32_t stationId, StationType destination);

    void addTrain(std::uint32_t line, std::uint32_t capacity);
    const std::vector<Train>& getTrains() const;

    void setBoardingPolicy(BoardingPolicy p);
    void tick();
    void stateFailed();
    bool isFailed() const;
    SimulationSnapshot snapshot() const;

  private:
    bool _canPassengerBoard(const Passenger& p, std::uint32_t stationId, const Train& train);
    bool _canBoard(const Passenger& p, std::uint32_t stationId, std::uint32_t lineId);
    std::vector<std::uint32_t> _adjacentStations(std::uint32_t stationId) const;
    void _ageWaitingPassengers();
    void _advanceTrainPosition(Train& t, Line& line);
    void _alightPassengers(Train& t, Station& station);
    void _boardPassengers(Train& t, Station& station);

    void _assertPassengerInvariants(const Passenger& p) const;
    void _assertInvariants() const;

    std::uint32_t nextStationId_{1};
    std::uint32_t nextLineId_{1};
    std::uint32_t nextTrainId_{1};
    std::uint32_t nextPassengerId_{1};
    std::uint32_t tick_{1};
    BoardingPolicy boardingPolicy_ = FIFO;
    bool failed_ = false;

    std::uint32_t completedPassengers_{0};
    RoutingCache routingCache_;
    std::unordered_map<StationId, Station> stations_;
    std::unordered_map<LineId, Line> lines_;
    std::vector<Train> trains_;
};
