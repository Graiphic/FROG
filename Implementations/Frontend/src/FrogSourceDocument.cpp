#include "frog_native_engine/FrogSourceDocument.hpp"
#include "FrogSourceEnvelope.generated.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <unordered_set>
#include <utility>

namespace frog::native_engine {
namespace {

using source::JsonValue;

constexpr std::string_view kInspectionLimitMessage =
    "Further inspection diagnostics omitted because their count or byte budget was reached; source and views remain complete.";
static_assert(kMaximumFrogSourceDiagnostics > 0U);
static_assert(kMaximumFrogSourceDiagnosticBytes > kInspectionLimitMessage.size());

bool isDecimalTenth(std::string_view text) noexcept {
    if (text.empty() || text.front() == '-') return false;
    const auto exponentPosition = text.find_first_of("eE");
    const auto mantissa = text.substr(0, exponentPosition);
    std::int64_t exponent = 0;
    if (exponentPosition != std::string_view::npos) {
        auto exponentText = text.substr(exponentPosition + 1);
        if (exponentText.empty()) return false;
        const bool negative = exponentText.front() == '-';
        if (negative || exponentText.front() == '+') exponentText.remove_prefix(1);
        if (exponentText.empty()) return false;
        const auto first = exponentText.find_first_not_of('0');
        if (first == std::string_view::npos) exponentText = "0";
        else exponentText.remove_prefix(first);
        const auto parsed = std::from_chars(
            exponentText.data(), exponentText.data() + exponentText.size(), exponent);
        if (parsed.ec != std::errc{} || parsed.ptr != exponentText.data() + exponentText.size()) return false;
        if (negative) exponent = -exponent;
    }

    std::int64_t digits = 0;
    std::int64_t integerDigits = 0;
    std::int64_t significantPosition = -1;
    bool decimal = false;
    bool fractionDigit = false;
    for (const char ch : mantissa) {
        if (ch == '.') {
            if (decimal || digits == 0) return false;
            decimal = true;
            continue;
        }
        if (ch < '0' || ch > '9') return false;
        if (!decimal) {
            if (integerDigits > 0 && mantissa.front() == '0') return false;
            ++integerDigits;
        } else fractionDigit = true;
        if (ch != '0') {
            if (ch != '1' || significantPosition != -1) return false;
            significantPosition = digits;
        }
        ++digits;
    }
    if (significantPosition == -1 || integerDigits == 0 || (decimal && !fractionDigit)) return false;
    // Rearrangement avoids adding an arbitrary source exponent to a size.
    return exponent == significantPosition - integerDigits;
}

std::string pointerChild(std::string_view parent, std::string_view key) {
    std::string path(parent);
    path.push_back('/');
    for (const char ch : key) {
        if (ch == '~') path += "~0";
        else if (ch == '/') path += "~1";
        else path.push_back(ch);
    }
    return path;
}

bool isNonemptyString(const JsonValue *value) noexcept {
    return value && value->type == JsonValue::Type::String && !value->string.empty();
}

} // namespace

struct FrogSourceDocument::Storage {
    std::shared_ptr<const std::string> original;
    JsonValue tree;
    FrogSourceEnvelopeStatus status = FrogSourceEnvelopeStatus::Invalid;
    std::vector<FrogSourceDiagnostic> diagnostics;
    std::vector<FrogSourcePortView> ports;
    std::vector<FrogSourceScopeView> scopes;
    std::size_t diagnosticBytes = 0;
    bool diagnosticLimitReached = false;

    void reportInspectionLimit() {
        if (diagnosticLimitReached) return;
        // Normal entries always reserve room for this one complete marker.
        diagnostics.push_back({ FrogSourceDiagnosticCode::InspectionLimit, {},
                                std::string(kInspectionLimitMessage) });
        diagnosticBytes += kInspectionLimitMessage.size();
        diagnosticLimitReached = true;
    }

    std::optional<std::size_t> diagnosticPathBudget(std::size_t messageBytes) {
        if (diagnosticLimitReached) return std::nullopt;
        constexpr auto normalByteBudget = kMaximumFrogSourceDiagnosticBytes - kInspectionLimitMessage.size();
        if (diagnostics.size() >= kMaximumFrogSourceDiagnostics - 1U ||
            messageBytes > normalByteBudget - diagnosticBytes) {
            reportInspectionLimit();
            return std::nullopt;
        }
        return normalByteBudget - diagnosticBytes - messageBytes;
    }

    void report(FrogSourceDiagnosticCode code, std::string_view path, std::string_view message) {
        const auto pathBudget = diagnosticPathBudget(message.size());
        if (!pathBudget) return;
        if (path.size() > *pathBudget) {
            reportInspectionLimit();
            return;
        }
        diagnostics.push_back({ code, std::string(path), std::string(message) });
        diagnosticBytes += path.size() + message.size();
    }

