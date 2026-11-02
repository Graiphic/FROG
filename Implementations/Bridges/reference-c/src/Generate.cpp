#include "ReferenceC.hpp"
#include <algorithm>
#include <bit>
#include <functional>

namespace reference_c {
namespace {
std::string literal(Type t,const J &v){switch(t){case Type::Bool:return v.boolean()?"1":"0";case Type::I32:return "frc_i32(UINT32_C("+std::to_string(static_cast<std::uint32_t>(signedInteger(v,32)))+"))";case Type::U32:return "UINT32_C("+std::to_string(unsignedInteger(v,32))+")";case Type::I64:return "frc_i64(UINT64_C("+std::to_string(static_cast<std::uint64_t>(signedInteger(v,64)))+"))";case Type::U64:return "UINT64_C("+std::to_string(unsignedInteger(v,64))+")";case Type::F64:return "frc_f64(UINT64_C("+std::to_string(std::bit_cast<std::uint64_t>(floating(v)))+"))";case Type::Array:fail("reference_c.constant_profile","Array constant expression is outside this profile.");}fail("reference_c.type_unsupported","Unknown literal type.");}
std::string read(Type t){switch(t){case Type::Bool:return "read_bool";case Type::F64:return "read_f64";case Type::I32:return "read_i32";case Type::U32:return "read_u32";case Type::I64:return "read_i64";case Type::U64:return "read_u64";case Type::Array:return "read_array";}fail("reference_c.type_unsupported","Unknown read type.");}
std::string print(Type t){switch(t){case Type::Bool:return "print_bool";case Type::F64:return "print_f64";case Type::I32:return "print_i32";case Type::U32:return "print_u32";case Type::I64:return "print_i64";case Type::U64:return "print_u64";case Type::Array:return "print_array";}fail("reference_c.type_unsupported","Unknown output type.");}
}
std::string generate(const Program &p){
    std::string c=R"FRC(/* Generated only from admitted public FIR; no Source/Studio interpreter. */
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
_Static_assert(sizeof(double)==8 && DBL_MANT_DIG==53 && FLT_RADIX==2,"binary64 required");
_Static_assert(sizeof(int64_t)==8 && sizeof(uint64_t)==8,"exact integer carriers required");
typedef struct {uint32_t n;double a[32];} frc_array;
static int bad_input=0;
static int64_t frc_i64(uint64_t x){int64_t y;memcpy(&y,&x,8);return y;}
static int32_t frc_i32(uint32_t x){int32_t y;memcpy(&y,&x,4);return y;}
static double frc_f64(uint64_t x){double y;memcpy(&y,&x,8);return y;}
static uint64_t read_u64(FILE *f){uint8_t b[8];uint64_t n=0;unsigned i;if(fread(b,1,8,f)!=8){bad_input=1;return 0;}for(i=0;i<8;++i)n|=((uint64_t)b[i])<<(8*i);return n;}
static uint32_t read_u32(FILE *f){uint8_t b[4];uint32_t n=0;unsigned i;if(fread(b,1,4,f)!=4){bad_input=1;return 0;}for(i=0;i<4;++i)n|=((uint32_t)b[i])<<(8*i);return n;}
static uint8_t read_bool(FILE *f){int n=fgetc(f);if(n<0||n>1){bad_input=1;return 0;}return (uint8_t)n;}
static int64_t read_i64(FILE *f){return frc_i64(read_u64(f));}
static int32_t read_i32(FILE *f){return frc_i32(read_u32(f));}
static double read_f64(FILE *f){return frc_f64(read_u64(f));}
static frc_array read_array(FILE *f){frc_array a={0};uint32_t i;a.n=read_u32(f);if(a.n>32){bad_input=1;a.n=0;return a;}for(i=0;i<a.n;++i)a.a[i]=read_f64(f);return a;}
static void print_bool(uint8_t n){fputs(n?"true":"false",stdout);}
static void print_i32(int32_t n){printf("%" PRId32,n);}
static void print_u32(uint32_t n){printf("%" PRIu32,n);}
static void print_i64(int64_t n){printf("%" PRId64,n);}
static void print_u64(uint64_t n){printf("%" PRIu64,n);}
static void print_f64(double n){if(isnan(n))fputs("\"nan\"",stdout);else if(isinf(n))fputs(signbit(n)?"\"-inf\"":"\"inf\"",stdout);else printf("%.17g",n);}
static void print_array(frc_array a){uint32_t i;printf("{\"shape\":[%" PRIu32 "],\"elements\":[",a.n);for(i=0;i<a.n;++i){if(i)fputc(',',stdout);print_f64(a.a[i]);}fputs("]}",stdout);}
int main(int argc,char **argv){
 FILE *input;char magic[8];const char *fault_code=0,*fault_message=0,*fault_object=0,*fault_pointer=0;int first=1;
 if(argc!=2 || !(input=fopen(argv[1],"rb")))return 2;
 if(fread(magic,1,8,input)!=8 || memcmp(magic,"FRCIN001",8)!=0)return 2;
)FRC";
    c+=" if(read_u32(input)!="+std::to_string(p.controls.size())+")return 2;\n";
    for(std::size_t i=0;i<p.controls.size();++i)c+=" "+ctype(p.controls[i].type)+" ctl"+std::to_string(i)+"="+read(p.controls[i].type)+"(input);\n";
    c+=" if(bad_input || fgetc(input)!=EOF)return 2;fclose(input);\n";
    for(const auto &n:p.nodes)for(const auto &v:n.ports)if(!v.input)c+=" "+ctype(v.type)+" "+v.variable+"={0};\n";
    const auto fault=[&](const Node &n,std::string_view code,std::string_view message){return "{fault_code="+cstring(encode(code))+";fault_message="+cstring(encode(message))+";fault_object="+cstring(encode(n.id))+";fault_pointer="+cstring(encode(n.sourcePointer))+";goto fault;}\n";};
    std::function<void(const std::string&)> region=[&](const std::string &id){for(const auto i:p.order.at(id)){const auto &n=p.nodes[i];
        if(n.family=="constant")c+=" "+port(n,"value",false).variable+"="+literal(port(n,"value",false).type,*n.literal)+";\n";
        else if(n.family=="widget_value"){if(!n.ports[0].input){const auto control=std::find_if(p.controls.begin(),p.controls.end(),[&](const Control &x){return x.widget==n.widget;});c+=" "+n.ports[0].variable+"=ctl"+std::to_string(control-p.controls.begin())+";\n";}}
        else if(n.kind=="for_loop"){
            for(const auto &s:n.shifts){for(const auto &stage:s.stages)c+=" "+port(n,stage.read,false).variable+"="+incoming(p,i,stage.initializer)+";\n";c+=" "+port(n,s.result,false).variable+"="+port(n,s.stages[0].read,false).variable+";\n";}
            c+=" "+port(n,"count_value",false).variable+"="+incoming(p,i,"count")+";\n for(int64_t it"+std::to_string(i)+"=0;it"+std::to_string(i)+"<"+std::to_string(n.count)+";++it"+std::to_string(i)+"){\n "+port(n,"iteration",false).variable+"=it"+std::to_string(i)+";\n";
            region(n.body);
            for(std::size_t group=0;group<n.shifts.size();++group){const auto &s=n.shifts[group];c+=" "+ctype(s.type)+" shift"+std::to_string(i)+"_"+std::to_string(group)+"="+incoming(p,i,s.write)+";\n";}
            for(std::size_t group=0;group<n.shifts.size();++group){const auto &s=n.shifts[group];const auto tmp="shift"+std::to_string(i)+"_"+std::to_string(group);for(std::size_t k=s.stages.size();k-->1;)c+=" "+port(n,s.stages[k].read,false).variable+"="+port(n,s.stages[k-1].read,false).variable+";\n";c+=" "+port(n,s.stages[0].read,false).variable+"="+tmp+";\n "+port(n,s.result,false).variable+"="+tmp+";\n";}c+=" }\n";
        }else if(n.kind=="frog.studio.array.build.real1d.v1"){const auto out=port(n,"result",false).variable;c+=" "+out+".n="+std::to_string(n.buildInputs.size())+";\n";for(std::size_t a=0;a<n.buildInputs.size();++a)c+=" "+out+".a["+std::to_string(a)+"]="+incoming(p,i,n.buildInputs[a])+";\n";}
        else if(n.kind=="frog.studio.array.index.real1d.v1"){const auto a=incoming(p,i,"array"),index=incoming(p,i,"index");c+=" if("+index+"<0 || (uint32_t)"+index+">="+a+".n)"+fault(n,"ARRAY_INDEX_RANGE","Array Index is outside the admitted rank-one bounds.");c+=" "+port(n,"element",false).variable+"="+a+".a[(uint32_t)"+index+"];\n";}
        else if(n.kind=="frog.studio.array.size.real1d.v1")c+=" "+port(n,"size",false).variable+"=(int32_t)"+incoming(p,i,"array")+".n;\n";
        else {const auto &out=port(n,"result",false);const auto a=incoming(p,i,"a"),z=incoming(p,i,"b");std::string expression;
            switch(out.type){case Type::F64:expression=a+"+"+z;break;case Type::I32:expression="frc_i32((uint32_t)"+a+"+(uint32_t)"+z+")";break;case Type::U32:expression="(uint32_t)("+a+"+"+z+")";break;case Type::I64:expression="frc_i64((uint64_t)"+a+"+(uint64_t)"+z+")";break;case Type::U64:expression="(uint64_t)("+a+"+"+z+")";break;default:fail("reference_c.add_type","Unsupported addition type.",n.id,n.sourcePointer);}
            c+=" "+out.variable+"="+expression+";\n";
        }
    }};region("");
    c+=" fputs(\"{\\\"schema\\\":\\\"frog.bridge.kernel.outputs\\\",\\\"schema_version\\\":\\\"0.1\\\",\\\"outcome\\\":\\\"completed\\\",\\\"outputs\\\":[\",stdout);\n";
    for(const auto &o:p.outputs){const auto &n=p.nodes[o.node];const auto expression=o.sink?incoming(p,o.node,o.port):port(n,o.port,false).variable;const auto prefix="{\"node_id\":"+encode(n.id)+",\"port_id\":"+encode(o.port)+",\"type\":"+encode(typeName(o.type))+",\"value\":";c+=" if(!first)fputc(',',stdout);first=0;fputs("+cstring(prefix)+",stdout);"+print(o.type)+"("+expression+");fputc('}',stdout);\n";}
    c+=R"FRC( fputs("]}",stdout);return ferror(stdout)?3:0;
fault:
 fputs("{\"schema\":\"frog.bridge.kernel.outputs\",\"schema_version\":\"0.1\",\"outcome\":\"fault\",\"outputs\":[],\"diagnostic\":{\"code\":",stdout);
 fputs(fault_code,stdout);fputs(",\"message\":",stdout);fputs(fault_message,stdout);fputs(",\"object_id\":",stdout);fputs(fault_object,stdout);fputs(",\"source_pointer\":",stdout);fputs(fault_pointer,stdout);fputs("}}",stdout);return ferror(stdout)?3:0;
}
)FRC";if(c.size()>2U*1024U*1024U)fail("reference_c.code_limit","Generated C exceeds2MiB.");return c;
}
}
