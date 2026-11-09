#include "frog_native_engine/CanonicalArrayNative.hpp"
#include <array>
#include <charconv>
#include <stdexcept>
#include <utility>
namespace frog::native_engine {
namespace {constexpr std::array<CanonicalArrayNativeContract,3> contracts{{
    {CanonicalArrayOperation::Build,"array.build","frog.studio.array.build.real1d.v1","result",canonicalArraySourceType},
    {CanonicalArrayOperation::Index,"array.index","frog.studio.array.index.real1d.v1","element","f64"},
    {CanonicalArrayOperation::Size,"array.size","frog.studio.array.size.real1d.v1","size","i32"}
}};
constexpr std::array ownedContracts{
    std::pair<std::string_view,std::string_view>{"array.size","frog.studio.array.size.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.index","frog.studio.array.index.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.replace_subset","frog.studio.array.replace_subset.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.insert","frog.studio.array.insert.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.delete","frog.studio.array.delete.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.initialize","frog.studio.array.initialize.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.subset","frog.studio.array.subset.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.max_min","frog.studio.array.max_min.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.reshape","frog.studio.array.reshape.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.sort_1d","frog.studio.array.sort_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.reverse_1d","frog.studio.array.reverse_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.search_1d","frog.studio.array.search_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.split_1d","frog.studio.array.split_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.rotate_1d","frog.studio.array.rotate_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.interpolate_1d","frog.studio.array.interpolate_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.threshold_1d","frog.studio.array.threshold_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.interleave_1d","frog.studio.array.interleave_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.decimate_1d","frog.studio.array.decimate_1d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.transpose_2d","frog.studio.array.transpose_2d.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.size","frog.studio.array.matrix.size.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.reshape","frog.studio.array.matrix.reshape.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.transpose","frog.studio.array.matrix.transpose.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.get_diagonal","frog.studio.array.matrix.get_diagonal.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.get_elements","frog.studio.array.matrix.get_elements.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.get_subset","frog.studio.array.matrix.get_subset.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.set_diagonal","frog.studio.array.matrix.set_diagonal.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.set_elements","frog.studio.array.matrix.set_elements.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.build","frog.studio.array.matrix.build.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.matrix.set_subset","frog.studio.array.matrix.set_subset.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.conversion.spreadsheet_string_to_array","frog.studio.array.conversion.spreadsheet_string_to_array.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.conversion.array_to_spreadsheet_string","frog.studio.array.conversion.array_to_spreadsheet_string.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.conversion.array_to_cluster","frog.studio.array.conversion.array_to_cluster.owned.v1"},
    std::pair<std::string_view,std::string_view>{"array.conversion.cluster_to_array","frog.studio.array.conversion.cluster_to_array.owned.v1"}};
constexpr auto ownedIds=[] {std::array<std::string_view,ownedContracts.size()+4> ids{};for(std::size_t i=0;i<ownedContracts.size();++i)ids[i]=ownedContracts[i].second;ids[ownedContracts.size()]=canonicalArrayBuildOwnedId;ids[ownedContracts.size()+1]=canonicalArrayConcatenateOwnedId;ids[ownedContracts.size()+2]=canonicalArrayIndexExtendedId;ids[ownedContracts.size()+3]=canonicalMatrixBuildRowsId;return ids;}();
}
std::span<const CanonicalArrayNativeContract> canonicalArrayNativeContracts()noexcept{return contracts;}
const CanonicalArrayNativeContract *canonicalArrayNativeContract(std::string_view id)noexcept{for(const auto &v:contracts)if(v.operationId==id)return &v;return nullptr;}
std::string_view canonicalArrayMigrationIdentity(std::string_view id)noexcept{if(id.starts_with("frog.studio."))id.remove_prefix(12);for(const auto &v:contracts)if(v.catalogId==id)return v.operationId;return {};}
std::string_view canonicalArrayValueMigrationIdentity(std::string_view id)noexcept{if(id.starts_with("frog.studio."))id.remove_prefix(12);if(id=="cluster.conversion.array_to_cluster")id="array.conversion.array_to_cluster";if(id=="cluster.conversion.cluster_to_array")id="array.conversion.cluster_to_array";for(const auto &[catalog,operation]:ownedContracts)if(catalog==id)return operation;return {};}
std::span<const std::string_view> canonicalArrayValueOperationIds()noexcept{return ownedIds;}
std::string canonicalArrayBuildPort(std::size_t ordinal){if(ordinal>=canonicalArrayMaximumBuildInputs)throw std::out_of_range("Build has at most32 inputs");return ordinal==0?"array_a":ordinal==1?"array_b":"input_"+std::to_string(ordinal+1);}
std::optional<std::size_t> canonicalArrayBuildPortOrdinal(std::string_view port)noexcept{if(port=="array_a")return 0;if(port=="array_b")return 1;if(port.size()>8||!port.starts_with("input_"))return {};port.remove_prefix(6);if(port.empty()||port.front()=='0')return {};std::size_t n=0;const auto p=std::from_chars(port.data(),port.data()+port.size(),n);if(p.ec!=std::errc{}||p.ptr!=port.data()+port.size()||n<3||n>canonicalArrayMaximumBuildInputs)return {};return n-1;}
bool canonicalArrayBuildPortsValid(std::span<const std::string> ports)noexcept{if(ports.empty()||ports.size()>canonicalArrayMaximumBuildInputs)return false;for(std::size_t i=0;i<ports.size();++i)if(canonicalArrayBuildPortOrdinal(ports[i])!=std::optional<std::size_t>{i})return false;return true;}
}