    void reportChild(FrogSourceDiagnosticCode code, std::string_view parent,
                     std::string_view key, std::string_view message) {
        const auto pathBudget = diagnosticPathBudget(message.size());
        if (!pathBudget) return;
        if (parent.size() >= *pathBudget) {
            reportInspectionLimit();
            return;
        }
        std::size_t pathBytes = parent.size() + 1U;
        // Arbitrary source keys can be megabytes long, and escaping '/' or '~'
        // expands them. Check before allocating a JSON Pointer, never truncate it.
        for (const char ch : key) {
            const std::size_t encodedBytes = ch == '/' || ch == '~' ? 2U : 1U;
            if (encodedBytes > *pathBudget - pathBytes) {
                reportInspectionLimit();
                return;
            }
            pathBytes += encodedBytes;
        }
        report(code, pointerChild(parent, key), message);
    }

    void inspectInterface(const JsonValue &interface) {
        std::unordered_set<std::string_view> ids;
        for (const auto &[field, direction] : {
                 std::pair{"inputs", FrogSourcePortDirection::Input},
                 std::pair{"outputs", FrogSourcePortDirection::Output}}) {
            const auto *values = interface.member(field);
            if (!values) continue;
            const std::string path = pointerChild("/interface", field);
            if (values->type != JsonValue::Type::Array) {
                report(FrogSourceDiagnosticCode::InvalidInterface, path, "Interface ports must be an array.");
                continue;
            }
            for (std::size_t index = 0; index < values->array.size(); ++index) {
                const auto &port = values->array[index];
                const auto itemPath = pointerChild(path, std::to_string(index));
                // Even malformed declarations remain visible and preserved.
                ports.push_back({ direction, &port, itemPath });
                const auto *id = port.member("id");
                if (!isNonemptyString(id)) {
                    report(FrogSourceDiagnosticCode::InvalidInterface, itemPath + "/id",
                           "Interface port ID must be a nonempty string.");
                } else if (!ids.insert(id->string).second) {
                    report(FrogSourceDiagnosticCode::DuplicateIdentity, itemPath + "/id",
                           "Interface port IDs must be unique across inputs and outputs.");
                }
                const auto *type = port.member("type");
                if (!isNonemptyString(type)) {
                    report(FrogSourceDiagnosticCode::InvalidInterface, itemPath + "/type",
                           "The v0.1 interface profile expects a nonempty type expression string; the original value is retained.");
                }
            }
        }
    }

    const JsonValue *inspectIdentities(const JsonValue &owner, std::string_view field,
                                       const std::string &ownerPath) {
        const auto *values = owner.member(field);
        if (!values) return nullptr;
        const auto path = pointerChild(ownerPath, field);
        if (values->type != JsonValue::Type::Array) {
            report(FrogSourceDiagnosticCode::InvalidDiagram, path, "Expected an array in the diagram profile.");
            return nullptr;
        }
        std::unordered_set<std::string_view> ids;
        for (std::size_t index = 0; index < values->array.size(); ++index) {
            const auto *id = values->array[index].member("id");
            const auto idPath = pointerChild(path, std::to_string(index)) + "/id";
            if (!isNonemptyString(id)) {
                report(FrogSourceDiagnosticCode::InvalidDiagram, idPath, "Identity must be a nonempty string.");
            } else if (!ids.insert(id->string).second) {
                report(FrogSourceDiagnosticCode::DuplicateIdentity, idPath,
                       "Duplicate identity in this scope; every declaration has been retained.");
            }
        }
        return values;
    }

    void inspectScope(const JsonValue &diagram, const std::string &path) {
        if (diagram.type != JsonValue::Type::Object) {
            report(FrogSourceDiagnosticCode::InvalidDiagram, path, "Diagram scope must be an object.");
            return;
        }
        scopes.push_back({ &diagram, path });
        inspectIdentities(diagram, "edges", path);
        const auto *nodes = inspectIdentities(diagram, "nodes", path);
        if (!nodes) return;
        for (std::size_t index = 0; index < nodes->array.size(); ++index) {
            const auto &node = nodes->array[index];
            const auto nodePath = path + "/nodes/" + std::to_string(index);
            const auto *kind = node.member("kind");
            if (!isNonemptyString(kind)) continue;
            // Public canonical structures keep region IDs and nesting. Never
            // flatten their scopes or infer graphs from opaque extension names.
            if (kind->string == "structure") {
                if (const auto *regions = inspectIdentities(node, "regions", nodePath)) {
                    for (std::size_t regionIndex = 0; regionIndex < regions->array.size(); ++regionIndex) {
                        const auto &region = regions->array[regionIndex];
                        if (const auto *body = region.member("diagram")) {
                            inspectScope(*body, nodePath + "/regions/" + std::to_string(regionIndex) + "/diagram");
                        }
                    }
                }
            } else if (kind->string == "for_loop") {
                // Only the published reference For corridor declares this
                // singular spelling (Examples 05 and 41).
                if (const auto *region = node.member("region")) {
                    if (const auto *body = region->member("diagram"))
                        inspectScope(*body, nodePath + "/region/diagram");
                    else inspectScope(*region, nodePath + "/region");
                }
            }
        }
    }

