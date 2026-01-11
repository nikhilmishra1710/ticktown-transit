#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"

TEST(Routing, DirectRouteOnSingleLine) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    EXPECT_TRUE(
        g.canRoute(A, StationType::Triangle)
    );
}

TEST(Routing, RouteWithTransferAtSharedStation) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto X = g.addStation(StationType::Square);
    auto B = g.addStation(StationType::Triangle);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, X);

    g.addStationToLine(l2, X);
    g.addStationToLine(l2, B);

    EXPECT_TRUE(
        g.canRoute(A, StationType::Triangle)
    );
}

TEST(Routing, NoRouteExistsBetweenComponents) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);
    auto D = g.addStation(StationType::Circle);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, B);

    g.addStationToLine(l2, C);
    g.addStationToLine(l2, D);

    EXPECT_FALSE(
        g.canRoute(A, StationType::Triangle)
    );
}

TEST(Routing, RouteExistsButDestinationTypeAbsent) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    EXPECT_FALSE(
        g.canRoute(A, StationType::Triangle)
    );
}
