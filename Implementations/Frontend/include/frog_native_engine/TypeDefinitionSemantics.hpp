#pragma once

#include "frog_native_engine/WidgetInstance.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {
inline constexpr std::string_view kTypeDefinitionExtension = ".ftype";
inline constexpr std::string_view kTypeDefinitionFormat = "frog.type_definition.draft";
struct TypeDefinitionDocument {
    std::string id;
    std::string name;
    std::uint64_t revision = 1;
    WidgetInstance root;
    std::string iconSvgText;
};
struct TypeDefinitionDiagnostic {
    std::string code;
    std::string message;
    std::string instanceId;
    std::string fieldPath;
    std::string oldValue;
    std::string newType;
    std::string proposedValue;
};
struct TypeDefinitionMigration {
    WidgetInstance proposedWidget;
    std::vector<TypeDefinitionDiagnostic> diagnostics;
    bool changed = false;
    bool accepted() const noexcept { return diagnostics.empty(); }
};

[[nodiscard]] bool typeDefinitionSupported(const WidgetInstance &, std::string *error = nullptr);
[[nodiscard]] bool typeDefinitionSchemaEquivalent(const WidgetInstance &, const WidgetInstance &);
// Explicit first association only: corresponding local fields may adopt the
// common identities after the complete ordered schema has been checked.
[[nodiscard]] bool typeDefinitionCanAssociate(const WidgetInstance &, const WidgetInstance &);
[[nodiscard]] std::string typeDefinitionSchemaFingerprint(const WidgetInstance &);
[[nodiscard]] std::string serializeTypeDefinition(const TypeDefinitionDocument &, std::string *error = nullptr);
[[nodiscard]] std::optional<TypeDefinitionDocument> deserializeTypeDefinition(std::string_view, std::string *error = nullptr);
// Pure preparation: no publication, file write or mutation of the input.
// Diagnostics retain the old value and the proposed replacement explicitly.
[[nodiscard]] TypeDefinitionMigration migrateTypeDefinitionWidget(
    const WidgetInstance &local, const WidgetInstance &previous,
    const WidgetInstance &current, bool acceptProposed = false);
[[nodiscard]] TypeDefinitionMigration linkWidgetToTypeDefinition(
    const WidgetInstance &local, const TypeDefinitionDocument &definition,
    std::string sourcePath, bool acceptProposed = false);
}
