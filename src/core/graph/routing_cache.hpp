#pragma once
#include "StationType.hpp"
#include "route_info.hpp"
#include <unordered_map>

class Graph;

struct RouteKey {
    StationId source;
    StationType destination;

    bool operator==(const RouteKey& other) const {
        return source == other.source && destination == other.destination;
    }
};

namespace std {
template <> struct hash<RouteKey> {
    std::size_t operator()(const RouteKey& k) const {
        std::size_t h1 = std::hash<StationId>{}(k.source);
        std::size_t h2 = std::hash<StationType>{}(k.destination);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
} // namespace std

class RoutingCache {
  public:
    const RouteInfo& get(StationId source, StationType destination, const Graph& graph);

    void invalidate();

  private:
    std::unordered_map<RouteKey, RouteInfo> cache_;
};
