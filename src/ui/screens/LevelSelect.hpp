#include "core/utils/LevelLoader.hpp"
#include "raylib.h"
#include "ui/Screen.hpp"

struct CardState {
    float revealTimer = 0.0f;
    bool isVisible = false;
};

class LevelSelect : public Screen {
  private:
    float scrollOffset_ = 0.0f;
    float maxScrollOffset_ = 0.0f;
    std::map<int, CardState> animationStates_;
    std::vector<LevelMetadata> levelMeta_;
    int level_;

  public:
    LevelSelect();
    void drawLevelCard(const LevelMetadata& meta, Rectangle bounds, float alpha, float scale);
    bool handleInteraction(const LevelMetadata& meta, Vector2 position);
    ScreenResult update() override;
};