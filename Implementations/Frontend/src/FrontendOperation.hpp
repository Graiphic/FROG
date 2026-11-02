#pragma once
// Internal boundary of the single public implementation. No Studio/Runtime DTO.
#include <frog/frontend.h>
#include <memory>
#include <functional>
#include <string>
#include <string_view>

namespace frog::native_engine {
class FrogSourceDocument;
class CanonicalExecutionIr;
class CanonicalEventExecutionIr;
struct DraftFrontendPreparation;
}
namespace frog::frontend {
struct OperationResult {
    frog_frontend_outcome outcome=FROG_FRONTEND_REJECTED;
    std::string firJson,diagnosticsJson,sourceMapsJson,provenanceJson,descriptorJson,resolvedSourceJson;
    // Private C++ integration within one pinned static build, not a public C++
    // ABI or Runtime DTO. CABI and Studio receive the SAME admitted owner.
    std::shared_ptr<const native_engine::CanonicalExecutionIr> typedFir;
    std::shared_ptr<const native_engine::CanonicalEventExecutionIr> typedEvent;
    std::shared_ptr<const native_engine::FrogSourceDocument> canonicalSource;
    std::shared_ptr<const native_engine::DraftFrontendPreparation> draftPreparation;
};
struct ApiFailure { frog_frontend_status status; };
[[nodiscard]] bool validUtf8(std::string_view) noexcept;
// Embedded hosts may allocate an artifact identity only after the immutable
// common preparation is known. This callback has no semantic/configuration
// authority and cannot replace the prepared source or FIR.
using PreparedSourceCallback=std::function<std::string(
    std::shared_ptr<const native_engine::FrogSourceDocument>,
    std::shared_ptr<const native_engine::DraftFrontendPreparation>,std::string_view)>;
[[nodiscard]] OperationResult runOperation(frog_frontend_operation,
    const frog_frontend_config &,const frog_frontend_request *,
    std::shared_ptr<const native_engine::FrogSourceDocument> capturedSource={},PreparedSourceCallback preparedSource={});
}
