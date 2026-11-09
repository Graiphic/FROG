#pragma once
#include <cstdint>
// Optional local counters, not a validation authority or public wire format.
// Counts are restricted to compound validation/append and the Array provider;
// they do not claim to intercept every SDK parser/owner check in the process.
namespace frog::sdk::runtime::validation_metrics {
struct Counts {
    std::uint64_t types=0,owners=0,values=0,arrayEnvelopes=0;
    std::uint64_t materializations=0,materializedElements=0,signatureChecks=0;
};
inline thread_local Counts *active=nullptr;
}
