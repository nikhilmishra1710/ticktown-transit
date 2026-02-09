#include "core/world/WorldGeometry.hpp"
#include <iostream>

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

IntersectionResult WorldGeometry::checkIntersection(Vector2 a, Vector2 b, Vector2 c, Vector2 d) {
    // Vectors for the lines
    float dx1 = b.x - a.x;
    float dy1 = b.y - a.y;
    float dx2 = d.x - c.x;
    float dy2 = d.y - c.y;

    float den = dy2 * dx1 - dx2 * dy1;

    // Parallel or coincident lines
    if (std::abs(den) < 1e-6f)
        return {false};

    float dx3 = a.x - c.x;
    float dy3 = a.y - c.y;

    float ua = (dx2 * dy3 - dy2 * dx3) / den;
    float ub = (dx1 * dy3 - dy1 * dx3) / den;

    // Check if the intersection point lies within both segments
    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
        return {true, {a.x + (ua * dx1), a.y + (ua * dy1)}};
    }

    return {false};
}

// Check a full track polyline against a river polyline
bool WorldGeometry::doesTrackNeedBridge(const std::vector<Vector2>& track,
                                        const std::vector<Vector2>& river) {
    for (size_t i = 0; i + 1 < track.size(); ++i) {
        for (size_t j = 0; j + 1 < river.size(); ++j) {
            if (checkIntersection(track[i], track[i + 1], river[j], river[j + 1]).intersects) {
                return true;
            }
        }
    }
    return false;
}

Polyline WorldGeometry::createBridgedPath(Vector2 trackStart, Vector2 trackEnd,
                                          const std::vector<Vector2>& riverPoints,
                                          float riverWidth) {
    Polyline baseTrack = getOctilinearPath(trackStart, trackEnd);
    Polyline result;
    result.addPoint(baseTrack.points.front());

    float halfWidth = riverWidth / 2.0f;

    for (size_t i = 0; i < baseTrack.points.size() - 1; ++i) {
        Vector2 pA = baseTrack.points[i];
        Vector2 pB = baseTrack.points[i + 1];
        Vector2 trackDir = Vector2Normalize({pB.x - pA.x, pB.y - pA.y});

        bool intersected = false;
        // Check EVERY segment of the river
        for (size_t j = 0; j < riverPoints.size() - 1; ++j) {
            auto res = checkIntersection(pA, pB, riverPoints[j], riverPoints[j + 1]);

            if (res.intersects) {
                // 1. Calculate Bank Points exactly at the crossing
                Vector2 bankIn = {res.point.x - trackDir.x * halfWidth,
                                  res.point.y - trackDir.y * halfWidth};
                Vector2 bankOut = {res.point.x + trackDir.x * halfWidth,
                                   res.point.y + trackDir.y * halfWidth};

                result.addPoint(bankIn);

                // 2. Mark this segment as the bridge
                result.bridgeIndices.push_back(result.points.size() - 1);

                result.addPoint(bankOut);
                intersected = true;
                break; // Found the crossing for this track segment
            }
        }
        result.addPoint(pB);
    }
    return result;
}