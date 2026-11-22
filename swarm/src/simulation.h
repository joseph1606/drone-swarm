#pragma once

#include <vector>

#include "swarm.h"
#include "versions.h"

struct HostSwarm {
    std::vector<float2> pos;
    std::vector<float2> vel;

    Swarm view() { return {pos.data(), vel.data()}; }
};

HostSwarm randomSwarm(const Params& params, float startSpeed, int seed);

double simulate(const Version& version, HostSwarm& swarm, const Params& params, int steps,
                int warmup);
