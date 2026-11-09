#include "frog_native_engine/CanonicalTimestampNative.hpp"
#include <array>
#include <utility>
namespace frog::native_engine {
namespace {
constexpr std::array<std::pair<std::string_view,std::string_view>,10> operations{{
    {"timestamp.get_current_time","frog.studio.timestamp.get_current_time.v1"},
    {"timestamp.get_date_time_string","frog.studio.timestamp.get_date_time_string.v1"},
    {"timestamp.to_date_time","frog.studio.timestamp.to_date_time.v1"},
    {"timestamp.from_date_time","frog.studio.timestamp.from_date_time.v1"},
    {"timestamp.format_date_time","frog.studio.timestamp.format_date_time.v1"},
    {"timestamp.to_timestamp","frog.studio.timestamp.to_timestamp.v1"},
    {"structures.tick_count","frog.studio.structures.tick_count.v1"},
    {"structures.elapsed_time","frog.studio.structures.elapsed_time.v1"},
    {"structures.wait","frog.studio.structures.wait.v1"},
    {"structures.wait_until_next","frog.studio.structures.wait_until_next.v1"},
}};
constexpr auto ids=[] {std::array<std::string_view,operations.size()> out{};
    for(std::size_t i=0;i<out.size();++i)out[i]=operations[i].second;return out;}();
}
std::string_view canonicalTimestampMigrationIdentity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    for(const auto &[catalog,operation]:operations)if(catalog==id)return operation;return {};
}
std::span<const std::string_view> canonicalTimestampOperationIds() noexcept {return ids;}
}
