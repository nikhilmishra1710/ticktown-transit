#include "../src/core/simulation/Simulation.hpp"
#include "../src/core/simulation/TickClock.hpp"
#include <iostream>

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
