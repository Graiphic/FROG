// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ClassProgram.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "FrogDocumentArrayStorage.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <charconv>
#include <set>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
bool identity(std::string_view id) {
    return !id.empty()&&id.size()<=128&&std::all_of(id.begin(),id.end(),[](unsigned char c){
        return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='.'||c=='-';});
}
}
std::optional<ClassMemberIdentity> readClassMemberIdentity(const Json &root,std::string *error) {
    const auto *member=root.member("class_member");
    if(!member)return {};
    const auto fail=[&]()->std::optional<ClassMemberIdentity>{
        if(error)*error="class_member requires class_id, method_id, implementation_id and a positive uint32 revision.";
        return {};
    };
    if(member->type!=Json::Type::Object||member->object.size()!=4)return fail();
    ClassMemberIdentity result;
    for(auto pair:{std::pair{"class_id",&result.classId},std::pair{"method_id",&result.methodId},
        std::pair{"implementation_id",&result.implementationId}}) {
        const auto *value=member->member(pair.first);
        if(!value||value->type!=Json::Type::String||!identity(value->string))return fail();
        *pair.second=value->string;
    }
    const auto *rev=member->member("revision");
    if(!rev||rev->type!=Json::Type::Number)return fail();
    const auto parsed=std::from_chars(rev->string.data(),rev->string.data()+rev->string.size(),result.revision);
    if(parsed.ec!=std::errc{}||parsed.ptr!=rev->string.data()+rev->string.size()||!result.revision)return fail();
    return result;
}
const CapturedClassMethod *CapturedClassProgram::findMember(const ClassCallTarget &target) const {
    const auto found=std::find_if(methods.begin(),methods.end(),[&](const auto &m){return m.declaration==target;});
    return found==methods.end()?nullptr:&*found;
}
std::optional<ClassCallTarget> CapturedClassProgram::attributeMember(std::string_view uri,std::string_view digest) const {
    if(!accepted()||uri.empty()||digest.empty())return {};
    const auto found=std::find_if(methods.begin(),methods.end(),[&](const auto &m){return m.source.canonicalUri==uri&&m.source.sha256==digest;});
    return found==methods.end()?std::nullopt:std::optional{found->declaration};
}
CapturedClassProgram captureClassProgram(std::span<const ClassDependencyRoot> roots,const DraftDependencyResolver &resolver) {
    CapturedClassProgram result;
    result.classes=captureClassHierarchy(roots,resolver);
    if(!result.classes.hierarchy||!result.classes.issues.empty())return result;
    constexpr std::size_t maximumBytes=64U*1024U*1024U,maximumMethods=1024;
    std::size_t totalBytes=0;
    for(const auto &definition:result.classes.definitions)totalBytes+=definition.bytes->size();
    std::map<std::string,ClassCallTarget,std::less<>> locations;
    for(const auto &definition:result.classes.definitions) {
        const auto *owner=result.classes.hierarchy->findClass(definition.identity);
        for(std::size_t index=0;index<owner->methods.size();++index) {
            const auto &method=owner->methods[index];
            const ClassCallTarget target{owner->id,method.id};
            const auto pointer="/methods/"+std::to_string(index)+"/implementation";
            const auto issue=[&](std::string code,std::string message,std::string uri={},std::string related={}) {
                if(result.issues.size()<256)result.issues.push_back({std::move(code),owner->id,pointer,std::move(message),
                    definition.canonicalUri,std::move(uri),std::move(related)});
            };
            if(result.methods.size()>=maximumMethods){issue("class.member_limit","A class program exceeds 1024 actual member sources.");return result;}
            auto captured=resolveDraftDependency({DraftDependencyKind::Source,definition.canonicalUri,method.implementation.path,pointer,{},{},{}},resolver);
            if(captured.state!=DraftDependencyState::Present||!captured.bytes) {
                issue(captured.diagnosticCode.empty()?"class.member_unavailable":captured.diagnosticCode,
                    captured.message.empty()?"The declared member FROG '"+method.implementation.path+"' could not be captured.":captured.message);continue;
            }
            if(!locations.emplace(captured.canonicalUri,target).second) {
                issue("class.member_duplicate","Different member declarations resolve to the same source location.",captured.canonicalUri,"/class_member");continue;
            }
            if(captured.bytes->size()>maximumBytes-totalBytes) {
                issue("class.member_limit","The complete class/member closure exceeds 64 MiB.");return result;
            }
            totalBytes+=captured.bytes->size();
            std::string error;
            const auto raw=source::parseJson(*captured.bytes,&error,source::JsonNumberPolicy::PreserveLexeme);
            if(!raw){issue("class.member_format",error,captured.canonicalUri,"/");continue;}
            const auto member=readClassMemberIdentity(*raw,&error);
            if(!member||member->classId!=owner->id||member->methodId!=method.id||
                member->implementationId!=method.implementation.id||member->revision!=method.implementation.revision) {
                issue("class.member_identity",error.empty()?"Actual member attribution does not match its .fclass declaration.":error,
                    captured.canonicalUri,"/class_member");continue;
            }
            auto normalized=prepareExecutionStorageSource(*captured.bytes,&error);
            auto document=normalized?readFrogDraftData(*normalized,&error):std::nullopt;
            if(!document||document->publicSourceAuthority||document->privateSourceAwaitingDiagramMigration||!document->declaredInterface) {
                issue("class.member_format",error.empty()?"A class member requires an authored draft and explicit connector.":error,captured.canonicalUri,"/interface");continue;
            }
            const auto actual=inspectFrogDraftInterface(*document->declaredInterface);
            if(!actual.inspectable||!actual.contractValid) {
                issue("class.member_connector","The actual member connector is invalid.",captured.canonicalUri,"/interface");continue;
            }
            ClassImplementationInterface connector{member->implementationId,member->revision,{}};
            if(std::any_of(actual.ports.begin(),actual.ports.end(),[](const auto &p){return p.input&&p.connection!="required";})) {
                issue("class.method_connector","Member inputs require actual caller producers; optional/default inputs are outside the by-value member profile.",captured.canonicalUri,"/interface/inputs");continue;
            }
            for(const auto &port:actual.ports) {
                const auto &type=port.parsedType.type;
                if(type.kind==binding::TypeKind::Object)
                    connector.ports.push_back({port.id,port.input,ClassPortKind::Object,{},type.name});
                else connector.ports.push_back({port.id,port.input,ClassPortKind::Data,type,{}});
            }
            auto failures=validateClassImplementation(*owner,index,connector);
            for(auto &failure:failures) {
                failure.sourceUri=definition.canonicalUri;failure.relatedSourceUri=captured.canonicalUri;
                failure.relatedPointer="/interface";result.issues.push_back(std::move(failure));
            }
            if(!failures.empty())continue;
            // The resolver's identity/revision describe a source snapshot.
            // Nominal member identity/revision come from verified source bytes.
            result.methods.push_back({target,std::move(captured),std::move(connector),
                std::make_shared<const FrogDraftData>(std::move(*document))});
        }
    }
    return result;
}
}
