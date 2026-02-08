#include "core/utils/LevelLoader.hpp"
#include <fstream>

// Initialize static members
std::vector<LevelMetadata> LevelLoader::levelsMetadata_;
nlohmann::json LevelLoader::cachedJson_;
bool LevelLoader::isLoaded_ = false;

// LevelLoader.cpp

void LevelLoader::ensureLoaded(const std::string& path) {
    if (isLoaded_)
        return;

    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Could not open levels file: " + path);
    }

    // Parse the entire file into memory once
    cachedJson_ = nlohmann::json::parse(f);

    // Populate metadata cache for the menu
    levelsMetadata_.clear();
    for (const auto& l : cachedJson_["levels"]) {
        LevelMetadata meta;
        meta.id = l["id"];
        meta.name = l["name"];
        meta.description = l.value("description", "");
        levelsMetadata_.push_back(meta);
    }

    isLoaded_ = true;
}

const std::vector<LevelMetadata>& LevelLoader::getAvailableLevels(const std::string& path) {
    ensureLoaded(path);
    return levelsMetadata_;
}

LevelConfig LevelLoader::loadLevel(int levelId, const std::string& path) {
    ensureLoaded(path);

    for (const auto& l : cachedJson_["levels"]) {
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
    throw std::runtime_error("Level ID not found in cache: " + std::to_string(levelId));
}