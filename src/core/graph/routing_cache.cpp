#include "routing_cache.hpp"
#include "Graph.hpp"
#include "StationType.hpp"
#include "id.hpp"

const RouteInfo& RoutingCache::get(StationId source, StationType destination, const Graph& graph) {
    RouteKey key = {source, destination};
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        return it->second;
    }

    RouteInfo info = graph.computeRoute(source, destination);

    auto [insertedIt, _] = cache_.emplace(key, std::move(info));
    return insertedIt->second;
}

void RoutingCache::invalidate() {
    cache_.clear();
}
