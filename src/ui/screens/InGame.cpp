#include "ui/screens/InGame.hpp"
#include "core/graph/id.hpp"
#include "core/simulation/LevelRegistry.hpp"
#include "core/simulation/SimulationCommand.hpp"
#include "raylib.h"
#include "raymath.h"
#include "ui/constants.hpp"
#include "ui/widgets/Button.hpp"
#include "ui/widgets/Station.hpp"
#include <cstdint>
#include <iostream>
#include <utility>

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
    Vector2 mouse = GetMousePosition();

    if (!isDragging_ && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        for (const auto& [stationId, pair] : snapshot.stationPositions) {
            Vector2 pos = {pair.first.first, pair.first.second};
            if (CheckCollisionPointCircle(mouse, pos, 15.0f)) {
                std::vector<LineId> zeroStationLines;
                std::cout << "Line numbers: " << snapshot.lines.size() << std::endl;
                for (const auto& line : snapshot.lines) {
                    if (line.stationIds.empty()) {
                        std::cout << "Line " << line.id
                                  << " has no stations, adding to zeroStationLines" << std::endl;
                        zeroStationLines.push_back(line.id);
                    } else if (line.stationIds.front() == stationId ||
                               line.stationIds.back() == stationId) {
                        selectedLine_ = line.id;
                        isDragging_ = true;
                        draggingStationId_ = stationId;
                        addAtEnd_ = (line.stationIds.back() == stationId);
                        break;
                    }
                }
                if (selectedLine_ == -1) {
                    if (!zeroStationLines.empty()) {
                        selectedLine_ = zeroStationLines.front();
                        isDragging_ = true;
                        draggingStationId_ = stationId;
                        zeroStationLine_ = true;
                        addAtEnd_ = true;
                        std::cout << "Selected empty line " << selectedLine_
                                  << " for dragging from station " << stationId << std::endl;
                    }
                }
            }
        }
    }

    if (isDragging_ && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        for (const auto& [stationId, pair] : snapshot.stationPositions) {
            Vector2 pos = {pair.first.first, pair.first.second};
            if (CheckCollisionPointCircle(mouse, pos, 15.0f) && stationId != draggingStationId_) {
                std::size_t index = addAtEnd_ ? SIZE_MAX : 0;
                if (zeroStationLine_) {
                    sim_.enqueueCommand(AddStationToLineCmd{.lineId = (uint32_t) selectedLine_,
                                                            .stationId = draggingStationId_,
                                                            .index = index});
                }
                sim_.enqueueCommand(AddStationToLineCmd{
                    .lineId = (uint32_t) selectedLine_, .stationId = stationId, .index = index});
                break;
            }
        }
        isDragging_ = false;
        selectedLine_ = -1;
        zeroStationLine_ = false;
    }

    this->DrawSnapshot(snapshot);

    return {AppState::IN_GAME};
}

void InGame::_renderPassenger(const PassengerView& snap, Vector2 pos) {
    float size = 5.0f;
    Color color = MAROON;

    switch (snap.destination) {
    case StationType::Circle:
        color = RED;
        break;
    case StationType::Square:
        color = GREEN;
        break;
    case StationType::Triangle:
        color = BLUE;
        break;
    default:
        color = MAROON;
        break;
    }
    DrawCircleV(pos, size, color);
}

void InGame::_renderStation(const StationView& snap, Vector2 pos) {
    float size = 15.0f;
    Color color =
        (CheckCollisionPointCircle(GetMousePosition(), pos, size + 5)) ? SKYBLUE : DARKGRAY;

    switch (snap.type) {
    case StationType::Circle:
        DrawCircleV(pos, size, color);
        break;
    case StationType::Square:
        DrawRectangleV({pos.x - size, pos.y - size}, {size * 2, size * 2}, color);
        break;
    case StationType::Triangle:
        DrawPoly(pos, 3, size + 2, 0, color);
        break;
    default:
        DrawCircleV(pos, size, color);
        break;
    }

    float offset = 20.0f;
    for (size_t i = 0; i < snap.waiting; ++i) {
        _renderPassenger(snap.passengers[i],
                         (Vector2){pos.x + offset + (i % 5) * 8, pos.y - 10 + (i / 5) * 8});
    }
}

void InGame::DrawSnapshot(const SimulationSnapshot& snap) {
    int y = 100;

    DrawText(TextFormat("Tick: %llu", snap.tick), 20, y, 20, BLACK);
    y += 40;
    DrawText(TextFormat("Selected Line: %d", selectedLine_), 20, y, 20, BLACK);
    y += 40;

    DrawText("Stations:", 20, y, 18, DARKBLUE);
    y += 24;

    for (const auto& s : snap.stations) {
        DrawText(TextFormat("Station %u type=%d waiting=%zu", s.id, (int) s.type, s.waiting), 40, y,
                 16, BLACK);

        std::pair<float, float> pos = snap.stationPositions.at(s.id).first;
        _renderStation(s, (Vector2){pos.first, pos.second});
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

    if (isDragging_ && selectedLine_ != -1) {
        // Find the position of the station we started dragging from
        auto it = snap.stationPositions.find(draggingStationId_);
        if (it != snap.stationPositions.end()) {
            Vector2 startPos = {it->second.first.first, it->second.first.second};
            Vector2 mousePos = GetMousePosition();

            // Draw a thick, semi-transparent ghost line
            Color lineCol = ColorFromHSV((selectedLine_ * 60), 0.7f, 0.9f);
            DrawLineEx(startPos, mousePos, 6.0f, Fade(lineCol, 0.5f));

            // Draw a small indicator at the mouse to show "docking" status
            DrawCircleV(mousePos, 5.0f, lineCol);
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
            selectedLine_ = line.id;
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
        if (selectedLine_ != -1) {
            sim_.enqueueCommand(AddTrainToLineCmd{.lineId = (uint32_t) selectedLine_});
        }
    }

    if (lineSelected && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x < WINDOW_WIDTH - 50) {
            selectedLine_ = -1;
        }
    }
}
