#pragma once
#include <functional>
#include <memory>
#include <string>

namespace frog::native_engine {
// The host owns reference resolution and all I/O. The Frontend passes the exact
// authored spelling and source location; it never probes a filesystem or CWD.
enum class DraftDependencyKind { Source = 1, TypeDefinition = 2 };
struct DraftDependencyRequest {
    DraftDependencyKind kind = DraftDependencyKind::Source;
    std::string ownerUri;
    std::string authoredReference;
    std::string sourcePointer;
    std::string expectedIdentity;
    std::string expectedVersion;
    std::string expectedSha256;
};
enum class DraftDependencyState {
    Present, Missing, Inaccessible, TemporarilyUnavailable, Unsupported,
    Cancelled, LimitExceeded
};
struct DraftDependencyResult {
    DraftDependencyState state = DraftDependencyState::Missing;
    std::string canonicalUri;
    std::string identity;
    std::string version;
    std::string sha256;
    std::shared_ptr<const std::string> bytes;
    std::string message;
    std::string diagnosticCode;
};
using DraftDependencyResolver = std::function<DraftDependencyResult(const DraftDependencyRequest &)>;
// Checks content and any explicitly declared identity/version/hash. Exceptions
// from the host (including cancellation/limits) propagate to its operation owner.
[[nodiscard]] DraftDependencyResult resolveDraftDependency(
    const DraftDependencyRequest &, const DraftDependencyResolver &);
}
