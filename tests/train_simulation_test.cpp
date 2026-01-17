#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"

TEST(TrainMovement, MovesForwardOneStation) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, /*capacity=*/1);

    g.tick();

    const Train& t = g.getTrains()[0];
    EXPECT_EQ(t.stationIndex, 1);
}

TEST(TrainMovement, ReversesAtLineEnd) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, 1);

    g.tick(); // A -> B
    g.tick(); // reverse, B -> A

    const Train& t = g.getTrains()[0];
    EXPECT_EQ(t.stationIndex, 0);
}

TEST(PassengerBoarding, BoardsIfRouteExists) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::Square);
    g.addTrain(line, 1);

    g.tick();

    const Train& t = g.getTrains()[0];
    EXPECT_EQ(t.onboard.size(), 1);
}

TEST(PassengerBoarding, RespectsCapacity) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::Square);
    g.spawnPassengerAt(A, StationType::Square);

    g.addTrain(line, 1);

    g.tick();

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 1);
    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 1);
}

TEST(PassengerDropoff, DropsAtDestinationType) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::Square);
    g.addTrain(line, 1);

    g.tick(); // board + move to B
    g.tick(); // drop

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 0);
}

TEST(PassengerDropoff, DoesNotDropEarly) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.spawnPassengerAt(A, StationType::Square);
    g.addTrain(line, 1);

    g.tick();

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 1);
}

