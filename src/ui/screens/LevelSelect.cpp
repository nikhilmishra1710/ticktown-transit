#include "ui/screens/LevelSelect.hpp"
#include "ui/widgets/Button.hpp"
#include <raylib.h>

ScreenResult LevelSelect::update() {
    DrawText("Select Level", 500, 150, 32, BLACK);

    for (int i = 0; i < 3; ++i) {
        if (DrawButton(TextFormat("Level %d", i + 1), {520, 250 + (float) i * 60, 240, 50}))
            return {AppState::IN_GAME, i+1};
    }

    if (DrawButton("Back", {520, 450, 240, 50}))
        return {AppState::MAIN_MENU};

    return {AppState::LEVEL_SELECT};
}
