#include "ui/Screen.hpp"

class LevelSelect : public Screen {
  private:
    int level_;

  public:
    ScreenResult update() override;
};