#include "core/graph/Graph.hpp"
#include "core/graph/Passenger.hpp"
#include "core/graph/StationType.hpp"
#include "core/graph/passenger_state_machine.hpp"
#include <gtest/gtest.h>

TEST(PassengerAging, WaitingPassengersAgeEachTick) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    g.spawnPassengerAt(A, StationType::Square);

    g.tick();
    g.tick();

    const auto& waiting = g.getStation(A)->waitingPassengers;
    ASSERT_EQ(waiting.size(), 1);
    EXPECT_EQ(waiting[0].age, 2);
}

TEST(PassengerAging, OnboardPassengersDoNotAge) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);

    g.addTrain(line, 1);
    g.spawnPassengerAt(A, StationType::Square);

    g.tick(); // boards passenger

    const auto& t = g.getTrains()[0];
    ASSERT_EQ(t.onboard.size(), 1);
    EXPECT_EQ(t.onboard[0].age, 0);
}

TEST(BoardingPolicy, FIFOPreservesArrivalOrder) {
    Graph g;
    g.setBoardingPolicy(BoardingPolicy::FIFO);

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addTrain(line, 1);

    g.spawnPassengerAt(B, StationType::Circle);
    g.tick(); // age first passenger
    g.spawnPassengerAt(B, StationType::Circle);

    g.tick(); // boarding

    const auto& onboard = g.getTrains()[0].onboard;
    ASSERT_EQ(onboard.size(), 1);
    EXPECT_GT(onboard[0].age, 0);
}

TEST(BoardingPolicy, ShortestRemainingHopWins) {
    Graph g;
    g.setBoardingPolicy(BoardingPolicy::SHORTEST_REMAINING_HOPS);

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

    g.spawnPassengerAt(A, StationType::Triangle); // 2 hops
    g.spawnPassengerAt(A, StationType::Square);   // 1 hop

    g.tick();

    const auto& onboard = g.getTrains()[0].onboard;
    ASSERT_EQ(onboard.size(), 1);
    EXPECT_EQ(onboard[0].destination, StationType::Square);
}

TEST(Fairness, AgingEventuallyOverridesDistance) {
    Graph g;
    g.setBoardingPolicy(BoardingPolicy::AGING_PRIORITY);

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);
    auto C = g.addStation(StationType::Triangle);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addStationToLine(line, C);

    g.spawnPassengerAt(A, StationType::Triangle); // farther
    for (int i = 0; i < 5; ++i)
        g.tick();

    g.spawnPassengerAt(A, StationType::Square); // closer

    g.addTrain(line, 1);
    g.tick();

    const auto& onboard = g.getTrains()[0].onboard;
    ASSERT_EQ(onboard.size(), 1);
    EXPECT_EQ(onboard[0].destination, StationType::Triangle);
}

TEST(Invariant, PassengerOwnershipIsExclusive) {
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
    EXPECT_EQ(g.getTrains()[0].onboard.size(), 2);
}

TEST(TransferSafety, PassengerTransfersOnce) {
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

    for (int i = 0; i < 5; ++i)
        g.tick();

    EXPECT_EQ(g.completedPassengers(), 1);
}

TEST(TransferSafety, NoStationPingPong) {
    Graph g;

    auto A = g.addStation(StationType::Circle);
    auto B = g.addStation(StationType::Square);

    auto line = g.addLine();
    g.addStationToLine(line, A);
    g.addStationToLine(line, B);
    g.addTrain(line, 1);

    g.spawnPassengerAt(A, StationType::Square);

    for (int i = 0; i < 10; ++i)
        g.tick();

    EXPECT_EQ(g.completedPassengers(), 1);
}

TEST(Week7Determinism, PolicyAndAgingDeterministic) {
    Graph g1, g2;
    g1.setBoardingPolicy(BoardingPolicy::AGING_PRIORITY);
    g2.setBoardingPolicy(BoardingPolicy::AGING_PRIORITY);

    auto setup = [](Graph& g) {
        auto A = g.addStation(StationType::Circle);
        auto B = g.addStation(StationType::Square);
        auto line = g.addLine();
        g.addStationToLine(line, A);
        g.addStationToLine(line, B);
        g.addTrain(line, 1);
        g.spawnPassengerAt(A, StationType::Square);
    };

    setup(g1);
    setup(g2);

    for (int i = 0; i < 5; ++i) {
        g1.tick();
        g2.tick();
    }

    EXPECT_EQ(g1.completedPassengers(), g2.completedPassengers());
}

TEST(PassengerFSM, IllegalTransitionFails) {
    Passenger p = {1, StationType::Circle, StationType::Square, PassengerState::WAITING};
    p.state = PassengerState::COMPLETED;

    EXPECT_THROW(PassengerFSM::waitingToOnTrain(p, 1), std::logic_error);
}
