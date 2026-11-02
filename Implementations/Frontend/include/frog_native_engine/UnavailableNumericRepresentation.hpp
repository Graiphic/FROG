#pragma once
#include <string_view>
namespace frog::native_engine {
// Identity only: these historical spellings never imply a CDB carrier,
// conversion, precision, fixed-point parameters or executable capability.
inline std::string_view unavailableComplexNumericIdentity(std::string_view name) noexcept {
    const auto equal=[&](std::string_view expected){if(name.size()!=expected.size())return false;
        for(std::size_t i=0;i<name.size();++i){const auto c=name[i];if((c>='A'&&c<='Z'?char(c-'A'+'a'):c)!=expected[i])return false;}return true;};
    if(equal("c80")||equal("cxt")||equal("complex_extended"))return "c80";
    if(equal("cfx"))return "cfx";
    return {};
}
}
