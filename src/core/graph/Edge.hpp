#pragma once
#include "core/graph/id.hpp"

class Edge {
    StationId from;
    StationId to;
    
    float distance;
};