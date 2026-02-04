#include <cstdint>
#include <raylib.h>

bool DrawStation(uint32_t stationId, Vector2 center, int fontSize = 20) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointCircle(mouse, center, 20.0f);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    DrawCircleV({center.x, center.y}, 20.0f, BLUE);
    DrawText(TextFormat("%u", stationId), center.x - 10.0f, center.y - 10.0f, 12, WHITE);

    return clicked;
}