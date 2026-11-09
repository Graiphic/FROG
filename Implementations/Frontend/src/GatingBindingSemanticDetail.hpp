#pragma once
#include "frog_native_engine/GatingBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/GatingOperation.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::gating_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
using Operation=GatingRealOperation;
Operation operation(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id=="math.gating.spike")return Operation::Spike;
    if(id=="math.gating.square")return Operation::Square;
    if(id=="math.gating.step")return Operation::Step;
    return Operation::Unknown;
}
GatingTypeResult failure(ResolutionStatus status,std::string code,std::string message) {
    return {{status,{},std::move(code),std::move(message)},false};
}

}
}
