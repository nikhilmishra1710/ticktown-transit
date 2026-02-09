#pragma once
#include "core/world/Polyline.hpp"
#include <raylib.h>

struct IntersectionResult {
    bool intersects = false;
    Vector2 point = {0, 0};
};

class WorldGeometry {
  public:
    static Polyline getOctilinearPath(Vector2 start, Vector2 end);
    static IntersectionResult checkIntersection(Vector2 a, Vector2 b, Vector2 c, Vector2 d);
    static bool doesTrackNeedBridge(const std::vector<Vector2>& track,
                                    const std::vector<Vector2>& river);

    static Polyline createBridgedPath(Vector2 trackStart, Vector2 trackEnd,
                                      const std::vector<Vector2>& riverPoints, float riverWidth);
};