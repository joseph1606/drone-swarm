# Drone Swarm

A 2D drone swarm flocking simulation in CUDA. Each drone steers by three local rules:
separation, alignment, and cohesion.

## Purpose

Every drone reacts to its neighbors, so the work grows quickly with swarm size. This
project measures how much a GPU speeds that up. It compares a CPU baseline with
increasingly optimized GPU versions, and checks that every version matches the CPU baseline.

## Requirements

- NVIDIA GPU
- CUDA Toolkit 11.6 or newer
- `make` and `bash`

## Build

```bash
./swarm/build.sh                       # release build for all major GPU architectures
./swarm/build.sh ARCH=-arch=native     # faster build for the local GPU only
./swarm/build.sh debug                 # device debug symbols, sync after every kernel
```

## Run

```bash
./swarm/run.sh <mode> <drones>
./swarm/run.sh verify 4096
./swarm/run.sh naive 65536
```

All other settings (flocking rules, steps, seed, block size) are in `swarm/config.txt`.

Modes:

- `cpu`: CPU baseline, every drone checks every other drone
- `naive`: GPU, one thread per drone, every drone checks every other drone
- `verify`: runs one step of every version and compares it with the CPU baseline

## Debugging

```bash
./swarm/build.sh debug
cd swarm && compute-sanitizer ./drone_swarm config.txt verify 1024
```

## Adding a version

1. Write a step function in a new file under `swarm/src/` and declare it in `swarm.h`.
2. Add one line to `kVersions` in `versions.h`.

It is then available as a mode, and `verify` checks it automatically.

## Google Colab

```
!git clone https://github.com/joseph1606/drone-swarm.git
%cd drone-swarm
!./swarm/build.sh ARCH=-arch=native
!./swarm/run.sh verify 4096
```
