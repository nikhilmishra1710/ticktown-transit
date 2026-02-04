#include "ui/screens/InGame.hpp"
#include "core/simulation/LevelRegistry.hpp"
#include "core/simulation/SimulationCommand.hpp"
#include "raylib.h"
#include "raymath.h"
#include "ui/constants.hpp"
#include "ui/widgets/Button.hpp"
#include "ui/widgets/Station.hpp"
#include <iostream>

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
                  s.enqueueCommand(AddPassengerCmd{i + 1});
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
    this->DrawSnapshot(snapshot);

    return {AppState::IN_GAME};
}

void InGame::DrawSnapshot(const SimulationSnapshot& snap) {
    int y = 100;

    DrawText(TextFormat("Tick: %llu", snap.tick), 20, y, 20, BLACK);
    y += 40;
    DrawText(TextFormat("Selected Line: %d", _selectedLine), 20, y, 20, BLACK);
    y += 40;

    DrawText("Stations:", 20, y, 18, DARKBLUE);
    y += 24;

    for (const auto& s : snap.stations) {
        DrawText(TextFormat("Station %u type=%d waiting=%zu", s.id, (int) s.type, s.waiting), 40, y,
                 16, BLACK);
        y += 20;
    }

    y += 20;
    DrawText("Trains:", 20, y, 18, DARKGREEN);
    y += 24;

    for (const auto& t : snap.trains) {
        DrawText(TextFormat("Train %u line=%u station=%u next station=%u onboard=%zu/%zu State=%u",
                            t.id, t.lineId, t.stationId, t.nextStationId, t.onboard, t.capacity,
                            (int) t.state),
                 40, y, 16, BLACK);
        y += 20;
    }

    y += 20;
    DrawText("Passengers:", 20, y, 18, MAROON);
    y += 24;

    for (const auto& p : snap.passengers) {
        DrawText(TextFormat("P%u %d->%d state=%d age=%zu", p.id, (int) p.origin,
                            (int) p.destination, (int) p.state, p.age),
                 40, y, 16, BLACK);
        y += 18;
    }

    for (const auto& [stationId, pair] : snap.stationPositions) {
        if (DrawStation(stationId, {pair.first.first, pair.first.second}, 14)) {
            if (_selectedLine != -1) {
                sim_.enqueueCommand(AddStationToLineCmd{.lineId = (uint32_t) _selectedLine,
                                                        .stationId = stationId});
            }
        }
        float i = 0.0f;
        for (auto& p : pair.second.passengers) {
            DrawLineV({pair.first.first + i, pair.first.second},
                      {pair.first.first + i, pair.first.second + 30.0f}, RED);
            i += 5.0f;
        }
    }

    DrawRectangle(WINDOW_WIDTH - 50, WINDOW_HEIGHT / 2 - 100, 40, 200, GRAY);
    bool lineSelected = false;
    for (size_t i = 0; i < snap.lines.size(); ++i) {
        const auto& line = snap.lines[i];
        Color lineColor = ColorFromHSV((i * 360) / snap.lines.size(), 0.8f, 0.8f);
        for (size_t j = 0; j + 1 < line.stationIds.size(); ++j) {
            auto it1 = snap.stationPositions.find(line.stationIds[j]);
            auto it2 = snap.stationPositions.find(line.stationIds[j + 1]);
            if (it1 != snap.stationPositions.end() && it2 != snap.stationPositions.end()) {
                DrawLineEx({it1->second.first.first, it1->second.first.second},
                           {it2->second.first.first, it2->second.first.second}, 2.0f, lineColor);
            }
        }
        if (DrawButton(TextFormat("L%d", line.id),
                       {WINDOW_WIDTH - 50, WINDOW_HEIGHT / 2 - 100 + (float) (i * 20), 40, 10})) {
            _selectedLine = line.id;
            lineSelected = true;
        }
    }

    for (size_t i = 0; i < snap.trains.size(); ++i) {
        const auto& t = snap.trains[i];
        auto it1 = snap.stationPositions.find(t.stationId);
        auto it2 = snap.stationPositions.find(t.nextStationId);
        if (it1 != snap.stationPositions.end() && it2 != snap.stationPositions.end()) {
            Vector2 pos = {
                Lerp(it1->second.first.first, it2->second.first.first,
                     (t.forward ? 0.0f : 1.0f) + (t.forward ? t.progress : (t.progress - 1))),
                Lerp(it1->second.first.second, it2->second.first.second,
                     (t.forward ? 0.0f : 1.0f) + (t.forward ? t.progress : (t.progress - 1)))};
            std::cout << "Train " << t.id << " position: (" << pos.x << ", " << pos.y << ")"
                      << std::endl;
            DrawCircleV(pos, 10.0f, DARKGREEN);
        }
    }

    if (DrawButton("Add Train", {WINDOW_WIDTH - 150, 20, 100, 40})) {
        if (_selectedLine != -1) {
            sim_.enqueueCommand(AddTrainToLineCmd{.lineId = (uint32_t) _selectedLine});
        }
    }

    if (lineSelected && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x < WINDOW_WIDTH - 50) {
            _selectedLine = -1;
        }
    }
}
