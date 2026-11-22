#include "cuda_check.h"
#include "rules.h"
#include "swarm.h"

namespace {

__global__ void naiveKernel(const float2* __restrict__ pos, const float2* __restrict__ vel,
                            float2* __restrict__ nextPos, float2* __restrict__ nextVel,
                            Params params) {
    const int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < params.count) updateDroneAllPairs(i, pos, vel, nextPos, nextVel, params);
}

}  // namespace

void stepNaive(Swarm current, Swarm next, const Params& params) {
    const int blocks = (params.count + params.blockSize - 1) / params.blockSize;
    naiveKernel<<<blocks, params.blockSize>>>(current.pos, current.vel, next.pos, next.vel, params);
    CUDA_CHECK_KERNEL();
}
