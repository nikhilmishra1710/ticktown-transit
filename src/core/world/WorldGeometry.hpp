#pragma once
#include "core/world/Polyline.hpp"
#include <raylib.h>

class WorldGeometry {
  public:
    static Polyline getOctilinearPath(Vector2 start, Vector2 end);
};