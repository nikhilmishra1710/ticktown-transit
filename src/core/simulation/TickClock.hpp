#pragma once
#include <chrono>

class TickClock {
  public:
    using Duration = std::chrono::milliseconds;

    explicit TickClock(Duration tick);

    bool shouldStep(Duration frameDelta);
    void consumeStep();

  private:
    Duration tick_;
    Duration accumulator_{0};
};
