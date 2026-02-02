#include "core/simulation/Level.hpp"

LevelConfig Level1() {
    return {.initialStations = 3, .initialLines = 2, .initialTrainsPerLine = 1, .randomSeed = 42};
}
