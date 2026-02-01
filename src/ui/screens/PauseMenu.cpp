#include "ui/screens/PauseMenu.hpp"
#include "ui/widgets/Button.hpp"
#include <raylib.h>

ScreenResult PauseMenu::update() {
    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.6f));
    DrawText("PAUSED", 550, 250, 40, WHITE);

    if (DrawButton("Resume", {540, 330, 240, 50}))
        return {AppState::IN_GAME};

    if (DrawButton("Main Menu", {540, 390, 240, 50}))
        return {AppState::MAIN_MENU};

    return {AppState::PAUSED};
}
