#include "core/utils/utils.hpp"
#include <stdexcept>

StationType stringToType(const std::string& s) {
    if (s == "CIRCLE")
        return StationType::CIRCLE;
    if (s == "SQUARE")
        return StationType::SQUARE;
    if (s == "TRIANGLE")
        return StationType::TRIANGLE;
    if (s == "STAR")
        return StationType::STAR;
    throw std::runtime_error("Unknown station type: " + s);
}