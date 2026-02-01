#include "ui/Screen.hpp"

class InGame : public Screen {
  private:
    int level_;

  public:
    InGame(int level);
    ScreenResult update() override;
};