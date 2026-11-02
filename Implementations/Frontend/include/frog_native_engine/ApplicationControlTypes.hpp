#pragma once
#include "BindingType.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace frog::native_engine {
enum class ApplicationControlKind { Reference, Property, Invoke, Close };
enum class ApplicationControlTargetKind { Unbound, ThisFrog, ThisApplication, FrontPanel, Widget };
enum class ApplicationControlPropertyMode { Read, Write };
struct ApplicationControlTarget {
    ApplicationControlTargetKind kind = ApplicationControlTargetKind::Unbound;
    // Empty owner means the containing document. Explicit external owners are
    // retained, never resolved against whichever document is currently active.
    std::string ownerDocumentId;
    std::string widgetId;
    bool operator==(const ApplicationControlTarget &) const = default;
};
struct ApplicationControlPropertyRow {
    std::string id;
    std::string memberId;
    ApplicationControlPropertyMode mode = ApplicationControlPropertyMode::Read;
    bool operator==(const ApplicationControlPropertyRow &) const = default;
};
struct ApplicationControlParameterBinding {
    std::string id;
    bool enabled = true;
    bool operator==(const ApplicationControlParameterBinding &) const = default;
};
struct ApplicationControlNode {
    ApplicationControlKind kind = ApplicationControlKind::Reference;
    std::string expectedClass = "frog.program";
    // Unknown is a generic reference; otherwise this is the complete schema.
    binding::TypeRef valueType;
    ApplicationControlTarget target{ApplicationControlTargetKind::ThisFrog, {}, {}};
    std::vector<ApplicationControlPropertyRow> properties;
    std::string methodId;
    std::vector<ApplicationControlParameterBinding> parameters;
    bool operator==(const ApplicationControlNode &) const = default;
};

// Neutral member/parameter/port descriptions. No widget or host authority.
enum class ApplicationControlMemberKind { Property, Method };
enum class ApplicationControlParameterDirection { In, Out, InOut };
struct ApplicationControlParameter {
    std::string id, label;
    binding::TypeRef type;
    ApplicationControlParameterDirection direction = ApplicationControlParameterDirection::In;
    bool required = true;
    std::optional<std::string> defaultValue;
};
struct ApplicationControlMemberDescriptor {
    std::string id, label, group, help;
    ApplicationControlMemberKind kind = ApplicationControlMemberKind::Property;
    binding::TypeRef type;
    bool readable = false, writable = false;
    bool authoringAvailable = false, runtimeAvailable = false;
    std::string unavailableReason;
    std::vector<ApplicationControlParameter> parameters;
    std::optional<binding::TypeRef> resultType;
    // One catalogue supplies both UI hierarchy and typed operation paths.
    // IDs remain persisted identities; neither labels nor dotted IDs are parsed.
    std::string declaringClass;
    std::vector<std::string> menuPath;
    std::uint32_t menuSection = 0, menuOrder = 0;
    std::vector<std::string> memberPath;
    std::vector<std::string> migrationAliases;
};
enum class ApplicationControlPortDirection { Input, Output };
struct ApplicationControlPort {
    std::string id, label;
    std::optional<std::size_t> rowIndex;
    ApplicationControlPortDirection direction = ApplicationControlPortDirection::Input;
    binding::TypeRef type;
    bool required = false;
    std::optional<std::string> defaultValue;
};
} // namespace frog::native_engine
