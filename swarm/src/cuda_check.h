#pragma once

#include <cstdio>
#include <cstdlib>

#include <cuda_runtime.h>

inline void checkCuda(cudaError_t error, const char* file, int line) {
    if (error != cudaSuccess) {
        std::fprintf(stderr, "CUDA error in %s line %d: %s\n", file, line,
                     cudaGetErrorString(error));
        std::exit(EXIT_FAILURE);
    }
}

inline void checkKernel(const char* file, int line) {
    checkCuda(cudaGetLastError(), file, line);
#ifdef DEBUG
    checkCuda(cudaDeviceSynchronize(), file, line);
#endif
}

#define CUDA_CHECK(call) checkCuda((call), __FILE__, __LINE__)
#define CUDA_CHECK_KERNEL() checkKernel(__FILE__, __LINE__)
