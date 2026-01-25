#pragma once
#include "id.hpp"
#include <vector>

struct RouteInfo {
    bool reachable;
    std::vector<StationId> path;

    RouteInfo(bool r, std::vector<StationId> p) : reachable(r), path(p) {
    }
};