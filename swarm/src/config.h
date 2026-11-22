#pragma once

#include <string>

#include "swarm.h"

struct Config {
    int steps;
    int warmup;
    int seed;
    float density;
    float startSpeed;
    float tolerance;
    Params params;
};

bool loadConfig(const std::string& path, Config& config);
