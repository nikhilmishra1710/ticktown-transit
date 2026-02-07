#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"

TEST(Routing, DirectRouteOnSingleLine) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);
    auto C = g.addStation(StationType::TRIANGLE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    EXPECT_TRUE(
        g.canRoute(A, StationType::TRIANGLE)
    );
}

TEST(Routing, RouteWithTransferAtSharedStation) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto X = g.addStation(StationType::SQUARE);
    auto B = g.addStation(StationType::TRIANGLE);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, X);

    g.addStationToLine(l2, X);
    g.addStationToLine(l2, B);

    EXPECT_TRUE(
        g.canRoute(A, StationType::TRIANGLE)
    );
}

TEST(Routing, NoRouteExistsBetweenComponents) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);
    auto C = g.addStation(StationType::TRIANGLE);
    auto D = g.addStation(StationType::CIRCLE);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, A);
    g.addStationToLine(l1, B);

    g.addStationToLine(l2, C);
    g.addStationToLine(l2, D);

    EXPECT_FALSE(
        g.canRoute(A, StationType::TRIANGLE)
    );
}

TEST(Routing, RouteExistsButDestinationTypeAbsent) {
    Graph g;

    auto A = g.addStation(StationType::CIRCLE);
    auto B = g.addStation(StationType::SQUARE);
    auto C = g.addStation(StationType::SQUARE);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    EXPECT_FALSE(
        g.canRoute(A, StationType::TRIANGLE)
    );
}
