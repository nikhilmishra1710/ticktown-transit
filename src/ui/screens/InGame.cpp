#include "ui/screens/InGame.hpp"
#include "core/graph/StationType.hpp"
#include "core/graph/id.hpp"
#include "core/simulation/Simulation.hpp"
#include "core/simulation/SimulationCommand.hpp"
#include "core/utils/LevelLoader.hpp"
#include "core/utils/utils.hpp"
#include "core/world/Polyline.hpp"
#include "raylib.h"
#include "ui/constants.hpp"
#include "ui/widgets/Button.hpp"
#include <cstdint>
#include <iostream>
#include <utility>

Simulation InGame::InitSimulation(int levelId) {
    auto cfg = LevelLoader::loadLevel(levelId);
    // Return the constructed simulation object
    return Simulation(cfg.seed);
}

InGame::InGame(int levelId)
    : sim_(InitSimulation(levelId)) // Explicitly initialize here!
{
    // Now you can do the rest (adding stations, lines, etc.)
    auto cfg = LevelLoader::loadLevel(levelId);
    for (const auto& st : cfg.initialStations) {
        sim_.enqueueCommand(AddStationCmd{st.x, st.y, stringToType(st.type)});
    }
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
            Vector2 pos = {pair.first, pair.second};
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
            Vector2 pos = {pair.first, pair.second};
            if (CheckCollisionPointCircle(mouse, pos, 15.0f) && stationId != draggingStationId_) {
                std::size_t index = addAtEnd_ ? SIZE_MAX : 0;
                if (zeroStationLine_) {
                    sim_.enqueueCommand(AddStationToLineCmd{.lineId = (uint32_t) selectedLine_,
                                                            .stationId = draggingStationId_,
                                                            .startStationId = 0,
                                                            .index = index});
                }
                sim_.enqueueCommand(AddStationToLineCmd{.lineId = (uint32_t) selectedLine_,
                                                        .stationId = stationId,
                                                        .startStationId = draggingStationId_,
                                                        .index = index});
                break;
            }
        }
        isDragging_ = false;
        selectedLine_ = -1;
        zeroStationLine_ = false;
    }

    if (isDraggingTrain_ && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();

        for (const auto& line : snapshot.lines) {
            for (size_t i = 0; i < line.stationIds.size() - 1; ++i) {
                auto key = std::make_pair(std::min(line.stationIds[i], line.stationIds[i + 1]),
                                          std::max(line.stationIds[i], line.stationIds[i + 1]));

                const auto& poly = snapshot.edgePaths[key];
                bool done = false;
                for (size_t j = 0; j < poly.points.size() - 1; ++j) {
                    // Check if mouse is near this specific polyline segment
                    if (CheckCollisionPointLine(mouse, poly.points[j], poly.points[j + 1], 10)) {
                        sim_.enqueueCommand(AddTrainToLineCmd{.lineId = line.id});
                        done = true;
                        break;
                    }
                }
                if (done)
                    break;
            }
        }
    }

    this->DrawSnapshot(snapshot);

    return {AppState::IN_GAME};
}

void InGame::_renderPassenger(const PassengerView& snap, Vector2 pos) {
    float size = 5.0f;
    Color color = MAROON;

    switch (snap.destination) {
    case StationType::CIRCLE:
        color = RED;
        break;
    case StationType::SQUARE:
        color = GREEN;
        break;
    case StationType::TRIANGLE:
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
    case StationType::CIRCLE:
        DrawCircleV(pos, size, color);
        break;
    case StationType::SQUARE:
        DrawRectangleV({pos.x - size, pos.y - size}, {size * 2, size * 2}, color);
        break;
    case StationType::TRIANGLE:
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

        std::pair<float, float> pos = snap.stationPositions.at(s.id);
        _renderStation(s, (Vector2){pos.first, pos.second});
        y += 20;
    }
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
            Vector2 startPos = {it->second.first, it->second.second};
            Vector2 mousePos = GetMousePosition();

            Polyline tempPath = sim_.getOctilinearPath(startPos, mousePos);
            for (size_t i = 0; i < tempPath.points.size() - 1; ++i) {
                DrawLineEx(tempPath.points[i], tempPath.points[i + 1], 10.0f, GRAY);
                DrawCircleV(tempPath.points[i], 5.0f, GRAY);
            }
            DrawCircleV(tempPath.points.back(), 15.0f, GRAY);
        }
    }

    DrawRectangle(WINDOW_WIDTH - 50, WINDOW_HEIGHT / 2 - 100, 40, 200, GRAY);
    bool lineSelected = false;
    for (size_t i = 0; i < snap.lines.size(); ++i) {
        const auto& line = snap.lines[i];
        if (line.stationIds.size() < 2)
            continue;
        Color lineColor = ColorFromHSV((i * 360) / snap.lines.size(), 0.8f, 0.8f);
        std::cout << "Drawing line " << line.id << " with color (" << (int) lineColor.r << ","
                  << (int) lineColor.g << "," << (int) lineColor.b << ")" << std::endl;
        std::vector<Polyline> paths = snap.linePaths.at(line.id);
        for (const auto& path : paths) {
            for (size_t j = 0; j < path.points.size() - 1; ++j) {
                DrawLineEx(path.points[j], path.points[j + 1], 10.0f, lineColor);
                DrawCircleV(path.points[j + 1], 5.0f, lineColor);
            }
        }
    }

    for (size_t i = 0; i < snap.trains.size(); ++i) {
        const auto& t = snap.trains[i];
        auto it1 = snap.stationPositions.find(t.stationId);
        auto it2 = snap.stationPositions.find(t.nextStationId);
        if (it1 != snap.stationPositions.end() && it2 != snap.stationPositions.end()) {
            std::pair<float, float> pos = snap.trainPositions.at(t.id);
            std::cout << "Train " << t.id << " position: (" << pos.first << ", " << pos.second
                      << ")" << std::endl;
            DrawCircleV({pos.first, pos.second}, 10.0f, DARKGREEN);
        }
    }

    y += 20;

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
