#include "core/graph/Graph.hpp"
#include <gtest/gtest.h>

TEST(Transfer, PassengerTransfersLines) {
    Graph g;

    auto A = g.addStation(Circle);
    auto X = g.addStation(Square);
    auto B = g.addStation(Triangle);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, X);

    g.addStationToLine(l2, X);
    g.addStationToLine(l2, B);

    g.addTrain(l1, 1);
    g.addTrain(l2, 1);

    g.spawnPassengerAt(A, Triangle);

    g.tick(); // A -> X (T1: at A T2: at X)
    g.tick(); // transfer (T1: at X T2: at B)
    g.tick(); // Board at X (T1: at A T2: at X)
    g.tick(); // X -> B (T1: at X T2: at B)

    EXPECT_EQ(g.completedPassengers(), 1);
}

TEST(Transfer, PassengerRejectsWrongLine) {
    Graph g;

    auto X = g.addStation(Circle);
    auto Y = g.addStation(Square);
    auto Z = g.addStation(Triangle);

    auto l1 = g.addLine();
    g.addStationToLine(l1, X);
    g.addStationToLine(l1, Y);

    g.spawnPassengerAt(X, Triangle);
    g.addTrain(l1, 1);

    g.tick();

    EXPECT_EQ(g.getTrains()[0].onboard.size(), 0);
}
