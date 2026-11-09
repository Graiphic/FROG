#include "frog_native_engine/CanonicalPathNative.hpp"
#include <array>
#include <utility>
namespace frog::native_engine {
namespace {
constexpr std::array<std::pair<std::string_view,std::string_view>,14> operations{{
    {"path.compose","frog.studio.path.compose.windows.v1"},
    {"path.decompose","frog.studio.path.decompose.windows.v1"},
    {"path.type","frog.studio.path.type.windows.v1"},
    {"path.get_component_directory","frog.studio.path.get_component_directory.windows.v1"},
    {"path.to_string","frog.studio.path.to_string.windows.v1"},
    {"path.from_string","frog.studio.path.from_string.windows.v1"},
    {"path.to_string_array","frog.studio.path.to_string_array.windows.v1"},
    {"path.from_string_array","frog.studio.path.from_string_array.windows.v1"},
    {"path.get_system_directory","frog.studio.path.get_system_directory.windows.v1"},
    {"path.compare","frog.studio.path.compare.windows.v1"},
    {"path.generate_temporary_file","frog.studio.path.generate_temporary_file.windows.v1"},
    {"path.common_relative","frog.studio.path.common_relative.windows.v1"},
    {"path.get_file_extension","frog.studio.path.get_file_extension.windows.v1"},
    {"path.replace_file_extension","frog.studio.path.replace_file_extension.windows.v1"},
}};
constexpr auto ids=[] {std::array<std::string_view,operations.size()> value{};
    for(std::size_t i=0;i<operations.size();++i)value[i]=operations[i].second;return value;}();
}
std::string_view canonicalPathValueMigrationIdentity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    // Existing Comparison kernels own these predicates; no duplicate algorithms.
    if(id=="path.empty_string_or_path")return "frog.studio.comparison.empty_string_or_path.owned.v1";
    if(id=="path.is_not_empty")return "frog.studio.path.is_not_empty.owned.v1";
    for(const auto &[catalog,operation]:operations)if(catalog==id)return operation;return {};
}
std::span<const std::string_view> canonicalPathValueOperationIds() noexcept {return ids;}
}
