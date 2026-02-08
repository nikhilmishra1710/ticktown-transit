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
    StationId draggingStationId_;
    int availableTrains_ = 0;
    bool isDraggingTrain_ = false;
    int draggingTrainInventoryIdx_ = -1; // Which train in our "dock"
    Vector2 trainDragPos_;

    void _renderPassenger(const PassengerView& snap, Vector2 pos);
    void _renderStation(const StationView& snap, Vector2 pos);
    void DrawSnapshot(const SimulationSnapshot& snap);

  public:
    InGame(int level);
    ScreenResult update() override;
    static Simulation InitSimulation(int levelId);
    void handleTrainDrag(Vector2 mouse, const SimulationSnapshot& snap);
};