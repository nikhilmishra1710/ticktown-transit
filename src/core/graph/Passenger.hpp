#pragma once
#include "StationType.hpp"
#include <stdexcept>

struct Passenger {
    StationType origin;
    StationType destination;

    Passenger(StationType o, StationType d) : origin(o), destination(d) {
        if (o == d) {
            throw std::logic_error("Passenger origin == destination");
        }
    }
};