#pragma once
#include <frog/bridge.hpp>
#include <frog/bridge_json.hpp>
#include <cstdint>
#include <filesystem>
#include <initializer_list>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace reference_c {
namespace b=frog::bridge;
namespace j=frog::bridge::json;
using J=j::Value;
struct Failure:std::runtime_error {
    std::string code,object,pointer;
    Failure(std::string c,std::string message,std::string o={},std::string p={}):std::runtime_error(std::move(message)),code(std::move(c)),object(std::move(o)),pointer(std::move(p)){}
};
[[noreturn]] inline void fail(std::string code,std::string message,std::string object={},std::string pointer={}){throw Failure(std::move(code),std::move(message),std::move(object),std::move(pointer));}
J parse(std::string_view);
J field(const J&,std::string_view);
std::string text(const J&);
std::string str(const J&,std::string_view);
void fields(const J&,std::initializer_list<std::string_view>,std::initializer_list<std::string_view> = {});
std::int64_t signedInteger(const J&,unsigned bits);
std::uint64_t unsignedInteger(const J&,unsigned bits);
double floating(const J&);
std::string encode(std::string_view s);
std::string cstring(std::string_view);
enum class Type {Bool,F64,I32,U32,I64,U64,Array};
Type type(std::string_view);
std::string typeName(Type);
std::string ctype(Type);
struct Port {std::string id;Type type;bool input=false;std::string variable;};
struct Stage {std::string initializer,read;std::size_t history=0;};
struct Shift {std::string result,write;Type type;std::vector<Stage> stages;};
struct Node {
    std::string id,family,kind,region,sourcePointer,widget;
    std::vector<Port> ports;
    std::optional<J> literal,defaultValue;
    std::vector<std::string> buildInputs;
    std::string body;
    std::vector<Shift> shifts;
    std::int64_t count=0;
};
struct Edge {std::size_t from=0,to=0;std::string output,input;};
struct Output {std::size_t node=0;std::string port;Type type;bool sink=false;};
struct Control {std::string widget;Type type;std::optional<J> initial;};
struct Program {
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::map<std::string,std::size_t> byId;
    std::map<std::string,std::vector<std::size_t>> order;
    std::vector<Output> outputs;
    std::vector<Control> controls;
    b::GraphRequirements requirements;
};
Program admit(std::string_view,const b::GraphRequirements&);
std::string generate(const Program&);
std::string controlBytes(const Program&,const std::optional<J>& parameters);
void checkOutputs(const Program&,std::string_view);
const Port &port(const Node&,std::string_view,bool input);
std::string incoming(const Program&,std::size_t,std::string_view);

struct Compiler {
    struct State;
    std::shared_ptr<const State> state;
    std::string configSha,version;
    std::filesystem::path executable,linker;
    std::vector<std::filesystem::path> includes,libraries;
};
Compiler compiler(const std::filesystem::path&);
std::filesystem::path executablePath();
std::string readFile(const std::filesystem::path&,std::size_t);
void writeFile(const std::filesystem::path&,std::string_view);
std::string fileHash(const std::filesystem::path&);
std::string nonce();
std::string excerpt(std::string_view);
std::string hex(std::string_view);
std::wstring wide(std::string_view);
std::string utf8(const std::filesystem::path&);
struct ProcessResult {std::uint32_t exit=0;std::string output,error;std::vector<std::filesystem::path> artifacts;};
ProcessResult process(const std::filesystem::path&,const std::vector<std::wstring>&,const std::filesystem::path&,const Compiler&,std::uint32_t);
void compile(const Compiler&,const std::filesystem::path&,std::vector<std::filesystem::path>& artifacts);
}
