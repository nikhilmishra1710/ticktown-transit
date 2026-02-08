#include "ui/screens/LevelSelect.hpp"
#include "core/utils/LevelLoader.hpp"
#include "ui/widgets/Button.hpp"
#include <raylib.h>

LevelSelect::LevelSelect() {
    levelMeta_ = LevelLoader::getAvailableLevels();
    scrollOffset_ = 0.0f;
    animationStates_.clear();
    int max_rows = (levelMeta_.size()) / 3 + ((levelMeta_.size() % 3 == 0) ? 0 : 1);
    if (max_rows * 450.0f > GetScreenHeight()) {
        maxScrollOffset_ = max_rows * 450.0f - GetScreenHeight();
    } else {
        maxScrollOffset_ = 0.0f;
    }
}

void DrawTextWrapped(const char* text, float x, float y, float maxWidth, int fontSize,
                     Color color) {
    std::string words = text;
    std::string line = "";
    std::string word = "";
    float cursorY = y;
    std::stringstream ss(words);

    while (ss >> word) {
        std::string testLine = line + word + " ";
        if (MeasureText(testLine.c_str(), fontSize) > maxWidth) {
            DrawText(line.c_str(), x, cursorY, fontSize, color);
            line = word + " ";
            cursorY += fontSize + 4;
        } else {
            line = testLine;
        }
    }
    DrawText(line.c_str(), x, cursorY, fontSize, color);
}

void DrawMiniMap(const LevelConfig& cfg, Rectangle bounds) {
    if (cfg.initialStations.empty())
        return;

    // 1. Find the bounds of the stations to scale them
    float minX = cfg.initialStations[0].x, maxX = minX;
    float minY = cfg.initialStations[0].y, maxY = minY;

    for (const auto& s : cfg.initialStations) {
        minX = std::min(minX, s.x);
        maxX = std::max(maxX, s.x);
        minY = std::min(minY, s.y);
        maxY = std::max(maxY, s.y);
    }

    float width = maxX - minX;
    float height = maxY - minY;
    float padding = 20.0f; // Keep icons away from edges

    auto getMapPos = [&](float x, float y) -> Vector2 {
        float nx = (width == 0) ? 0.5f : (x - minX) / width;
        float ny = (height == 0) ? 0.5f : (y - minY) / height;
        return {bounds.x + padding + nx * (bounds.width - 2 * padding),
                bounds.y + padding + ny * (bounds.height - 2 * padding)};
    };

    // 2. Draw Lines (stylized)
    for (const auto& line : cfg.initialLines) {
        // We'll just draw a single connected line through all stations for the preview
        Color col = GetColor(std::stoul(line.hex.substr(1), nullptr, 16) << 8 | 0xFF);
        for (size_t i = 0; i + 1 < cfg.initialStations.size(); ++i) {
            DrawLineEx(getMapPos(cfg.initialStations[i].x, cfg.initialStations[i].y),
                       getMapPos(cfg.initialStations[i + 1].x, cfg.initialStations[i + 1].y), 2.0f,
                       col);
        }
    }

    // 3. Draw Stations as simple dots
    for (const auto& s : cfg.initialStations) {
        DrawCircleV(getMapPos(s.x, s.y), 4.0f, DARKGRAY);
    }
}

void LevelSelect::drawLevelCard(const LevelMetadata& meta, Rectangle bounds, float alpha,
                                float scale) {
    float w = bounds.width * scale;
    float h = bounds.height * scale;
    Rectangle b = {bounds.x + (bounds.width - w) / 2, bounds.y + (bounds.height - h) / 2, w, h};

    // Background
    DrawRectangleRounded(b, 0.05f, 10, Fade(RAYWHITE, alpha));
    DrawRectangleRoundedLines(b, 0.05f, 10, 2.0f, Fade(LIGHTGRAY, alpha));

    // Mini Map Area
    Rectangle mapArea = {b.x + 10, b.y + 10, b.width - 20, 180};
    DrawRectangleRec(mapArea, Fade(BLACK, alpha * 0.05f));

    // Load full config just for the preview
    auto fullCfg = LevelLoader::loadLevel(meta.id);
    DrawMiniMap(fullCfg, mapArea);

    // Text Content
    DrawText(meta.name.c_str(), b.x + 15, b.y + 200, 24, Fade(BLACK, alpha));
    DrawTextWrapped(meta.description.c_str(), b.x + 15, b.y + 240, b.width - 30, 16,
                    Fade(GRAY, alpha));
}

bool LevelSelect::handleInteraction(const LevelMetadata& meta, Vector2 position) {
    float width = 300.0f;
    float height = 400.0f;
    Rectangle bounds = {position.x, position.y, width, height};

    if (CheckCollisionPointRec(GetMousePosition(), bounds)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true; // Level selected
        }
    }
    return false;
}

ScreenResult LevelSelect::update() {
    auto levels = levelMeta_;
    Vector2 mouse = GetMousePosition();
    scrollOffset_ -= GetMouseWheelMove() * 20.0f;

    if (scrollOffset_ < 0)
        scrollOffset_ = 0;
    else if (scrollOffset_ > maxScrollOffset_)
        scrollOffset_ = maxScrollOffset_;

    for (size_t i = 0; i < levels.size(); ++i) {
        auto& state = animationStates_[levels[i].id];
        int row = i / 3;
        int col = i % 3;
        // Calculate raw position
        float startX = GetScreenWidth() / 4.0f + col * 350 - 150;
        float startY = 50.0f + row * 450.0f - scrollOffset_;
        Rectangle cardBounds = {startX, startY, 300, 400};

        // Only animate if inside window
        if (startY > -400 && startY < GetScreenHeight()) {
            state.revealTimer = std::min(1.0f, state.revealTimer + GetFrameTime() * 2.0f);
        }

        // Apply Fade and Zoom math
        float alpha = state.revealTimer;
        float scale = 0.9f + (state.revealTimer * 0.1f);
        if (CheckCollisionPointRec(mouse, cardBounds))
            scale += 0.05f; // Hover zoom

        // Draw with transformations
        drawLevelCard(levels[i], cardBounds, alpha, scale);
        if (handleInteraction(levels[i], {startX, startY})) {
            return {AppState::IN_GAME, levels[i].id};
        }
    }

    if (DrawButton("Back", {520, 450, 240, 50}))
        return {AppState::MAIN_MENU};

    return {AppState::LEVEL_SELECT};
}
