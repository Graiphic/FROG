#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <map>

namespace frog::native_engine {

inline constexpr std::string_view kPolymorphicFrogFormat = "frog.document.polymorphic";

struct PolymorphicFrogMember {
    std::string id;
    std::string documentPath;
    std::string menuName;
    std::string selectorName;
    bool operator==(const PolymorphicFrogMember &) const = default;
};

struct PolymorphicFrog {
    std::string id;
    std::string title;
    std::string description;
    std::string documentationUri;
    std::string iconSvg;
    std::vector<PolymorphicFrogMember> members;
    bool drawInstanceIcon = false;
    bool showSelectorByDefault = false;
    bool adaptToType = true;
    bool operator==(const PolymorphicFrog &) const = default;
};

// The enclosing call's document_path identifies the family. The last automatic
// choice is used only when the family disables adaptation. Member paths and
// signatures still come from the current family and its dependency snapshot.
struct PolymorphicFrogSelection {
    std::optional<std::string> manualMemberId;
    std::optional<bool> showSelector;
    std::optional<std::string> lastAutomaticMemberId;
    bool operator==(const PolymorphicFrogSelection &) const = default;
};

struct PolymorphicFrogIssue {
    std::string code;
    std::string memberId;
    std::string slotId;
    std::string message;
};

struct PolymorphicFrogPort {
    std::string publicId;
    std::string label;
    binding::TypeRef type;
    bool input = true;
    std::string requirement;
};

// Slot IDs and the map pattern are supplied by the ordinary interface owner.
// Empty slots are absent; member-specific public IDs need not coincide.
struct PolymorphicFrogInterface {
    std::string profileId;
    std::string patternId;
    std::vector<std::string> transforms;
    std::map<std::string, PolymorphicFrogPort> slots;
};

struct PolymorphicFrogResolution {
    std::optional<std::size_t> memberIndex;
    std::vector<PolymorphicFrogIssue> issues;
};

[[nodiscard]] bool isPolymorphicFrog(std::string_view source);
[[nodiscard]] std::optional<PolymorphicFrog> readPolymorphicFrog(
    std::string_view source, std::string *error = nullptr);
[[nodiscard]] std::string writePolymorphicFrog(const PolymorphicFrog &family);
[[nodiscard]] std::vector<PolymorphicFrogIssue> validatePolymorphicFrog(
    const PolymorphicFrog &family);

// Signatures follow family order. A missing interface is invalid even when
// another member is selected. Input evidence is keyed by connector slot;
// wiredSlots also includes outputs, for detecting a missing terminal.
[[nodiscard]] PolymorphicFrogResolution resolvePolymorphicFrog(
    const PolymorphicFrog &family, const PolymorphicFrogSelection &selection,
    std::span<const std::optional<PolymorphicFrogInterface>> signatures,
    const std::map<std::string, binding::TypeRef> &inputs,
    std::span<const std::string> wiredSlots);

struct FrogDraftData;
[[nodiscard]] std::optional<PolymorphicFrogInterface> inspectPolymorphicFrogInterface(
    const FrogDraftData &member);

} // namespace frog::native_engine
