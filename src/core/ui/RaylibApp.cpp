#include "RaylibApp.hpp"
#include "DrawSnapshot.hpp"
#include <raylib.h>

RaylibApp::RaylibApp() {
    InitWindow(1200, 800, "Metro Simulation");
    SetTargetFPS(60);

    auto A = graph_.addStation(StationType::Circle);
    auto B = graph_.addStation(StationType::Square);

    auto line = graph_.addLine();
    graph_.addStationToLine(line, A);
    graph_.addStationToLine(line, B);

    graph_.addTrain(line, 2);
    graph_.spawnPassengerAt(A, StationType::Square);
}

void RaylibApp::run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_SPACE))
            paused_ = !paused_;

        if (IsKeyPressed(KEY_N))
            graph_.tick();

        if (!paused_) {
            tickAccumulator_ += GetFrameTime();
            if (tickAccumulator_ > 1.0f) {
                graph_.tick();
                tickAccumulator_ = 0.0f;
            }
        }

        DrawSnapshot(graph_.snapshot());

        DrawText(paused_ ? "PAUSED" : "RUNNING", 20, 20, 20, DARKGRAY);
        DrawText("SPACE = Play/Pause, N = Step", 20, 50, 16, GRAY);

        EndDrawing();
    }

    CloseWindow();
}
