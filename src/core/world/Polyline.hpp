#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Polyline {
    std::vector<Vector2> points;
    float totalLength = 0.0f;
    bool bridge = false;
    std::vector<size_t> bridgeIndices; // Store intersection points with rivers

    void addPoint(Vector2 p) {
        if (!points.empty()) {
            totalLength += Vector2Distance(points.back(), p);
        }
        points.push_back(p);
    }
};