// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ClassSource.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <array>
#include <set>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
namespace cj=canonical_json;
constexpr std::array<std::string_view,5> kinds{"default","read_field","write_field","static_call","dynamic_call"};
Json revision(std::uint32_t n){Json v;v.type=Json::Type::Number;v.string=std::to_string(n);return v;}
Json dependency(const ClassDependency &d){auto v=cj::object();v.object={{"id",cj::string(d.id)},
    {"path",cj::string(d.path)},{"revision",revision(d.revision)}};return v;}
}
bool readClassSourceDeclaration(const Json &root,ClassSourceDeclaration &out,std::string *error) {
    ClassSourceDeclaration next;
    if(const auto *refs=root.member("classes")) {
        if(refs->type!=Json::Type::Array||refs->array.size()>kMaximumClassClosure) {
            if(error)*error="classes requires at most 1024 explicit class dependencies.";return false;
        }
        std::set<std::string> ids;
        for(const auto &ref:refs->array) {
            auto value=readClassDependency(ref,kClassDefinitionExtension,error);
            if(!value)return false;
            if(!ids.insert(value->id).second){if(error)*error="Duplicate nominal class dependency.";return false;}
            next.classes.push_back(std::move(*value));
        }
    }
    if(root.member("class_member")) {
        next.member=readClassMemberIdentity(root,error);
        if(!next.member)return false;
    }
    out=std::move(next);return true;
}
Json writeClassSourceDeclaration(const ClassSourceDeclaration &source) {
    auto result=cj::object();
    if(!source.classes.empty()) {
        auto refs=cj::array();for(const auto &ref:source.classes)refs.array.push_back(dependency(ref));
        result.object.emplace("classes",std::move(refs));
    }
    if(source.member) {
        const auto &m=*source.member;auto member=cj::object();
        member.object={{"class_id",cj::string(m.classId)},{"method_id",cj::string(m.methodId)},
            {"implementation_id",cj::string(m.implementationId)},{"revision",revision(m.revision)}};
        result.object.emplace("class_member",std::move(member));
    }
    return result;
}
std::optional<ClassSourceOperation> readClassSourceOperation(const Json &value,std::string *error) {
    const auto fail=[&]()->std::optional<ClassSourceOperation>{if(error)*error="Invalid closed class operation declaration.";return {};};
    if(value.type!=Json::Type::Object||value.object.size()!=3)return fail();
    const auto *kind=value.member("kind"),*owner=value.member("class"),*member=value.member("member_id");
    if(!kind||kind->type!=Json::Type::String||!owner||!member||member->type!=Json::Type::String)return fail();
    const auto at=std::find(kinds.begin(),kinds.end(),kind->string);
    if(at==kinds.end())return fail();
    auto ref=readClassDependency(*owner,kClassDefinitionExtension,error);if(!ref)return {};
    const auto op=static_cast<ClassOperationKind>(at-kinds.begin());
    if((op==ClassOperationKind::Default)!=member->string.empty()||member->string.size()>128)return fail();
    return ClassSourceOperation{op,std::move(*ref),member->string};
}
Json writeClassSourceOperation(const ClassSourceOperation &op) {
    auto v=cj::object();v.object={{"kind",cj::string(std::string(kinds.at(static_cast<std::size_t>(op.kind))))},
        {"class",dependency(op.owner)},{"member_id",cj::string(op.memberId)}};return v;
}
}
