#include "ui/RaylibApp.hpp"
#include "ui/screens/InGame.hpp"
#include "ui/screens/LevelSelect.hpp"
#include "ui/screens/MainMenu.hpp"
#include "ui/screens/PauseMenu.hpp"
#include <raylib.h>

RaylibApp::RaylibApp() : state_(AppState::MAIN_MENU), activeLevel_(-1) {
    InitWindow(1280, 720, "MetroSim");
    SetTargetFPS(60);
    switchState(state_);
}

bool shouldClose() {
    return (WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) || IsKeyPressed(KEY_Q);
}

void RaylibApp::run() {
    while (!shouldClose() && state_ != AppState::EXIT) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        auto result = screen_->update();
        if (result.nextState != state_) {
            if (result.selectedLevel != -1)
                activeLevel_ = result.selectedLevel;
            switchState(result.nextState);
        }

        EndDrawing();
    }
    CloseWindow();
}

void RaylibApp::switchState(AppState next) {
    state_ = next;
    switch (state_) {
    case AppState::MAIN_MENU:
        screen_ = std::make_unique<MainMenu>();
        break;
    case AppState::LEVEL_SELECT:
        screen_ = std::make_unique<LevelSelect>();
        break;
    case AppState::IN_GAME:
        screen_ = std::make_unique<InGame>(activeLevel_);
        break;
    case AppState::PAUSED:
        screen_ = std::make_unique<PauseMenu>();
        break;
    case AppState::EXIT:
        break;
    }
}
