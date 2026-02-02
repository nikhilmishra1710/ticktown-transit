#include "ui/screens/InGame.hpp"
#include "core/simulation/LevelRegistry.hpp"
#include "raylib.h"
#include "ui/DrawSnapshot.hpp"

InGame::InGame(int levelId)
    : sim_([&] {
          auto cfg = getLevelConfig(levelId);
          Simulation s(cfg.randomSeed);

          // Level bootstrapping
          for (uint32_t i = 0; i < cfg.initialStations; ++i) {
              s.enqueueCommand(AddStationCmd{.x = 200.0f + i * 200.0f, .y = 360.0f});
          }
          for (uint32_t i = 0; i < cfg.initialStations; ++i) {
              for (uint32_t j = 0; j < cfg.initialPassengers; ++j) {
                  s.enqueueCommand(AddPassengerCmd{i+1});
              }
          }

          for (uint32_t i = 0; i < cfg.initialLines; ++i) {
              s.enqueueCommand(AddLineCmd{i});
          }

          return s;
      }()) {
}

ScreenResult InGame::update() {
    if (IsKeyPressed(KEY_P)) {
        return {AppState::PAUSED};
    }

    if (!paused_) {
        sim_.step(std::chrono::milliseconds(16));
    }

    auto snapshot = sim_.snapshot();
    DrawSnapshot(snapshot);

    return {AppState::IN_GAME};
}
