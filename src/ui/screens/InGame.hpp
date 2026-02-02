#include "core/simulation/Simulation.hpp"
#include "ui/Screen.hpp"

class InGame : public Screen {
  private:
    int level_;
    Simulation sim_;
    bool paused_;

  public:
    InGame(int level);
    ScreenResult update() override;
};