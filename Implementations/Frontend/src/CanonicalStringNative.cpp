#include "frog_native_engine/CanonicalStringNative.hpp"
#include <array>
#include <charconv>
#include <cstdint>
#include <utility>

namespace frog::native_engine {
namespace {
constexpr std::array<CanonicalStringNativeContract,2> contracts{{
    {CanonicalStringOperation::LengthBytes,"string.length","frog.studio.string.length.bytes.v1",canonicalStringBytesProfile,"i32",1,1},
    {CanonicalStringOperation::ConcatenateBytes,"string.concatenate","frog.studio.string.concatenate.bytes.v1",canonicalStringBytesProfile,"string",2,canonicalStringMaximumInputs}
}};
constexpr std::array<std::pair<std::string_view,std::string_view>,54> valueOperations{{
    {"string.length","frog.studio.string.length.tree.bytes.v1"},
    {"string.concatenate","frog.studio.string.concatenate.typed.bytes.v1"},
    {"string.subset","frog.studio.string.subset.bytes.v1"},
    {"string.replace_substring","frog.studio.string.replace_substring.bytes.v1"},
    {"string.reverse","frog.studio.string.reverse.bytes.v1"},
    {"string.rotate","frog.studio.string.rotate.bytes.v1"},
    {"string.to_upper","frog.studio.string.to_upper.ascii.v1"},
    {"string.to_lower","frog.studio.string.to_lower.ascii.v1"},
    {"string.search_split","frog.studio.string.search_split.bytes.v1"},
    {"array.conversion.string_to_byte_array","frog.studio.array.conversion.string_to_byte_array.bytes.v1"},
    {"array.conversion.byte_array_to_string","frog.studio.array.conversion.byte_array_to_string.bytes.v1"},
    {"string.conversion.format_into_string","frog.studio.string.format_into_string.typed.bytes.v1"},
    {"string.conversion.flatten_to_string","frog.studio.string.conversion.flatten_to_string.binary.v1"},
    {"string.conversion.unflatten_from_string","frog.studio.string.conversion.unflatten_from_string.binary.v1"},
    {"numeric.data_manipulation.flatten_to_byte_array","frog.studio.numeric.data_manipulation.flatten_to_byte_array.binary.v1"},
    {"numeric.data_manipulation.unflatten_from_byte_array","frog.studio.numeric.data_manipulation.unflatten_from_byte_array.binary.v1"},
    {"string.trim_whitespace","frog.studio.string.trim_whitespace.bytes.v1"},
    {"string.normalize_end_of_line","frog.studio.string.normalize_end_of_line.bytes.v1"},
    {"string.search","frog.studio.string.match_first.bytes.v1"},
    {"string.append_true_false","frog.studio.string.append_true_false.bytes.v1"},
    {"string.append_multiline","frog.studio.string.pick_line.bytes.v1"},
    {"string.lexical_class","frog.studio.string.lexical_class.tree.bytes.v1"},
    {"string.scan_tokens","frog.studio.string.scan_tokens.bytes.v1"},
    {"string.escape","frog.studio.string.escape.bytes.v1"},
    {"string.search_1d_array","frog.studio.string.search_1d_array.regex.bytes.v1"},
    {"string.match_pattern","frog.studio.string.match_pattern.bytes.v1"},
    {"string.match_regular_expression","frog.studio.string.match_regular_expression.bytes.v1"},
    {"string.search_replace_pattern","frog.studio.string.search_replace_pattern.bytes.v1"},
    {"string.search_replace_regular_expression","frog.studio.string.search_replace_regular_expression.bytes.v1"},
    {"string.conversion.string_array_to_delimited_string","frog.studio.string.array_to_delimited.bytes.v1"},
    {"string.conversion.delimited_string_to_string_array","frog.studio.string.delimited_to_array.bytes.v1"},
    {"string.conversion.number_to_decimal_string","frog.studio.string.conversion.number_to_decimal_string.typed.bytes.v1"},
    {"string.conversion.number_to_hex_string","frog.studio.string.conversion.number_to_hex_string.typed.bytes.v1"},
    {"string.conversion.number_to_octal_string","frog.studio.string.conversion.number_to_octal_string.typed.bytes.v1"},
    {"string.conversion.number_to_fractional_string","frog.studio.string.conversion.number_to_fractional_string.typed.bytes.v1"},
    {"string.conversion.number_to_exponential_string","frog.studio.string.conversion.number_to_exponential_string.typed.bytes.v1"},
    {"string.conversion.number_to_engineering_string","frog.studio.string.conversion.number_to_engineering_string.typed.bytes.v1"},
    {"string.conversion.decimal_string_to_number","frog.studio.string.conversion.decimal_string_to_number.typed.bytes.v1"},
    {"string.conversion.hex_string_to_number","frog.studio.string.conversion.hex_string_to_number.typed.bytes.v1"},
    {"string.conversion.octal_string_to_number","frog.studio.string.conversion.octal_string_to_number.typed.bytes.v1"},
    {"string.conversion.floating_string_to_number","frog.studio.string.conversion.floating_string_to_number.typed.bytes.v1"},
    {"string.conversion.scan_from_string","frog.studio.string.conversion.scan_from_string.typed.bytes.v1"},
    {"string.conversion.scan_value","frog.studio.string.conversion.scan_value.typed.bytes.v1"},
    {"string.conversion.format_value","frog.studio.string.conversion.format_value.typed.bytes.v1"},
    {"string.conversion.array_to_spreadsheet_string","frog.studio.string.conversion.array_to_spreadsheet_string.typed.bytes.v1"},
    {"string.conversion.spreadsheet_string_to_array","frog.studio.string.conversion.spreadsheet_string_to_array.typed.bytes.v1"},
    {"string.conversion.path_to_string","frog.studio.string.conversion.path_to_string.data.v1"},
    {"string.conversion.string_to_path","frog.studio.string.conversion.string_to_path.data.v1"},
    {"string.conversion.string_array_to_path","frog.studio.string.conversion.string_array_to_path.data.v1"},
    {"string.conversion.path_to_string_array","frog.studio.string.conversion.path_to_string_array.data.v1"},
    {"string.conversion.relative_path_to_portable_string","frog.studio.string.conversion.relative_path_to_portable_string.data.v1"},
    {"string.conversion.flatten_to_json","frog.studio.string.conversion.flatten_to_json.data.v1"},
    {"string.conversion.unflatten_from_json","frog.studio.string.conversion.unflatten_from_json.data.v1"},
    {"string.empty_string_or_path","frog.studio.comparison.empty_string_or_path.owned.v1"}
}};
constexpr auto valueOperationIds=[] {
    std::array<std::string_view,valueOperations.size()+1> ids{};
    for(std::size_t i=0;i<valueOperations.size();++i)ids[i]=valueOperations[i].second;
    ids.back()="frog.studio.string.search_split.single_character.bytes.v1";return ids;
}();
}
std::span<const CanonicalStringNativeContract> canonicalStringNativeContracts() noexcept {return contracts;}
const CanonicalStringNativeContract *canonicalStringNativeContract(std::string_view id) noexcept {
    for(const auto &contract:contracts)if(contract.operationId==id)return &contract;return nullptr;
}
std::string_view canonicalStringMigrationIdentity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    for(const auto &contract:contracts)if(contract.catalogId==id)return contract.operationId;return {};
}
std::string_view canonicalStringValueMigrationIdentity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    // Two palette identities, one existing raw-byte operation. The composed
    // port adapter preserves the authored value/result endpoint identities.
    if(id=="numeric.conversion.string_to_byte_array")id="array.conversion.string_to_byte_array";
    else if(id=="numeric.conversion.byte_array_to_string")id="array.conversion.byte_array_to_string";
    for(const auto &[catalog,operation]:valueOperations)if(catalog==id)return operation;return {};
}
std::span<const std::string_view> canonicalStringValueOperationIds() noexcept {return valueOperationIds;}
std::optional<std::size_t> canonicalStringConcatPortOrdinal(std::string_view port) noexcept {
    if(port=="a")return 0;if(port=="b")return 1;
    if(!port.starts_with("input_")||port.size()>8)return {};
    port.remove_prefix(6);if(port.empty()||port.front()=='0')return {};
    std::size_t number=0;const auto parsed=std::from_chars(port.data(),port.data()+port.size(),number);
    if(parsed.ec!=std::errc{}||parsed.ptr!=port.data()+port.size()||number<3||number>canonicalStringMaximumInputs)return {};
    return number-1;
}
bool canonicalStringInputPortsValid(const CanonicalStringNativeContract &contract,std::span<const std::string> ports) noexcept {
    if(ports.size()<contract.minimumInputs||ports.size()>contract.maximumInputs)return false;
    if(contract.operation==CanonicalStringOperation::LengthBytes)return ports.size()==1&&ports[0]=="value";
    if(ports[0]!="a"||ports[1]!="b")return false;
    std::size_t previous=1;
    for(std::size_t i=2;i<ports.size();++i){const auto ordinal=canonicalStringConcatPortOrdinal(ports[i]);if(!ordinal||*ordinal<=previous)return false;previous=*ordinal;}
    return true;
}
bool canonicalStringUtf8Valid(std::string_view text) noexcept {
    for(std::size_t at=0;at<text.size();) {
        const auto first=static_cast<unsigned char>(text[at++]);if(first<0x80)continue;
        unsigned count=0;std::uint32_t code=0,minimum=0;
        if(first>=0xC2&&first<=0xDF){count=1;code=first&0x1F;minimum=0x80;}
        else if(first>=0xE0&&first<=0xEF){count=2;code=first&0x0F;minimum=0x800;}
        else if(first>=0xF0&&first<=0xF4){count=3;code=first&7;minimum=0x10000;}
        else return false;
        if(count>text.size()-at)return false;
        for(unsigned i=0;i<count;++i){const auto next=static_cast<unsigned char>(text[at++]);if((next&0xC0)!=0x80)return false;code=(code<<6)|(next&0x3F);}
        if(code<minimum||code>0x10FFFF||(code>=0xD800&&code<=0xDFFF))return false;
    }
    return true;
}
}
