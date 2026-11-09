#include <frog/bridge_json.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {
namespace j=frog::bridge::json;
unsigned checks=0;
void need(bool value,std::string_view why){++checks;if(!value)throw std::runtime_error(std::string(why));}
void numbers() {
    frog::bridge::Error error;
    for(const auto *lexeme:{"9007199254740993","-9007199254740993","18446744073709551615","-0","1e400","1.2300e-12"}) {
        const auto value=j::parse(lexeme,error);
        need(value&&value->kind()==j::Kind::Number&&value->numberLexeme()==lexeme,"Exact JSON numeric lexeme was coerced or rounded");
        need(j::encode(*value)==lexeme,"Encoding changed an authoritative numeric lexeme");
    }
}
void lifetimes() {
    frog::bridge::Error error;
    auto root=j::parse("{\"null\":null,\"child\":{\"value\":\"A\\u0000B\"},\"array\":[true,18446744073709551615]}",error);
    need(root&&root->kind()==j::Kind::Object,"The actual object did not parse");
    auto null=root->member("null"),child=root->member("child"),array=root->member("array");
    need(null&&null->kind()==j::Kind::Null&&!root->member("absent"),"Absent member was conflated with an explicit JSON null");
    need(child&&array,"Nested handles are missing");auto entries=array->elements();auto members=child->members();
    root.reset();child.reset();array.reset();
    need(entries.size()==2&&entries[0].boolean()&&entries[1].numberLexeme()=="18446744073709551615","Child array handles borrowed a destroyed tree");
    need(members.size()==1&&members[0].first=="value"&&members[0].second.string()==std::string_view("A\0B",3),"Object handles or embedded NUL did not retain exact owned bytes");
    const auto view=members[0].second.string();auto retained=members[0].second;members.clear();
    need(view==std::string_view("A\0B",3)&&retained.string()==view,"A surviving value handle did not retain its shared immutable tree");
    bool refused=false;try{(void)entries[1].string();}catch(const std::logic_error &){refused=true;}need(refused,"A wrong-kind accessor fabricated a string");
    refused=false;try{(void)null->member("absent");}catch(const std::logic_error &){refused=true;}need(refused,"A null was silently interpreted as an object");
}
void strictAndBounded() {
    frog::bridge::Error error;
    for(const auto *bad:{"{\"x\":1,\"x\":2}","{\"x\":1,\"\\u0078\":2}","\"\\ud800\"","[1,]","NaN",""}) {
        need(!j::parse(bad,error)&&!error.code.empty()&&!error.message.empty(),"The strict shared reader admitted malformed or ambiguous JSON");
    }
    const std::string invalid="\""+std::string("\xc0\xaf",2)+"\"";
    need(!j::parse(invalid,error),"Malformed UTF-8 bypassed the shared strict parser");
    const auto deep=std::string(130,'[')+"null"+std::string(130,']');
    need(!j::parse(deep,error),"The shared JSON nesting budget was bypassed");
    need(!j::parse("null",error,3)&&error.code=="bridge.json.byte_limit","The explicit caller byte ceiling was ignored");
    need(!j::parse("null",error,0),"Zero-byte admission silently raised its ceiling");
    need(j::parse("null",error,4).has_value()&&error.code.empty(),"The exact byte boundary or successful error reset changed");
    const std::string oversized(16U*1024U*1024U+1U,' ');
    need(!j::parse(oversized,error,oversized.size())&&error.code=="bridge.json.byte_limit","A caller raised the SDK hard byte ceiling");
}
void encodingAndHashes() {
    frog::bridge::Error error;
    const auto value=j::parse(" { \"b\": [1.00e2, -0], \"a\": true } ",error);
    need(value&&j::encode(*value)=="{\"a\":true,\"b\":[1.00e2,-0]}","Encoding no longer uses the shared deterministic encoder");
    need(value->originalUtf8()=="{ \"b\": [1.00e2, -0], \"a\": true }","Original object token lost authoritative order or whitespace");
    auto spanRoot=j::parse(std::string(" {\"z\":1,\"context\": { \"x\": [1.00, -0] }} "),error);auto span=spanRoot->member("context");auto child=span->member("x");spanRoot.reset();span.reset();
    need(child->originalUtf8()=="[1.00, -0]"&&child->elements()[0].originalUtf8()=="1.00","Child token span borrowed source storage or was re-encoded");
    const auto quoted=j::quote(std::string_view("A\0B\n\"\\",6));const auto decoded=j::parse(quoted,error);
    need(decoded&&decoded->string()==std::string_view("A\0B\n\"\\",6),"Quote did not preserve escaped string bytes exactly");
    bool refused=false;try{(void)j::quote(std::string_view("\xff",1));}catch(const std::invalid_argument &){refused=true;}need(refused,"Quote produced malformed UTF-8 JSON");
    need(j::sha256("")=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855","Empty SHA256 differs from the standard digest");
    need(j::sha256("abc")=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad","SHA256 is not the exact shared owner digest");
    need(j::sha256(std::string_view("a\0b",3))!=j::sha256("a"),"SHA256 stopped at embedded NUL");
}
}
int main() {
    try{numbers();lifetimes();strictAndBounded();encodingAndHashes();std::cout<<"BRIDGE_JSON_PASS checks="<<checks<<" shared_strict_parser=1 exact_numbers=1 shared_lifetime=1 private_header_in_consumer=0\n";return 0;}
    catch(const std::exception &error){std::cerr<<"BRIDGE_JSON_FAIL "<<error.what()<<'\n';return 1;}
}
