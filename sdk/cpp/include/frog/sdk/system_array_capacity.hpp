#pragma once
// Host allocation policy only, never a Source/FIR semantic rule. The caller
// captures an explicit per-run limit and still handles allocator failure.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__unix__)
#include <unistd.h>
#endif
namespace frog::sdk::runtime {
struct ArraySystemMemory {
    std::uint64_t physical=0,commit=0,addressSpace=0;
    std::size_t residentBudget()const noexcept {
        return static_cast<std::size_t>((std::min)({physical,commit,addressSpace,
            std::uint64_t((std::numeric_limits<std::size_t>::max)())}));
    }
};
inline std::optional<ArraySystemMemory> queryArraySystemMemory()noexcept {
#if defined(_WIN32)
    MEMORYSTATUSEX memory{};memory.dwLength=sizeof(memory);
    if(!GlobalMemoryStatusEx(&memory))return {};
    return ArraySystemMemory{memory.ullAvailPhys,memory.ullAvailPageFile,memory.ullAvailVirtual};
#elif defined(_SC_AVPHYS_PAGES) && defined(_SC_PAGESIZE)
    const auto pages=sysconf(_SC_AVPHYS_PAGES),pageSize=sysconf(_SC_PAGESIZE);
    if(pages<0||pageSize<=0||std::uint64_t(pages)>UINT64_MAX/std::uint64_t(pageSize))return {};
    const auto available=std::uint64_t(pages)*std::uint64_t(pageSize);
    return ArraySystemMemory{available,available,std::uint64_t((std::numeric_limits<std::size_t>::max)())};
#else
    return {};
#endif
}
} // namespace frog::sdk::runtime
