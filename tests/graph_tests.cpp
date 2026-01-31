#include <gtest/gtest.h>
#include "../src/core/graph/Graph.hpp"

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

TEST(Graph, InvalidStationRemovalThrows) {
    Graph g;
    EXPECT_THROW(g.removeStation(999), std::logic_error);
    EXPECT_THROW(g.removeLine(999), std::logic_error);
    auto s1 = g.addStation(StationType::Circle);
    auto line = g.addLine();
    g.addStationToLine(line, s1);
    EXPECT_THROW(g.addStationToLine(line, s1), std::logic_error);
}


TEST(GraphInvariant, RemovingStationCleansAllLines) {
    Graph g;

    auto s1 = g.addStation(StationType::Circle);
    auto s2 = g.addStation(StationType::Square);

    auto l1 = g.addLine();
    auto l2 = g.addLine();

    g.addStationToLine(l1, s1);
    g.addStationToLine(l1, s2);
    g.addStationToLine(l2, s1);

    g.removeStation(s1);

    const Line* line1 = g.getLine(l1);
    const Line* line2 = g.getLine(l2);

    ASSERT_NE(line1, nullptr);
    ASSERT_NE(line2, nullptr);

    EXPECT_TRUE(
        std::find(line1->stationIds.begin(), line1->stationIds.end(), s1)
        == line1->stationIds.end()
    );

    EXPECT_TRUE(
        std::find(line2->stationIds.begin(), line2->stationIds.end(), s1)
        == line2->stationIds.end()
    );
}

TEST(GraphInvariant, DoubleRemovalThrows) {
    Graph g;

    auto s1 = g.addStation(StationType::Circle);
    g.removeStation(s1);

    EXPECT_THROW(
        g.removeStation(s1),
        std::logic_error
    );
}
