#pragma once
#include <string>

namespace frog::native_engine {
// Persisted Studio build intent. Unknown values are retained by document/project
// readers; selection rejects them instead of silently choosing another backend.
// Absence in an old document does not synthesize a persisted property.
struct NativeProgramOptions {
    int version = 1;
    std::string targetId = "windows-x64";
    std::string backendId = "llvm-22.1.7";
    std::string optimizationProfile = "default";
    bool operator==(const NativeProgramOptions &) const = default;
};
}
