#include "config.h"

#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>

bool loadConfig(const std::string& path, Config& config) {
    Params& params = config.params;
    const std::map<std::string, float*> floats = {
        {"density", &config.density},
        {"start_speed", &config.startSpeed},
        {"tolerance", &config.tolerance},
        {"neighbor_radius", &params.neighborRadius},
        {"separation_radius", &params.separationRadius},
        {"separation_weight", &params.separationWeight},
        {"alignment_weight", &params.alignmentWeight},
        {"cohesion_weight", &params.cohesionWeight},
        {"max_speed", &params.maxSpeed},
        {"dt", &params.dt},
    };
    const std::map<std::string, int*> ints = {
        {"steps", &config.steps},
        {"warmup", &config.warmup},
        {"seed", &config.seed},
        {"block_size", &params.blockSize},
    };

    std::ifstream file(path);
    if (!file) {
        std::fprintf(stderr, "Cannot open config file %s\n", path.c_str());
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line.substr(0, line.find('#')));
        std::string key;
        if (!(stream >> key)) continue;

        if (const auto floatSetting = floats.find(key); floatSetting != floats.end()) {
            stream >> *floatSetting->second;
        } else if (const auto intSetting = ints.find(key); intSetting != ints.end()) {
            stream >> *intSetting->second;
        } else {
            std::fprintf(stderr, "Unknown setting %s in %s\n", key.c_str(), path.c_str());
            return false;
        }
    }
    return true;
}
