#pragma once

#include <cuda_runtime.h>

struct Params {
    int count;
    float worldSize;
    float neighborRadius;
    float separationRadius;
    float separationWeight;
    float alignmentWeight;
    float cohesionWeight;
    float maxSpeed;
    float dt;
    int blockSize;
};

struct Swarm {
    float2* pos;
    float2* vel;
};

void stepCpu(Swarm current, Swarm next, const Params& params);
void stepNaive(Swarm current, Swarm next, const Params& params);
