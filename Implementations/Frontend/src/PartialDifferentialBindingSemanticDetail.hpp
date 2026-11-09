#pragma once
#include "frog_native_engine/PartialDifferentialBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::partialdifferential_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
#include "../snapshot/generated/partial-differential-runtime.generated.inc"

struct Failure : std::runtime_error { std::string code; Failure(std::string c, std::string m) : std::runtime_error(std::move(m)), code(std::move(c)) {} };
[[noreturn]] void fail(std::string code, std::string message) { throw Failure(std::move(code), std::move(message)); }
std::string_view strip(std::string_view id) { if (id.starts_with("frog.studio.")) id.remove_prefix(12); return id; }
const FunctionBindingDefinition* definition(std::string_view id) {
    static const auto definitions = partialDifferentialRuntimeDefinitions(); id = strip(id);
    const auto found = std::find_if(definitions.begin(), definitions.end(), [id](const auto& d) { return d.catalogId == id; });
    return found == definitions.end() ? nullptr : &*found;
}
const FunctionPortBinding* port(const FunctionBindingDefinition& d, std::string_view id) {
    const auto found = std::find_if(d.ports.begin(), d.ports.end(), [id](const auto& p) { return p.portId == id; });
    return found == d.ports.end() ? nullptr : &*found;
}
T parse(std::string_view s) { auto p = binding::parseType(s); if (!p.valid()) fail("PDE_SHAPE_MISMATCH", p.error); return p.type; }
}
}
