#include "core/simulation/Simulation.hpp"
#include "raylib.h"
#include "ui/Screen.hpp"

class InGame : public Screen {
  private:
    int level_;
    Simulation sim_;
    bool paused_;
    bool isDragging_ = false;
    int selectedLine_ = -1;
    bool zeroStationLine_ = false;
    bool addAtEnd_ = true;
    bool isDraggingTrain_ = false;
    StationId draggingStationId_;

    void _renderPassenger(const PassengerView& snap, Vector2 pos);
    void _renderStation(const StationView& snap, Vector2 pos);
    void DrawSnapshot(const SimulationSnapshot& snap);

  public:
    InGame(int level);
    ScreenResult update() override;
};