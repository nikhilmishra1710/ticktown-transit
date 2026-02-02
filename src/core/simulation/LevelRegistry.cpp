#include "core/simulation/LevelRegistry.hpp"
#include <stdexcept>

LevelConfig getLevelConfig(int levelId) {
    switch (levelId) {
    case 1:
        return {
            .initialStations = 3, .initialLines = 2, .initialTrainsPerLine = 1, .randomSeed = 42, .initialPassengers = 5};
    default:
        throw std::logic_error("Invalid level id");
    }
}
