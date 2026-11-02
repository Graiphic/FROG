#pragma once
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {
// Private local realization contract, independent of the editor and LLVM.
inline constexpr std::string_view canonicalCollectionProfile="frog.studio.collection@1";
inline constexpr std::string_view canonicalCollectionOrder="frog.studio.collection.order@1";
struct CanonicalCollectionPort {
    std::string id;
    binding::TypeRef type;
    bool input=false,required=true;
};
struct CanonicalCollectionDescriptor {
    std::string operation;
    std::vector<std::string> rows;
    // Ordered as the declared operation, including logical Build row order.
    std::vector<CanonicalCollectionPort> ports;
};
[[nodiscard]] std::string_view canonicalCollectionOperationId(std::string_view authored) noexcept;
[[nodiscard]] source::JsonValue canonicalCollectionConfiguration(std::span<const std::string> rows={});
[[nodiscard]] std::optional<CanonicalCollectionDescriptor> canonicalCollectionDescribe(
    std::string_view operation,const source::JsonValue &configuration,
    std::span<const CanonicalCollectionPort> actualPorts,std::string *error=nullptr);
}
