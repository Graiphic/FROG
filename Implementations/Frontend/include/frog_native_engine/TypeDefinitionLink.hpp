#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <optional>
#include <string_view>

namespace frog::native_engine {
// Private authoring provenance. The executable value remains its ordinary
// resolved type; a link is never a Refnum or a shared value location.
struct TypeDefinitionLink {
    std::string definitionId;
    std::string sourcePath;
    std::uint64_t resolvedRevision = 0;
    std::string resolvedSchemaFingerprint;
    // Complete previous schema, including stable field/Enum identities. This
    // recovery snapshot is never authority when the source is unavailable.
    std::string baselineWidgetJson;
    std::string unresolvedReason;
    std::string iconSvgText;
    // The definition owns each member ID. The consumer owns its corresponding
    // front-panel object ID and any wires or references targeting that object.
    std::map<std::string, std::string, std::less<>> tabChildBindings;
    bool operator==(const TypeDefinitionLink &) const = default;
};
[[nodiscard]] std::string serializeTypeDefinitionLink(const TypeDefinitionLink &);
[[nodiscard]] inline bool typeDefinitionUsesStructuredBaseline(const TypeDefinitionLink &link) noexcept {
    return link.baselineWidgetJson.size()>8U*1024U*1024U;
}
[[nodiscard]] std::optional<TypeDefinitionLink> deserializeTypeDefinitionLink(
    std::string_view, std::string *error = nullptr);
}
