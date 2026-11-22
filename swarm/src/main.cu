#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "config.h"
#include "cuda_check.h"
#include "simulation.h"
#include "verify.h"
#include "versions.h"

namespace {

constexpr const char* kVerifyMode = "verify";

void printUsage(const char* program) {
    std::printf("Usage: %s <config file> <mode> <drones>\nModes:", program);
    for (const Version& version : kVersions) {
        std::printf(" %s", version.name);
    }
    std::printf(" %s\n", kVerifyMode);
}

void printDevice() {
    int device = 0;
    CUDA_CHECK(cudaGetDevice(&device));
    cudaDeviceProp properties;
    CUDA_CHECK(cudaGetDeviceProperties(&properties, device));
    std::printf("%-10s %s\n", "Device", properties.name);
    std::printf("%-10s %d.%d\n", "Compute", properties.major, properties.minor);
    std::printf("%-10s %.1f GB\n", "Memory", properties.totalGlobalMem / 1e9);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 4) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    const std::string mode = argv[2];
    const Version* version = findVersion(mode);
    const int drones = std::atoi(argv[3]);
    if ((version == nullptr && mode != kVerifyMode) || drones <= 0) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    Config config{};
    if (!loadConfig(argv[1], config)) return EXIT_FAILURE;
    config.params.count = drones;
    config.params.worldSize = std::sqrt(drones / config.density);

    HostSwarm swarm = randomSwarm(config.params, config.startSpeed, config.seed);

    if (mode == kVerifyMode) {
        printDevice();
        return verify(swarm, config.params, config.tolerance) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (version->onGpu) printDevice();
    const double msPerStep = simulate(*version, swarm, config.params, config.steps, config.warmup);
    std::printf("%-10s %s\n", "Version", version->name);
    std::printf("%-10s %d\n", "Drones", drones);
    std::printf("%-10s %d\n", "Steps", config.steps);
    std::printf("%-10s %.3f ms per step\n", "Time", msPerStep);
    return EXIT_SUCCESS;
}
