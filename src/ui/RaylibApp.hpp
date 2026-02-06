#pragma once
#include "ui/AppState.hpp"
#include "ui/Screen.hpp"
#include <cstddef>
#include <memory>

class RaylibApp {
  public:
    RaylibApp();
    void run();

  private:
    AppState state_;
    int activeLevel_;

    std::unique_ptr<Screen> screen_;
    std::unique_ptr<Screen> prevScreen_ = NULL;

    void switchState(AppState next);
};
