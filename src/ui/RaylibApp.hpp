#pragma once
#include "ui/AppState.hpp"
#include "ui/Screen.hpp"
#include <memory>

class RaylibApp {
  public:
    RaylibApp();
    void run();

  private:
    AppState state_;
    int activeLevel_;

    std::unique_ptr<Screen> screen_;

    void switchState(AppState next);
};
