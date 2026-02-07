#pragma once
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

struct StationInit {
    float x, y;
    std::string type;
    int passengers;
};

struct LineInit {
    int id;
    std::string hex;
};

struct LevelConfig {
    int id;
    std::string name;
    uint64_t seed;
    float initialInterval;
    float rampRate;
    float minInterval;
    int initialTrains;
    std::vector<LineInit> initialLines;
    std::vector<StationInit> initialStations;
};

class LevelLoader {
  public:
    static LevelConfig loadLevel(int levelId, const std::string& path = "assets/levels.json");
};