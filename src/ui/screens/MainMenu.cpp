#include "ui/screens/MainMenu.hpp"
#include "ui/widgets/Button.hpp"
#include <raylib.h>

ScreenResult MainMenu::update() {
    DrawText("METRO SIM", 500, 200, 40, BLACK);

    if (DrawButton("Start", {540, 300, 240, 50}))
        return {AppState::LEVEL_SELECT};

    if (DrawButton("Exit", {540, 360, 240, 50}))
        return {AppState::EXIT};

    return {AppState::MAIN_MENU};
}
