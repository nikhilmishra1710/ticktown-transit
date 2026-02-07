#include "core/utils/LevelLoader.hpp"

LevelConfig LevelLoader::loadLevel(int levelId, const std::string& path) {
    std::ifstream f(path);
    json data = json::parse(f);

    for (auto& l : data["levels"]) {
        if (l["id"] == levelId) {
            LevelConfig cfg;
            cfg.name = l["name"];
            cfg.seed = l["seed"];
            cfg.initialInterval = l["difficulty"]["initialSpawnInterval"];
            cfg.rampRate = l["difficulty"]["spawnRampRate"];
            cfg.minInterval = l["difficulty"]["minInterval"];
            cfg.initialTrains = l["resources"]["initialTrains"];

            for (auto& line : l["resources"]["initialLines"]) {
                cfg.initialLines.push_back({line["id"], line["hex"]});
            }

            for (auto& st : l["resources"]["initialStations"]) {
                cfg.initialStations.push_back({st["x"], st["y"], st["type"], st["passengers"]});
            }
            return cfg;
        }
    }
    throw std::runtime_error("Level not found");
}