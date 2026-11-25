#pragma once

#include "frog_native_engine/ApplicationControlSchema.hpp"
#include <unordered_map>

namespace frog::native_engine {

// Query-local descriptors for one immutable class, type and presentation.
// The strings indexed below belong to the lookup and outlive all row queries.
class ApplicationControlMemberLookup {
public:
    ApplicationControlMemberLookup(std::string_view classId, const binding::TypeRef &valueType,
        const WidgetInstance *presentation)
        : classId_(classId), valueType_(valueType), presentation_(presentation) {}
    ApplicationControlMemberLookup(const ApplicationControlMemberLookup &) = delete;
    ApplicationControlMemberLookup &operator=(const ApplicationControlMemberLookup &) = delete;

    [[nodiscard]] const ApplicationControlMemberDescriptor *find(std::string_view memberId);

private:
    std::string_view classId_;
    const binding::TypeRef &valueType_;
    const WidgetInstance *presentation_;
    std::optional<std::vector<ApplicationControlMemberDescriptor>> catalogue_;
    std::unordered_map<std::string_view, const ApplicationControlMemberDescriptor *> byId_;
    std::unordered_map<std::string_view, std::optional<ApplicationControlMemberDescriptor>> rootMembers_;
};

}
