#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"

TEST(PassengerPressure, PassengersAccumulateIfNoTrain) {
    Graph g;
    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::SQUARE);

    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 2);
}

TEST(PassengerPressure, StationOverflowFailsSimulation) {
    Graph g;
    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.getMutableStation(A).maxCapacity = 2;

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::SQUARE);

    EXPECT_TRUE(g.isFailed());
}

TEST(PassengerPressure, TrainReducesQueue) {
    Graph g;
    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, 2);

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::SQUARE);

    g.tick(); // A Alighting
    g.tick(); // A Boarding
    g.tick(); // A -> B

    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 0);
}

TEST(PassengerPressure, FailureIsPermanent) {
    Graph g;
    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.getMutableStation(A).maxCapacity = 1;

    g.spawnPassengerAt(A, StationType::SQUARE);
    g.spawnPassengerAt(A, StationType::SQUARE);

    EXPECT_TRUE(g.isFailed());

    g.tick(); // should do nothing
    EXPECT_TRUE(g.isFailed());
}

