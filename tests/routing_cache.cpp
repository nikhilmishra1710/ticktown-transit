#include "core/graph/Graph.hpp"
#include <gtest/gtest.h>

TEST(RoutingInvalidation, NextHopRecomputedAfterStationInsertion) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    auto hop1 = g.nextHop(A, StationType::Triangle);
    ASSERT_EQ(hop1, B);

    auto D = g.addStation(StationType::Star);
    g.addStationToLine(line, D); // mutation

    auto hop2 = g.nextHop(A, StationType::Triangle);
    ASSERT_TRUE(hop2.has_value());
}

TEST(Serviceability, PassengerDoesNotBoardIfNextHopNotServedByTrain) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, B);

    g.addStationToLine(l2, A);
    g.addStationToLine(l2, C);

    g.addTrain(l1, 1); // only serves A->B

    g.spawnPassengerAt(A, StationType::Triangle);

    g.tick();

    EXPECT_EQ(g.getStation(A)->waitingPassengers.size(), 1);
    EXPECT_EQ(g.getTrains()[0].onboard.size(), 0);
}

TEST(TopologyMutation, TrainDoesNotServeInsertedStationMidRoute) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    g.addTrain(line, 1);

    g.tick(); // train at B

    auto D = g.addStation(StationType::Star);
    g.addStationToLine(line, D);

    g.tick(); // train must go to C, not D

    EXPECT_EQ(g.getTrains()[0].currentStationId, C);
}

TEST(Monotonicity, PassengerNeverRevisitsStation) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, B);
    g.addStationToLine(l2, B);
    g.addStationToLine(l2, C);

    g.addTrain(l1, 1);
    g.addTrain(l2, 1);

    g.spawnPassengerAt(A, StationType::Triangle);

    for (int i = 0; i < 10; ++i)
        g.tick();

    EXPECT_EQ(g.completedPassengers(), 1);
}
