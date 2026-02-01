#include "ui/screens/InGame.hpp"
#include "raylib.h"

InGame::InGame(int level) : level_(level) {
}

ScreenResult InGame::update() {
    DrawText("IN GAME", 40, 40, 30, BLACK);
    DrawText(TextFormat("Level %d", level_), 40, 80, 20, DARKGRAY);

    if (IsKeyPressed(KEY_ESCAPE))
        return {AppState::PAUSED};

    return {AppState::IN_GAME};
}
