#include "frog_native_engine/StructureBindingDefinition.hpp"
#include "CaseBindingDefinition.generated.hpp"
namespace frog::native_engine::binding {
const StructureBindingDefinition &caseBindingDefinition() {
    static const StructureBindingDefinition definition{
        generated::caseId, generated::caseVersion, generated::outerPort,
        generated::innerPort, parseTypeExpr(generated::selectorExpression).expression,
        generated::everyRegion, generated::allowDefault, generated::resolverId, generated::loweringId};
    return definition;
}
const StructureBindingDefinition *findStructureBindingDefinition(std::string_view id) {
    const auto &definition = caseBindingDefinition();
    return id == definition.id ? &definition : nullptr;
}
}
