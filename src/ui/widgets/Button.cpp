#include "ui/widgets/Button.hpp"

bool DrawButton(const char* text, Rectangle bounds, int fontSize) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color bg = hovered ? LIGHTGRAY : GRAY;
    DrawRectangleRec(bounds, bg);
    DrawRectangleLinesEx(bounds, 2, DARKGRAY);

    int textWidth = MeasureText(text, fontSize);
    DrawText(text, bounds.x + (bounds.width - textWidth) / 2,
             bounds.y + (bounds.height - fontSize) / 2, fontSize, BLACK);

    return clicked;
}
