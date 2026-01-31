#pragma once
#include "../core/graph/Graph.hpp"

class RaylibApp {
  public:
    RaylibApp();
    void run();

  private:
    Graph graph_;
    bool paused_ = true;
    float tickAccumulator_ = 0.0f;
};
