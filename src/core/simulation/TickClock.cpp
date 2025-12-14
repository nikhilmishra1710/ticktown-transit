#include "TickClock.hpp"

TickClock::TickClock(Duration tick) : tick_(tick) {}

bool TickClock::shouldStep(Duration frameDelta) {
    accumulator_ += frameDelta;
    return accumulator_ >= tick_;
}

void TickClock::consumeStep() {
    accumulator_ -= tick_;
}
