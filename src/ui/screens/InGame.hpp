#include "core/simulation/Simulation.hpp"
#include "ui/Screen.hpp"

class InGame : public Screen {
  private:
    int level_;
    Simulation sim_;
    bool paused_;
    void DrawSnapshot(const SimulationSnapshot& snap);
    int _selectedLine = -1;

  public:
    InGame(int level);
    ScreenResult update() override;
};