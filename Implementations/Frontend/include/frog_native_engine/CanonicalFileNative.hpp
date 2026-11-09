#pragma once
#include <optional>
#include <span>
#include <string>
#include <string_view>
namespace frog::native_engine {
struct DiagramNode;
struct CanonicalFileNativeConfiguration {
    bool readLines=false,convertEol=true,countConnected=false,fileConnected=true,retainReference=false;
    bool operator==(const CanonicalFileNativeConfiguration &)const=default;
};
[[nodiscard]] std::string_view canonicalFileMigrationIdentity(std::string_view)noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalFileNativeOperationIds()noexcept;
// Connection flags come from the actual original edge set, before defaults are
// materialized. Authoring options come from the already parsed real owner.
[[nodiscard]] CanonicalFileNativeConfiguration canonicalFileNativeConfiguration(
    const DiagramNode &,bool fileConnected,bool countConnected,bool retainReference)noexcept;
[[nodiscard]] std::string canonicalFileNativeConfigurationJson(const CanonicalFileNativeConfiguration &);
[[nodiscard]] std::optional<CanonicalFileNativeConfiguration> canonicalFileNativeConfigurationFromJson(
    std::string_view,std::string *error=nullptr)noexcept;
}
