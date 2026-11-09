#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/CanonicalFileNative.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <array>
#include <utility>
namespace frog::native_engine {
namespace {
constexpr std::array<std::pair<std::string_view,std::string_view>,3> entries{{
    {"file.exists","frog.studio.file.exists.windows.v1"},
    {"file.text_read","frog.studio.file.text_read.windows.v1"},
    {"file.text_write","frog.studio.file.text_write.windows.v1"}
}};
constexpr auto ids=[] {std::array<std::string_view,3> out{};for(std::size_t i=0;i<out.size();++i)out[i]=entries[i].second;return out;}();
}
std::string_view canonicalFileMigrationIdentity(std::string_view id)noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    for(const auto &[catalog,operation]:entries)if(catalog==id)return operation;return {};
}
std::span<const std::string_view> canonicalFileNativeOperationIds()noexcept{return ids;}
std::string canonicalFileNativeConfigurationJson(const CanonicalFileNativeConfiguration &c) {
    // The two actual writer flags are file_read_lines / file_convert_eol. The
    // normalized recipe below retains them plus Source-attested wiring only.
    const auto bit=[](bool v){return v?"true":"false";};
    return std::string("{\"profile\":\"frog.studio.file.windows@1\",\"version\":1,\"read_lines\":")+bit(c.readLines)+
        ",\"convert_eol\":"+bit(c.convertEol)+",\"count_connected\":"+bit(c.countConnected)+
        ",\"file_connected\":"+bit(c.fileConnected)+",\"retain_reference\":"+bit(c.retainReference)+"}";
}
std::optional<CanonicalFileNativeConfiguration> canonicalFileNativeConfigurationFromJson(std::string_view text,std::string *error)noexcept {
    const auto bad=[&](std::string message)->std::optional<CanonicalFileNativeConfiguration>{if(error)*error=std::move(message);return {};};
    try {
        const auto root=source::parseJson(text,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        if(!root||root->type!=source::JsonValue::Type::Object||root->object.size()!=7)return bad("The File recipe requires exactly its closed seven fields.");
        const auto *profile=root->member("profile"),*version=root->member("version");
        if(!profile||profile->type!=source::JsonValue::Type::String||profile->string!="frog.studio.file.windows@1"||
            !version||version->type!=source::JsonValue::Type::Number||version->string!="1")return bad("The File recipe lost its exact profile/version.");
        CanonicalFileNativeConfiguration out;
        const std::array<std::pair<std::string_view,bool*>,5> fields{{
            {"read_lines",&out.readLines},{"convert_eol",&out.convertEol},{"count_connected",&out.countConnected},
            {"file_connected",&out.fileConnected},{"retain_reference",&out.retainReference}}};
        for(const auto &[id,target]:fields){const auto *value=root->member(id);if(!value||value->type!=source::JsonValue::Type::Boolean)return bad("Every File recipe flag must retain its actual Boolean value.");*target=value->boolean;}
        return out;
    }catch(...){return bad("The exact File configuration could not be decoded.");}
}
CanonicalFileNativeConfiguration canonicalFileNativeConfiguration(const DiagramNode &node,
    bool fileConnected,bool countConnected,bool retainReference)noexcept {
    return {node.fileReadLines,node.fileConvertEol,countConnected,fileConnected,retainReference};
}
}
