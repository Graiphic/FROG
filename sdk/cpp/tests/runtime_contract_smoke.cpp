#include "frog/sdk/runtime_contract.hpp"
#include "frog/sdk/runtime_contract_codec.hpp"

#include <cassert>

int main() {
    using namespace frog::sdk::runtime;

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
