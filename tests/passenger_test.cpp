#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"

TEST(PassengerPressure, PassengersAccumulateIfNoTrain) {
    Graph g;
    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::Square);
    g.spawnPassengerAt(A, StationType::Square);

    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 2);
}

TEST(PassengerPressure, StationOverflowFailsSimulation) {
    Graph g;
    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.getMutableStation(A).maxCapacity = 2;

    g.spawnPassengerAt(A, StationType::Square);
    g.spawnPassengerAt(A, StationType::Square);
    g.spawnPassengerAt(A, StationType::Square);

    EXPECT_TRUE(g.isFailed());
}

TEST(PassengerPressure, TrainReducesQueue) {
    Graph g;
    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, 2);

    g.spawnPassengerAt(A, StationType::Square);
    g.spawnPassengerAt(A, StationType::Square);

    g.tick();

    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 0);
}

TEST(PassengerPressure, FailureIsPermanent) {
    Graph g;
    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.getMutableStation(A).maxCapacity = 1;

    g.spawnPassengerAt(A, StationType::Square);
    g.spawnPassengerAt(A, StationType::Square);

    EXPECT_TRUE(g.isFailed());

    g.tick(); // should do nothing
    EXPECT_TRUE(g.isFailed());
}

