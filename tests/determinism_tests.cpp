#include <gtest/gtest.h>
#include "../src/core/simulation/Simulation.hpp"

TEST(Simulation, DeterministicState) {
    Simulation a(123);
    Simulation b(123);

    for (int i = 0; i < 1000; ++i) {
        a.step(std::chrono::milliseconds(16));
        b.step(std::chrono::milliseconds(16));
    }

    EXPECT_EQ(a.stateHash(), b.stateHash());
}
