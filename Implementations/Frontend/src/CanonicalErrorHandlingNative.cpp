#include "frog_native_engine/CanonicalErrorHandlingNative.hpp"
#include <array>
#include <utility>
namespace frog::native_engine {
namespace {
constexpr std::array<std::pair<std::string_view,std::string_view>,19> operations{{
    {"error.build_cluster","frog.studio.error.build_cluster.v1"},
    {"error.demultiplex","frog.studio.error.demultiplex.v1"},
    {"error.downgrade_to_warning","frog.studio.error.downgrade_to_warning.v1"},
    {"error.filter_code","frog.studio.error.filter_code.v1"},
    {"error.generate_report","frog.studio.error.generate_report.v1"},
    {"error.is_error_or_warning","frog.studio.error.is_error_or_warning.v1"},
    {"error.is_error","frog.studio.error.is_error.v1"},
    {"error.is_warning","frog.studio.error.is_warning.v1"},
    {"error.multiplex","frog.studio.error.multiplex.v1"},
    {"error.override_call_chain","frog.studio.error.override_call_chain.v1"},
    {"error.read_call_chain","frog.studio.error.read_call_chain.v1"},
    {"error.read_description","frog.studio.error.read_description.v1"},
    {"error.read_code","frog.studio.error.read_code.v1"},
    {"error.read_location","frog.studio.error.read_location.v1"},
    {"error.translate_code","frog.studio.error.translate_code.v1"},
    {"error.upgrade_to_error","frog.studio.error.upgrade_to_error.v1"},
    {"error.write_call_chain","frog.studio.error.write_call_chain.v1"},
    {"error.write_current_call_chain","frog.studio.error.write_current_call_chain.v1"},
    {"error.write_code","frog.studio.error.write_code.v1"},
}};
constexpr auto ids=[] {std::array<std::string_view,operations.size()> out{};
    for(std::size_t i=0;i<out.size();++i)out[i]=operations[i].second;return out;}();
}
std::string_view canonicalErrorHandlingMigrationIdentity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id=="error.clear"||id=="error.clear_cluster")return "frog.studio.error.clear.v1";
    if(id=="error.display")return "frog.studio.error.display.v1";
    if(id=="error.retain_first")return "frog.studio.error.retain_first.v1";
    for(const auto &[catalog,operation]:operations)if(catalog==id)return operation;return {};
}
std::span<const std::string_view> canonicalErrorHandlingOperationIds() noexcept {return ids;}
}
