#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"
#include "core/graph/StationType.hpp"

TEST(TrainMovement, MovesForwardOneStation) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, /*capacity=*/1);

    g.tick(); // Alighting
    g.tick(); // Boarding
    g.tick(); // Moves forrward

    const Train& t = g.getTrains()[0];
    EXPECT_EQ(t.stationIndex, 1);
}

TEST(TrainMovement, ReversesAtLineEnd) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, 1);

    g.tick(); // A Alighting
    g.tick(); // A Boarding
    g.tick(); // A -> B

    const Train& t1 = g.getTrains()[0];
    EXPECT_EQ(t1.stationIndex, 1);

    g.tick(); // B Alighting
    g.tick(); // B Boarding
    g.tick(); // B -> A

    const Train& t2 = g.getTrains()[0];
    EXPECT_EQ(t2.stationIndex, 0);
}

TEST(PassengerBoarding, BoardsIfRouteExists) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::STAR);
    g.addTrain(line, 1);

    g.tick(); // A Alighting
    g.tick(); // A Boarding

    const Train& t = g.getTrains()[0];
    EXPECT_EQ(t.onboard.size(), 1);
    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 1);
}

TEST(PassengerBoarding, RespectsCapacity) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::SQUARE);

    g.addTrain(line, 1);

    g.tick(); // A Alighting
    g.tick(); // A Boarding

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 1);
    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 1);
}

TEST(PassengerDropoff, DropsAtDestinationType) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.addTrain(line, 1);

    g.tick(); // A Alighting
    g.tick(); // A Boarding
    g.tick(); // A -> B
    g.tick(); // B Alighting

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 0);
}

TEST(PassengerDropoff, DoesNotDropEarly) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.addTrain(line, 1);

    g.tick(); // A Alighting
    g.tick(); // A Boarding
    g.tick(); // A -> B

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 1);
}

