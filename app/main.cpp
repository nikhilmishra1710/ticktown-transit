#include <chrono>
#include <iostream>
#include "core/simulation/Simulation.hpp"
#include "core/simulation/TickClock.hpp"

int main() {
    using namespace std::chrono;

    Simulation sim(42);
    TickClock clock(16ms);

    for (int i = 0; i < 100; ++i) {
        if (clock.shouldStep(16ms)) {
            sim.step(16ms);
            clock.consumeStep();
        }
    }

    std::cout << "Final hash: " << sim.stateHash() << "\n";
}
