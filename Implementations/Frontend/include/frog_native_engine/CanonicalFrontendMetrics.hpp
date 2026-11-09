#pragma once
// Optional caller-owned observations, never semantic inputs or FIR metadata.
// One synchronous producer invocation may use an instance at a time.
#include <chrono>
#include <cstdint>

namespace frog::native_engine {
enum class CanonicalFrontendPhase { Capture, Semantics, FirConstruction, Sealing };
struct CanonicalFrontendMetrics {
    std::uint64_t captureNanoseconds=0,semanticNanoseconds=0,
        firConstructionNanoseconds=0,sealingNanoseconds=0;
    std::uint64_t captureCalls=0,semanticCalls=0,firConstructionCalls=0,sealingCalls=0;
    // Event admission currently assembles body records while validating them.
    // Its admission bucket must not be reported as validation-only work.
    bool eventAdmissionIncludesAssembly=false;
    bool phaseActive=false;
};

// Outermost scopes own elapsed time. A reader's sealing scope intentionally
// absorbs the required reference rederivation, so it cannot be double-counted
// as another first-pass validation/construction. No clock is read when disabled.
class CanonicalFrontendPhaseScope final {
public:
    CanonicalFrontendPhaseScope(CanonicalFrontendMetrics *metrics,
        CanonicalFrontendPhase phase) noexcept :metrics_(metrics),phase_(phase) {
        if(metrics_&&!metrics_->phaseActive) {
            metrics_->phaseActive=true;owns_=true;begin_=Clock::now();
        }
    }
    ~CanonicalFrontendPhaseScope(){finish();}
    CanonicalFrontendPhaseScope(const CanonicalFrontendPhaseScope &)=delete;
    CanonicalFrontendPhaseScope &operator=(const CanonicalFrontendPhaseScope &)=delete;
    void finish() noexcept {
        if(!owns_)return;
        const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-begin_).count();
        const auto ns=elapsed>0?static_cast<std::uint64_t>(elapsed):0;
        switch(phase_) {
        case CanonicalFrontendPhase::Capture:metrics_->captureNanoseconds+=ns;++metrics_->captureCalls;break;
        case CanonicalFrontendPhase::Semantics:metrics_->semanticNanoseconds+=ns;++metrics_->semanticCalls;break;
        case CanonicalFrontendPhase::FirConstruction:metrics_->firConstructionNanoseconds+=ns;++metrics_->firConstructionCalls;break;
        case CanonicalFrontendPhase::Sealing:metrics_->sealingNanoseconds+=ns;++metrics_->sealingCalls;break;
        }
        metrics_->phaseActive=false;owns_=false;
    }
private:
    using Clock=std::chrono::steady_clock;
    CanonicalFrontendMetrics *metrics_;
    CanonicalFrontendPhase phase_;
    Clock::time_point begin_{};
    bool owns_=false;
};
} // namespace frog::native_engine
