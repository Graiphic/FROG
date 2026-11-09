#pragma once
#include <charconv>
#include <optional>
#include <string_view>
#include <vector>

namespace frog::native_engine {
// Parse only the authoring signature. Formatting/scanning values stays in the runtime.
struct StringFormatField { unsigned argument; char code; };
inline std::optional<std::vector<StringFormatField>> stringFormatFields(std::string_view text) {
    std::vector<StringFormatField> fields; unsigned next=0;
    for(std::size_t i=0;i<text.size();++i) {
        if(text[i]=='\\') { if(i+1<text.size()) ++i; continue; }
        if(text[i]!='%') continue;
        if(++i==text.size()) return std::nullopt;
        if(text[i]=='%') continue;
        if(text[i]==';') continue;
        if((text[i]=='.' || text[i]==',') && i+1<text.size() && text[i+1]==';') { ++i; continue; }
        unsigned argument=next; const auto start=i;
        while(i<text.size() && text[i]>='0' && text[i]<='9') ++i;
        if(i<text.size() && text[i]=='$') {
            unsigned n=0; auto p=std::from_chars(text.data()+start,text.data()+i,n);
            if(p.ec!=std::errc{} || n<1 || n>32) return std::nullopt;
            argument=n-1; ++i;
        } else i=start;
        while(i<text.size()) {
            if(text[i]=='{' || text[i]=='<') {
                const char closing=text[i]=='{'?'}':'>';auto end=i+1;
                for(;end<text.size();++end) {
                    if(text[end]=='\\'&&end+1<text.size()){++end;continue;}
                    if(text[end]==closing)break;
                }
                if(end==text.size()) return std::nullopt;
                i=end+1; continue;
            }
            if(std::string_view("-+#^0 ._123456789").find(text[i])!=std::string_view::npos) { ++i; continue; }
            break;
        }
        if(i==text.size() || argument>=32) return std::nullopt;
        const char code=text[i];
        if(code=='[') {
            auto end=text.find(']',i+1);
            if(end==std::string_view::npos) return std::nullopt;
            i=end;
        } else if(std::string_view("xobdufegpsTt").find(code)==std::string_view::npos) return std::nullopt;
        fields.push_back({argument,code}); next=argument+1;
    }
    return fields;
}
inline unsigned stringFormatArgumentCount(const std::vector<StringFormatField> &fields) {
    unsigned count=1; for(const auto &f:fields) if(f.argument+1>count) count=f.argument+1; return count;
}
}
