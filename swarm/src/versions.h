#pragma once

#include <string>

#include "swarm.h"

struct Version {
    const char* name;
    bool onGpu;
    void (*step)(Swarm current, Swarm next, const Params& params);
};

inline constexpr Version kVersions[] = {
    {"cpu", false, stepCpu},
    {"naive", true, stepNaive},
};

inline constexpr const char* kReferenceVersion = "cpu";

inline const Version* findVersion(const std::string& name) {
    for (const Version& version : kVersions) {
        if (name == version.name) return &version;
    }
    return nullptr;
}
