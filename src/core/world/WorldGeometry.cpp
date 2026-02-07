#include "core/world/WorldGeometry.hpp"

Polyline WorldGeometry::getOctilinearPath(Vector2 start, Vector2 end) {
    Polyline path;
    path.addPoint(start);

    float dx = end.x - start.x;
    float dy = end.y - start.y;

    float absDx = std::abs(dx);
    float absDy = std::abs(dy);

    // Determine the diagonal component
    float diagDist = std::min(absDx, absDy);

    // Calculate the first "kink" point
    Vector2 kink;
    kink.x = start.x + (dx > 0 ? diagDist : -diagDist);
    kink.y = start.y + (dy > 0 ? diagDist : -diagDist);

    path.addPoint(kink);
    path.addPoint(end); // Final straight segment (either purely horizontal or vertical)

    return path;
}