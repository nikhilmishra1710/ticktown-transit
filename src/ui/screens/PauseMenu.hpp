#include "ui/Screen.hpp"

class PauseMenu : public Screen {
  private:
    int level_;

  public:
    ScreenResult update() override;
};