#include "simulation.h"

#include <chrono>
#include <cmath>
#include <random>
#include <utility>

#include "cuda_check.h"

namespace {

constexpr float kPi = 3.14159265f;

class DeviceSwarm {
public:
    explicit DeviceSwarm(int count) {
        CUDA_CHECK(cudaMalloc(&swarm_.pos, count * sizeof(float2)));
        CUDA_CHECK(cudaMalloc(&swarm_.vel, count * sizeof(float2)));
    }
    ~DeviceSwarm() {
        cudaFree(swarm_.pos);
        cudaFree(swarm_.vel);
    }
    DeviceSwarm(const DeviceSwarm&) = delete;
    DeviceSwarm& operator=(const DeviceSwarm&) = delete;

    Swarm view() { return swarm_; }

private:
    Swarm swarm_{};
};

void upload(const HostSwarm& host, Swarm device) {
    const size_t bytes = host.pos.size() * sizeof(float2);
    CUDA_CHECK(cudaMemcpy(device.pos, host.pos.data(), bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(device.vel, host.vel.data(), bytes, cudaMemcpyHostToDevice));
}

void download(Swarm device, HostSwarm& host) {
    const size_t bytes = host.pos.size() * sizeof(float2);
    CUDA_CHECK(cudaMemcpy(host.pos.data(), device.pos, bytes, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaMemcpy(host.vel.data(), device.vel, bytes, cudaMemcpyDeviceToHost));
}

void waitForDevice(const Version& version) {
    if (version.onGpu) CUDA_CHECK(cudaDeviceSynchronize());
}

double timeSteps(const Version& version, Swarm& current, Swarm& next, const Params& params,
                 int steps, int warmup) {
    for (int i = 0; i < warmup; ++i) version.step(current, next, params);
    waitForDevice(version);

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < steps; ++i) {
        version.step(current, next, params);
        std::swap(current, next);
    }
    waitForDevice(version);

    const std::chrono::duration<double, std::milli> elapsed =
        std::chrono::steady_clock::now() - start;
    return elapsed.count() / steps;
}

}  // namespace

HostSwarm randomSwarm(const Params& params, float startSpeed, int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> coordinate(0.0f, params.worldSize);
    std::uniform_real_distribution<float> heading(0.0f, 2.0f * kPi);

    HostSwarm swarm{std::vector<float2>(params.count), std::vector<float2>(params.count)};
    for (int i = 0; i < params.count; ++i) {
        const float x = coordinate(rng);
        const float y = coordinate(rng);
        const float angle = heading(rng);
        swarm.pos[i] = make_float2(x, y);
        swarm.vel[i] = make_float2(startSpeed * std::cos(angle), startSpeed * std::sin(angle));
    }
    return swarm;
}

double simulate(const Version& version, HostSwarm& swarm, const Params& params, int steps,
                int warmup) {
    if (!version.onGpu) {
        HostSwarm scratch{std::vector<float2>(params.count), std::vector<float2>(params.count)};
        Swarm current = swarm.view();
        Swarm next = scratch.view();
        const double msPerStep = timeSteps(version, current, next, params, steps, warmup);
        if (steps % 2 == 1) std::swap(swarm, scratch);
        return msPerStep;
    }

    DeviceSwarm bufferA(params.count);
    DeviceSwarm bufferB(params.count);
    Swarm current = bufferA.view();
    Swarm next = bufferB.view();
    upload(swarm, current);
    const double msPerStep = timeSteps(version, current, next, params, steps, warmup);
    download(current, swarm);
    return msPerStep;
}
