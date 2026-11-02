#pragma once

#include "frog_native_engine/FrogJson.hpp"

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {

// Inspection output is bounded separately from source preservation. These
// limits include the single InspectionLimit marker reserved for truncation.
inline constexpr std::size_t kMaximumFrogSourceDiagnostics = 256U;
inline constexpr std::size_t kMaximumFrogSourceDiagnosticBytes = 64U * 1024U;

// Transactions are sequential and reparsed after each explicit edit. Bound
// both the number of edits and total parsed work, including JSON preconditions.
inline constexpr std::size_t kMaximumFrogSourceTransactionEdits = 256U;
inline constexpr std::size_t kMaximumFrogSourceTransactionParseBytes = 256U * 1024U * 1024U;

enum class FrogSourceEditKind { Set, Remove };

struct FrogSourceEdit {
    FrogSourceEditKind kind = FrogSourceEditKind::Set;
    std::string pointer; // Strict RFC 6901 JSON Pointer, not a URI fragment.
    // Required precondition: nullopt asserts absence; otherwise compare JSON
    // types/values, object members and exact number lexemes. Whitespace, object
    // member order and string escape spelling do not affect the comparison.
    std::optional<std::string> expectedJson;
    std::string valueJson; // A strict JSON value for Set; empty for Remove.
};

// Recognition concerns the source envelope only, never semantic validity,
// editor support, lowering or execution. Invalid authoring content is retained.
enum class FrogSourceEnvelopeStatus { Recognized, UnsupportedVersion, Invalid };
enum class FrogSourceDiagnosticCode {
    UnsupportedVersion,
    InvalidEnvelope,
    InvalidInterface,
    InvalidDiagram,
    DuplicateIdentity,
    InspectionLimit
};

struct FrogSourceDiagnostic {
    FrogSourceDiagnosticCode code;
    std::string path; // JSON Pointer; empty for the document-wide limit marker.
    std::string message;
};

enum class FrogSourcePortDirection { Input, Output };

// Views borrow the immutable tree. Keep a copy of the owning document while
// using a view; copying a document shares its storage without copying its tree.
struct FrogSourcePortView {
    FrogSourcePortDirection direction;
    const source::JsonValue *declaration;
    std::string path;
};

struct FrogSourceScopeView {
    const source::JsonValue *diagram;
    std::string path;
};

// Exact decimal comparison: numeric 0.1, 0.10 and 1e-1 are equivalent, whereas
// a neighbouring decimal that rounds to double(0.1) is not the same version.
[[nodiscard]] bool isCurrentPublicFrogSourceVersion(
    const source::JsonValue &version) noexcept;

// Public source authority, with no canvas, Studio state or runtime snapshot.
// A successful read establishes strict JSON readability. Always inspect the
// envelope status and diagnostics separately; neither grants execution support.
class FrogSourceDocument {
public:
    [[nodiscard]] static std::optional<FrogSourceDocument> read(
        std::string_view source,
        std::string *error = nullptr);

    // Share an already captured immutable source buffer without copying its
    // bytes. The document retains this owner independently of the caller;
    // tree/diagnostic/port/scope views keep their existing document lifetime.
    [[nodiscard]] static std::optional<FrogSourceDocument> readOwned(
        std::shared_ptr<const std::string> source,
        std::string *error = nullptr);

    [[nodiscard]] const source::JsonValue &root() const noexcept;
    [[nodiscard]] const source::JsonValue *section(std::string_view name) const noexcept;
    [[nodiscard]] std::string_view originalUtf8() const noexcept;
    [[nodiscard]] FrogSourceEnvelopeStatus envelopeStatus() const noexcept;
    [[nodiscard]] std::span<const FrogSourceDiagnostic> diagnostics() const noexcept;
    [[nodiscard]] std::span<const FrogSourcePortView> publicPorts() const noexcept;
    [[nodiscard]] std::span<const FrogSourceScopeView> diagramScopes() const noexcept;

    // Public writer, not a validator. Retains authoring diagnostics, unknown
    // content and exact bytes except the version value, emitted as "0.1".
    // A missing/unknown version or mixed public/private envelope is refused;
    // originalUtf8 remains available for preservation in every readable case.
    [[nodiscard]] std::optional<std::string> serializeCanonical(
        std::string *error = nullptr) const;

    // Atomic immutable edit batch. Root/version/legacy-envelope changes are
    // forbidden. Set replaces an existing value, adds an absent object member,
    // or appends to an array at '-'; numeric array indices only replace existing
    // elements. Parents must exist. Remove requires an existing expected value.
    // All preconditions observe the preceding edits in this batch. Failure
    // leaves this document and all retained snapshots unchanged. Successful
    // results have fresh ranges/views and a string version; no defaults, public
    // ports, panels or graph objects are inferred or repaired.
    [[nodiscard]] std::optional<FrogSourceDocument> transact(
        std::span<const FrogSourceEdit> edits,
        std::string *error = nullptr) const;

private:
    struct Storage;
    explicit FrogSourceDocument(std::shared_ptr<const Storage> storage);
    std::shared_ptr<const Storage> storage_;
};

} // namespace frog::native_engine
