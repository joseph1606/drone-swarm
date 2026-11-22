#include "verify.h"

#include <cmath>
#include <cstdio>

#include "rules.h"

namespace {

constexpr int kVerifySteps = 1;

float droneError(const HostSwarm& a, const HostSwarm& b, int i, float worldSize) {
    const float2 positionGap = make_float2(wrapDelta(a.pos[i].x - b.pos[i].x, worldSize),
                                           wrapDelta(a.pos[i].y - b.pos[i].y, worldSize));
    return std::fmax(length(positionGap), length(a.vel[i] - b.vel[i]));
}

void printDrone(const char* name, const HostSwarm& swarm, int i) {
    std::printf("  %-8s position %.6f %.6f  velocity %.6f %.6f\n", name, swarm.pos[i].x,
                swarm.pos[i].y, swarm.vel[i].x, swarm.vel[i].y);
}

bool compare(const char* name, const HostSwarm& expected, const HostSwarm& actual,
             const Params& params, float tolerance) {
    float maxError = 0.0f;
    int worstDrone = 0;
    for (int i = 0; i < params.count; ++i) {
        const float error = droneError(expected, actual, i, params.worldSize);
        if (std::isnan(error) || error > maxError) {
            maxError = error;
            worstDrone = i;
        }
    }

    const bool passed = maxError <= tolerance;
    std::printf("%-10s %-12.2e %-8d %s\n", name, maxError, worstDrone, passed ? "PASS" : "FAIL");
    if (!passed) {
        printDrone(kReferenceVersion, expected, worstDrone);
        printDrone(name, actual, worstDrone);
    }
    return passed;
}

}  // namespace

bool verify(const HostSwarm& start, const Params& params, float tolerance) {
    std::printf("%-10s %d\n", "Drones", params.count);
    std::printf("%-10s %g\n\n", "Tolerance", tolerance);
    std::printf("%-10s %-12s %-8s %s\n", "Version", "Max error", "Drone", "Result");

    const Version& reference = *findVersion(kReferenceVersion);
    HostSwarm expected = start;
    simulate(reference, expected, params, kVerifySteps, 0);

    bool allPassed = true;
    for (const Version& version : kVersions) {
        if (&version == &reference) continue;
        HostSwarm actual = start;
        simulate(version, actual, params, kVerifySteps, 0);
        allPassed &= compare(version.name, expected, actual, params, tolerance);
    }
    return allPassed;
}
