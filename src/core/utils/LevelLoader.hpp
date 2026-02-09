#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
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
    std::map<std::uint32_t, std::pair<float, std::vector<std::pair<float, float>>>> geography;
};

struct LevelMetadata {
    int id;
    std::string name;
    std::string description;
};

class LevelLoader {
  public:
    static LevelConfig loadLevel(int levelId, const std::string& path = "assets/levels.json");
    static const std::vector<LevelMetadata>&
    getAvailableLevels(const std::string& path = "assets/levels.json");

  private:
    static void ensureLoaded(const std::string& path);

    static std::vector<LevelMetadata> levels_;
    static std::vector<LevelMetadata> levelsMetadata_;
    static nlohmann::json cachedJson_;
    static bool isLoaded_;
};