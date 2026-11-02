#include "frog/sdk/runtime_contract.hpp"
#include "frog/sdk/runtime_contract_codec.hpp"
#include "frog/sdk/runtime_array_value.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

int main() {
    using namespace frog::sdk::runtime;
    const auto require=[](bool condition,const char *message){if(!condition)throw std::runtime_error(message);};
    const auto nan=std::numeric_limits<double>::quiet_NaN();
    const auto inf=std::numeric_limits<double>::infinity();
    for(const auto name:{"f32","f64"}) {
        const auto type=valueTypeFromCanonicalName(name);
        require(runtimeValueRepresentable(RuntimeValue{nan},type),"IEEE NaN lost its physical real carrier");
        require(runtimeValueRepresentable(RuntimeValue{inf},type) && runtimeValueRepresentable(RuntimeValue{-inf},type),"IEEE infinity lost its sign/real carrier");
    }
    require(!runtimeValueRepresentable(RuntimeValue{0.1},valueTypeFromCanonicalName("f32")),"Finite F64 was silently narrowed to F32");
    require(!runtimeValueRepresentable(RuntimeValue{nan},valueTypeFromCanonicalName("i64")),"An IEEE real impersonated an integer");
    const RuntimeValueOwner ieeeOwner{"xy-ieee",std::string(64,'a'),1,2,3};
    const auto array=makeRuntimeArray(valueTypeFromCanonicalName("f64"),{3},{RuntimeValue{nan},RuntimeValue{inf},RuntimeValue{-inf}},ieeeOwner);
    const auto arrayType=valueTypeFromCanonicalName("array<f64>");
    require(array && runtimeValueValidForOwner(*array,arrayType,ieeeOwner),"Owned XY IEEE Array was rejected");
    auto foreignOwner=ieeeOwner;++foreignOwner.epoch;
    require(!runtimeValueValidForOwner(*array,arrayType,foreignOwner),"IEEE transport weakened Array ownership");
    for(const auto name:{"c32","c64"}) {
        const auto type=valueTypeFromCanonicalName(name);const auto value=makeRuntimeComplex(type,nan,-inf);
        const auto parts=value?readRuntimeComplex(*value,type):std::nullopt;
        require(parts && std::isnan(parts->real) && std::isinf(parts->imaginary) && parts->imaginary<0,"Complex IEEE coordinates were lost");
        require(runtimeValueRepresentable(*value,type),"Complex IEEE aggregate is not representable");
    }
    require(!makeRuntimeComplex(valueTypeFromCanonicalName("c32"),0.1,0),"Finite complex components were silently narrowed");
    require(!makeRuntimeComplex(valueTypeFromCanonicalName("c32"),(std::numeric_limits<double>::max)(),0),"Finite complex overflow became infinity implicitly");

    ProgramArtifact artifact;
    artifact.artifactId = ArtifactId{ "artifact-1" };
    artifact.sourceDocumentId = DocumentId{ "document-1" };
    artifact.revision = ArtifactRevision{ 2U, 3U, 4U };

    RuntimeNode control;
    control.id = NodeId{ "control" };
    control.kind = NodeKind::Control;
    control.operationId = "frog.ui.numeric.control";
    control.ports.push_back(RuntimePort{
        PortId{ "value" }, "value", PortDirection::Output,
        valueTypeFromCanonicalName("f64") });
    artifact.nodes.push_back(control);

    RuntimeNode indicator;
    indicator.id = NodeId{ "indicator" };
    indicator.kind = NodeKind::Indicator;
    indicator.operationId = "frog.ui.numeric.indicator";
    indicator.ports.push_back(RuntimePort{
        PortId{ "value" }, "value", PortDirection::Input,
        valueTypeFromCanonicalName("f64") });
    artifact.nodes.push_back(indicator);

    artifact.edges.push_back(RuntimeEdge{
        EdgeId{ "edge" },
        RuntimeEndpoint{ NodeId{ "control" }, PortId{ "value" } },
        RuntimeEndpoint{ NodeId{ "indicator" }, PortId{ "value" } }
    });
    artifact.uiBindings.push_back(UiBinding{
        WidgetId{ "control-widget" },
        RuntimeEndpoint{ NodeId{ "control" }, PortId{ "value" } },
        UiBindingDirection::ControlToRuntime,
        valueTypeFromCanonicalName("f64"),
        RuntimeValue{ 1.5 }
    });
    artifact.uiBindings.push_back(UiBinding{
        WidgetId{ "indicator-widget" },
        RuntimeEndpoint{ NodeId{ "indicator" }, PortId{ "value" } },
        UiBindingDirection::RuntimeToIndicator,
        valueTypeFromCanonicalName("f64"),
        RuntimeValue{ 0.0 }
    });

    assert(validateProgramArtifact(artifact).empty());

    RuntimeCapabilities capabilities;
    capabilities.targetId = "frog.runtime.test";
    capabilities.valueTypes = { "f64" };
    assert(validateRuntimeCapabilities(artifact, capabilities).empty());

    const auto encoded = encodeProgramArtifact(artifact);
    const auto decoded = decodeProgramArtifact(encoded);
    assert(decoded.diagnostics.empty());
    assert(decoded.artifact.has_value());
    assert(decoded.artifact->artifactId == artifact.artifactId);
    assert(decoded.artifact->revision == artifact.revision);
    assert(decoded.artifact->nodes == artifact.nodes);
    assert(decoded.artifact->edges == artifact.edges);
    assert(decoded.artifact->uiBindings == artifact.uiBindings);

    auto truncated = encoded;
    truncated.pop_back();
    const auto rejected = decodeProgramArtifact(truncated);
    assert(!rejected.artifact.has_value());
    assert(!rejected.diagnostics.empty());
    assert(valueTypeFromCanonicalName("u8").bitWidth == 8U);
    assert(valueTypeFromCanonicalName("array:bool").rank == 1U);
    assert(runtimeValueMatches(RuntimeValue{ true }, valueTypeFromCanonicalName("bool")));
    assert(!runtimeValueMatches(RuntimeValue{ true }, valueTypeFromCanonicalName("f64")));

    artifact.edges.front().source.portId = PortId{ "missing" };
    const auto diagnostics = validateProgramArtifact(artifact);
    assert(!diagnostics.empty());
    assert(diagnostics.front().blocksRun);
    return 0;
}