    void inspect() {
        if (tree.member("format")) {
            report(FrogSourceDiagnosticCode::InvalidEnvelope, "/format",
                   "Private draft and public source are distinct authorities; this is not a recognized public envelope.");
            return;
        }
        const auto *version = tree.member("spec_version");
        if (!version || (version->type != JsonValue::Type::String && version->type != JsonValue::Type::Number)) {
            report(FrogSourceDiagnosticCode::InvalidEnvelope, "/spec_version", "Source version must be a string or number.");
            return;
        }
        if (!isCurrentPublicFrogSourceVersion(*version)) {
            status = FrogSourceEnvelopeStatus::UnsupportedVersion;
            report(FrogSourceDiagnosticCode::UnsupportedVersion, "/spec_version",
                   "Unsupported source version; no current-version envelope or graph rules have been applied.");
            return;
        }
        status = FrogSourceEnvelopeStatus::Recognized;
        for (const auto name : source::envelope::requiredSections) {
            if (!tree.member(name)) {
                status = FrogSourceEnvelopeStatus::Invalid;
                reportChild(FrogSourceDiagnosticCode::InvalidEnvelope, "", name, "Required source section is missing.");
            }
        }
        for (const auto &[name, value] : tree.object) {
            if (std::find(source::envelope::knownSections.begin(), source::envelope::knownSections.end(), name) ==
                source::envelope::knownSections.end()) {
                status = FrogSourceEnvelopeStatus::Invalid;
                reportChild(FrogSourceDiagnosticCode::InvalidEnvelope, "", name,
                       "Unknown top-level section in source v0.1; retained without claiming support.");
            } else if (name != "spec_version" && value.type != JsonValue::Type::Object) {
                status = FrogSourceEnvelopeStatus::Invalid;
                reportChild(FrogSourceDiagnosticCode::InvalidEnvelope, "", name, "Source section must be an object.");
            }
        }
        if (const auto *interface = tree.member("interface"); interface && interface->type == JsonValue::Type::Object)
            inspectInterface(*interface);
        if (const auto *diagram = tree.member("diagram")) inspectScope(*diagram, "/diagram");
    }
};

bool isCurrentPublicFrogSourceVersion(const JsonValue &version) noexcept {
    return (version.type == JsonValue::Type::String && version.string == "0.1") ||
           (version.type == JsonValue::Type::Number && isDecimalTenth(version.string));
}

FrogSourceDocument::FrogSourceDocument(std::shared_ptr<const Storage> storage)
    : storage_(std::move(storage)) {}

std::optional<FrogSourceDocument> FrogSourceDocument::read(std::string_view text, std::string *error) {
    return readOwned(std::make_shared<const std::string>(text), error);
}

std::optional<FrogSourceDocument> FrogSourceDocument::readOwned(
    std::shared_ptr<const std::string> text, std::string *error) {
    if (!text) {
        if (error) *error = "FROG source bytes require an immutable owner.";
        return std::nullopt;
    }
    auto root = source::parseJson(*text, error, source::JsonNumberPolicy::PreserveLexeme);
    if (!root) return std::nullopt;
    if (root->type != JsonValue::Type::Object) {
        if (error) *error = "FROG source root must be a JSON object.";
        return std::nullopt;
    }
    auto storage = std::make_shared<Storage>();
    storage->original = std::move(text);
    storage->tree = std::move(*root);
    storage->inspect();
    return FrogSourceDocument(std::move(storage));
}

const JsonValue &FrogSourceDocument::root() const noexcept { return storage_->tree; }
const JsonValue *FrogSourceDocument::section(std::string_view name) const noexcept { return storage_->tree.member(name); }
std::string_view FrogSourceDocument::originalUtf8() const noexcept { return *storage_->original; }
FrogSourceEnvelopeStatus FrogSourceDocument::envelopeStatus() const noexcept { return storage_->status; }
std::span<const FrogSourceDiagnostic> FrogSourceDocument::diagnostics() const noexcept { return storage_->diagnostics; }
std::span<const FrogSourcePortView> FrogSourceDocument::publicPorts() const noexcept { return storage_->ports; }
std::span<const FrogSourceScopeView> FrogSourceDocument::diagramScopes() const noexcept { return storage_->scopes; }

} // namespace frog::native_engine
