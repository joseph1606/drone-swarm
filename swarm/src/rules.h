#pragma once

#include "swarm.h"
#include "vec2.h"

struct NeighborSums {
    float2 separation;
    float2 velocity;
    float2 offset;
    int count;
};

HOST_DEVICE inline float wrapDelta(float delta, float size) {
    if (delta > 0.5f * size) return delta - size;
    if (delta < -0.5f * size) return delta + size;
    return delta;
}

HOST_DEVICE inline float wrapPosition(float x, float size) {
    if (x < 0.0f) return x + size;
    if (x >= size) return x - size;
    return x;
}

HOST_DEVICE inline void addNeighbor(NeighborSums& sums, float2 selfPos, float2 otherPos,
                                    float2 otherVel, const Params& params) {
    const float2 offset = make_float2(wrapDelta(otherPos.x - selfPos.x, params.worldSize),
                                      wrapDelta(otherPos.y - selfPos.y, params.worldSize));
    const float distanceSquared = dot(offset, offset);
    if (distanceSquared >= params.neighborRadius * params.neighborRadius) return;

    sums.velocity += otherVel;
    sums.offset += offset;
    ++sums.count;

    constexpr float kMinDistanceSquared = 1e-6f;
    if (distanceSquared < params.separationRadius * params.separationRadius) {
        sums.separation -= offset / fmaxf(distanceSquared, kMinDistanceSquared);
    }
}

HOST_DEVICE inline void moveDrone(float2& pos, float2& vel, const NeighborSums& sums,
                                  const Params& params) {
    float2 acceleration = params.separationWeight * sums.separation;
    if (sums.count > 0) {
        const float inverseCount = 1.0f / sums.count;
        acceleration += params.alignmentWeight * (sums.velocity * inverseCount - vel);
        acceleration += params.cohesionWeight * (sums.offset * inverseCount);
    }

    vel += acceleration * params.dt;
    const float speed = length(vel);
    if (speed > params.maxSpeed) vel *= params.maxSpeed / speed;

    pos += vel * params.dt;
    pos.x = wrapPosition(pos.x, params.worldSize);
    pos.y = wrapPosition(pos.y, params.worldSize);
}

HOST_DEVICE inline void updateDroneAllPairs(int i, const float2* pos, const float2* vel,
                                            float2* nextPos, float2* nextVel,
                                            const Params& params) {
    const float2 selfPos = pos[i];
    NeighborSums sums = {};
    for (int j = 0; j < params.count; ++j) {
        if (j != i) addNeighbor(sums, selfPos, pos[j], vel[j], params);
    }

    float2 newPos = selfPos;
    float2 newVel = vel[i];
    moveDrone(newPos, newVel, sums, params);
    nextPos[i] = newPos;
    nextVel[i] = newVel;
}
