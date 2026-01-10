#include <gtest/gtest.h>
#include "core/graph/Graph.hpp"

TEST(Graph, StationLifecycle) {
    Graph g;
    auto id = g.addStation(StationType::Circle);
    EXPECT_EQ(g.stationCount(), 1u);
    g.removeStation(id);
    EXPECT_EQ(g.stationCount(), 0u);
}

TEST(Graph, StationRemovalCleansLines) {
    Graph g;
    auto s1 = g.addStation(StationType::Circle);
    auto s2 = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, s1);
    g.addStationToLine(line, s2);

    g.removeStation(s1);

    const auto* l = g.getLine(line);
    ASSERT_NE(l, nullptr);
    EXPECT_EQ(l->stationIds.size(), 1u);
    EXPECT_EQ(l->stationIds[0], s2);
}
