# Drone Swarm

A 2D drone swarm flocking simulation in CUDA. Each drone steers by three local rules:
separation, alignment, and cohesion.

## Purpose

Every drone reacts to its neighbors, so the work grows quickly with swarm size. This
project measures how much a GPU speeds that up. It compares a CPU baseline with
increasingly optimized GPU versions, and checks that every version matches the CPU baseline.

## Quick start

```bash
git clone https://github.com/joseph1606/drone-swarm.git && cd drone-swarm
./swarm/build.sh
./swarm/run.sh verify 4096
```

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

Modes:

- `cpu`: CPU baseline, every drone checks every other drone
- `naive`: GPU, one thread per drone, every drone checks every other drone
- `verify`: runs one step of every version and compares it with the CPU baseline

Example output on a Tesla T4:

```
$ ./swarm/run.sh naive 4096
Device     Tesla T4
Compute    7.5
Memory     15.6 GB
Version    naive
Drones     4096
Steps      100
Time       1.245 ms per step
```

```
$ ./swarm/run.sh verify 1024
Device     Tesla T4
Compute    7.5
Memory     15.6 GB
Drones     1024
Tolerance  0.001

Version    Max error    Drone    Result
naive      0.00e+00     0        PASS
```

## Settings

All other settings are in `swarm/config.txt`.

| Setting             | Meaning                                                      |
|---------------------|--------------------------------------------------------------|
| `density`           | drones per unit area, sets the world size                    |
| `neighbor_radius`   | distance at which drones see each other                      |
| `separation_radius` | distance at which drones push apart                          |
| `separation_weight` | strength of keeping distance                                 |
| `alignment_weight`  | strength of matching neighbors' direction                    |
| `cohesion_weight`   | strength of moving toward neighbors                          |
| `max_speed`         | speed limit                                                  |
| `start_speed`       | initial speed of every drone                                 |
| `dt`                | seconds per step                                             |
| `steps`             | timed steps                                                  |
| `warmup`            | untimed steps before timing starts                           |
| `seed`              | random seed for the starting swarm                           |
| `block_size`        | GPU threads per block                                        |
| `tolerance`         | largest difference `verify` accepts                          |

## Debugging

```bash
./swarm/build.sh debug
cd swarm && compute-sanitizer ./drone_swarm config.txt verify 1024
```

## Adding a version

1. Write a step function in a new file under `swarm/src/` and declare it in `swarm.h`.
2. Add one line to `kVersions` in `versions.h`.

It is then available as a mode, and `verify` checks it automatically.

## Planned

- Visualization: save drone positions during a run and render them to a video.

## Google Colab

```
!git clone https://github.com/joseph1606/drone-swarm.git
%cd drone-swarm
!./swarm/build.sh ARCH=-arch=native
!./swarm/run.sh verify 4096
```
