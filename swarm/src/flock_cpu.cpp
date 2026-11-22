#include "rules.h"
#include "swarm.h"

void stepCpu(Swarm current, Swarm next, const Params& params) {
    for (int i = 0; i < params.count; ++i) {
        updateDroneAllPairs(i, current.pos, current.vel, next.pos, next.vel, params);
    }
}
