#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace frog::native_engine {

inline constexpr std::string_view kFrogDraftDocumentFormat = "frog.document.draft";
inline constexpr std::uint32_t kLegacyFrogDraftRevision = 0U;
inline constexpr std::uint32_t kCurrentFrogDraftRevision = 2U;
// Opt-in storage envelope. Ordinary readable files keep revision 2.
inline constexpr std::uint32_t kFrogArrayStorageDraftRevision = 3U;
// Conditional private state encoding: older readers must reject, not narrow.
inline constexpr std::uint32_t kFrogExactIntegerStateDraftRevision = 4U;
// Conditional complete complex lexical state; revision-4 readers reject it.
inline constexpr std::uint32_t kFrogComplexStateDraftRevision = 5U;
// Opt-in symbolic references and named operations. Older readers must reject
// this revision instead of dropping targets or reinterpreting reference wires.
inline constexpr std::uint32_t kFrogApplicationControlDraftRevision = 6U;
// Configurable Connectivity nodes opt in; older readers reject instead of
// silently dropping member identities or reinterpreting their connections.
inline constexpr std::uint32_t kFrogConnectivityDraftRevision = 7U;
// Linked standard Type Definitions must never be silently detached by an
// older reader. Ordinary documents without a link retain their old revision.
inline constexpr std::uint32_t kFrogTypeDefinitionDraftRevision = 8U;
// Explicit recovery copies retain unresolved original bytes/provenance.
// Older readers must refuse instead of silently dropping that authority.
inline constexpr std::uint32_t kFrogRecoveryDraftRevision = 9U;
inline constexpr std::uint32_t kMaximumSupportedFrogDraftRevision = kFrogRecoveryDraftRevision;
inline constexpr std::string_view kFrogWidgetValueDraftFormat = "frog.widget.value.draft";

[[nodiscard]] bool validFrogStudioFileVersion(std::string_view version) noexcept;

enum class FrogDraftRevisionFieldKind : std::uint8_t {
    Missing,
    Number,
    InvalidType
};

struct FrogDraftRevisionField {
    FrogDraftRevisionFieldKind kind = FrogDraftRevisionFieldKind::Missing;
    double number = 0.0;
};

struct FrogDraftRevisionResolution {
    std::uint32_t sourceRevision = kLegacyFrogDraftRevision;
    bool requiresMigration = true;
};

[[nodiscard]] std::optional<FrogDraftRevisionResolution> resolveFrogDraftRevision(
    FrogDraftRevisionField field,
    std::string *error = nullptr);

} // namespace frog::native_engine
