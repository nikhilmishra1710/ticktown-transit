#pragma once
#include "ui/AppState.hpp"

struct ScreenResult {
    AppState nextState;
    int selectedLevel = -1;
};

class Screen {
  public:
    virtual ~Screen() = default;
    virtual ScreenResult update() = 0;
};
