// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/ClassDefinition.hpp"

namespace frog::native_engine {
struct ClassMemberIdentity {
    std::string classId,methodId,implementationId;
    std::uint32_t revision=0;
    bool operator==(const ClassMemberIdentity &) const = default;
};
struct ClassSourceDeclaration {
    std::vector<ClassDependency> classes;
    std::optional<ClassMemberIdentity> member;
    bool operator==(const ClassSourceDeclaration &) const = default;
};
enum class ClassOperationKind { Default,ReadField,WriteField,StaticCall,DynamicCall };
struct ClassSourceOperation {
    ClassOperationKind kind=ClassOperationKind::Default;
    ClassDependency owner;
    std::string memberId;
    bool operator==(const ClassSourceOperation &) const = default;
};
[[nodiscard]] std::optional<ClassMemberIdentity> readClassMemberIdentity(const source::JsonValue &,std::string *error=nullptr);
[[nodiscard]] bool readClassSourceDeclaration(const source::JsonValue &,ClassSourceDeclaration &,std::string *error=nullptr);
[[nodiscard]] source::JsonValue writeClassSourceDeclaration(const ClassSourceDeclaration &);
[[nodiscard]] std::optional<ClassSourceOperation> readClassSourceOperation(const source::JsonValue &,std::string *error=nullptr);
[[nodiscard]] source::JsonValue writeClassSourceOperation(const ClassSourceOperation &);
}
