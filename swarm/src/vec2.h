#pragma once

#include <cmath>

#include <cuda_runtime.h>

#ifdef __CUDACC__
#define HOST_DEVICE __host__ __device__
#else
#define HOST_DEVICE
#endif

HOST_DEVICE inline float2 operator+(float2 a, float2 b) {
    return make_float2(a.x + b.x, a.y + b.y);
}

HOST_DEVICE inline float2 operator-(float2 a, float2 b) {
    return make_float2(a.x - b.x, a.y - b.y);
}

HOST_DEVICE inline float2 operator*(float2 a, float s) {
    return make_float2(a.x * s, a.y * s);
}

HOST_DEVICE inline float2 operator*(float s, float2 a) {
    return a * s;
}

HOST_DEVICE inline float2 operator/(float2 a, float s) {
    return make_float2(a.x / s, a.y / s);
}

HOST_DEVICE inline float2& operator+=(float2& a, float2 b) {
    return a = a + b;
}

HOST_DEVICE inline float2& operator-=(float2& a, float2 b) {
    return a = a - b;
}

HOST_DEVICE inline float2& operator*=(float2& a, float s) {
    return a = a * s;
}

HOST_DEVICE inline float dot(float2 a, float2 b) {
    return a.x * b.x + a.y * b.y;
}

HOST_DEVICE inline float length(float2 a) {
    return sqrtf(dot(a, a));
}
