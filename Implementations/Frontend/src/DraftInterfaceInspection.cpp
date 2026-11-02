#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/DraftConditionalDisableSpecialization.hpp"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
namespace frog::native_engine {
void addDocumentDiagnostic(
    DocumentReadiness &readiness,
    DocumentDiagnosticSeverity severity,
    DocumentDiagnosticDomain domain,
    std::string code,
    std::string subjectId,
    std::string message,
    bool blocksStructure,
    bool blocksSemantics,
    bool blocksBuild,
    bool blocksRun) {
    if (blocksStructure) {
        readiness.structurallyValid = false;
    }
    if (blocksSemantics) {
        readiness.semanticallyValid = false;
    }
    if (blocksBuild) {
        readiness.buildable = false;
    }
    if (blocksRun) {
        readiness.runnable = false;
    }
    readiness.diagnostics.push_back(DocumentDiagnostic{
        severity,
        domain,
        std::move(code),
        std::move(subjectId),
        std::move(message),
        blocksBuild,
        blocksRun
    });
    if(domain==DocumentDiagnosticDomain::RuntimeTarget)
        readiness.diagnostics.back().stage=DocumentDiagnosticStage::Capability;
    else if(domain==DocumentDiagnosticDomain::FrontendCapability)
        readiness.diagnostics.back().stage=DocumentDiagnosticStage::Resolve;
}

void validateDeclaredDraftInterface(const std::optional<FrogDraftInterface> &declaration,
    FrogWidgetDataView canvas, std::span<const DiagramNode *const> diagramNodes,
    DocumentReadiness &readiness) {
    if (!declaration) return; // Explicit legacy absence is not an inferred API.
    const auto inspection = inspectFrogDraftInterface(*declaration);
    for (const auto &diagnostic : inspection.diagnostics) {
        addDocumentDiagnostic(readiness, diagnostic.severity, diagnostic.domain, diagnostic.code,
            diagnostic.subjectId, diagnostic.message, false,
            diagnostic.domain == DocumentDiagnosticDomain::InterfaceContract &&
                diagnostic.severity == DocumentDiagnosticSeverity::Error,
            diagnostic.blocksBuild, diagnostic.blocksRun);
    }
    if(!inspection.inspectable)readiness.validationIncomplete=true;
    if (!inspection.inspectable || std::any_of(inspection.diagnostics.begin(), inspection.diagnostics.end(),
        [](const auto &diagnostic) { return diagnostic.code == "draft_interface.invalid_declaration"; })) return;
    const auto unsupported = [&](std::string code, std::string subject, std::string message) {
        addDocumentDiagnostic(readiness, DocumentDiagnosticSeverity::Warning, DocumentDiagnosticDomain::RuntimeTarget,
            std::move(code), std::move(subject), std::move(message), false, false, true, true);
    };
    const auto invalid = [&](std::string code, std::string subject, std::string message) {
        addDocumentDiagnostic(readiness, DocumentDiagnosticSeverity::Error, DocumentDiagnosticDomain::InterfaceContract,
            std::move(code), std::move(subject), std::move(message), false, true, true, true);
    };
    struct DeclaredPort {
        bool input;
        binding::TypeRef type;
        bool supportedType;
        std::string_view connection;
        std::string path;
    };
    std::unordered_map<std::string_view, DeclaredPort> ports;
    std::vector<std::string_view> portOrder;
    // Binding validation keeps the historical first unique, nonempty ID. The
    // inspection still exposes every invalid/duplicate entry without repair.
    for (const auto &port : inspection.ports) {
        if (port.id.empty() || ports.contains(port.id)) continue;
        const bool supportedType = !port.typeExpression.empty() &&
            port.parsedType.valid() && port.parsedType.type.known();
        ports.emplace(port.id, DeclaredPort{port.input, port.parsedType.type,
            supportedType, port.connection, port.sourcePath});
        portOrder.push_back(port.id);
    }
    std::unordered_set<std::string_view> terminalWidgets;
    for (const auto *node : diagramNodes)
        if (node->kind == "widget_value" && !node->widgetId.empty()) terminalWidgets.insert(node->widgetId);
    std::unordered_set<std::string_view> represented;
    for (const auto &object : canvas.objects()) {
        if (!object.widget) continue;
        const auto &widget = *object.widget;
        for (const bool input : {true, false}) {
            const auto &bindingId = input ? widget.publicInputId : widget.publicOutputId;
            if (!bindingId || bindingId->empty()) continue;
            const auto found = ports.find(*bindingId);
            if (found == ports.end()) {
                invalid("draft_interface.binding_id_missing", object.id,
                    "Widget binding references undeclared private interface port '" + *bindingId + "'.");
                continue;
            }
            const auto &port = found->second;
            if (port.input != input || parseWidgetRole(widget.role).value_or(WidgetRole::Support) !=
                (input ? WidgetRole::Control : WidgetRole::Indicator)) {
                invalid("draft_interface.binding_direction_mismatch", object.id,
                    "Widget binding direction/role contradicts declared private interface port '" + *bindingId + "'.");
                continue;
            }
            const auto widgetType = binding::widgetType(widget);
            if (!port.supportedType || !widgetType.valid() || !widgetType.type.known()) continue;
            if (port.type.canonical() != widgetType.type.canonical()) {
                invalid("draft_interface.binding_type_mismatch", object.id,
                    "Widget type '" + widgetType.type.canonical() + "' contradicts declared port '" + *bindingId +
                    "' of type '" + port.type.canonical() + "'.");
                continue;
            }
            if (input && widget.interfaceConnectionRequirement.value_or("recommended") != port.connection) {
                invalid("draft_interface.binding_connection_mismatch", object.id,
                    "Widget input connection policy contradicts declared private interface port '" + *bindingId + "'.");
                continue;
            }
            if (terminalWidgets.contains(object.id)) represented.insert(found->first);
        }
    }
    for (const auto id : portOrder) if (!represented.contains(id)) {
        const auto &port = ports.at(id);
        unsupported("draft_interface.boundary_unsupported", port.path,
            "Declared private interface port '" + std::string(id) +
            "' has no compatible widget binding and active Diagram terminal supported by this execution boundary. "
            "The declaration is preserved; this is a local capability limit, not public FROG invalidity.");
    }
}

void validatePublicInterface(
    FrogWidgetDataView canvas,
    DocumentReadiness &readiness) {
    std::unordered_set<std::string> publicIds;
    std::unordered_set<std::string> slotIds;
    for (const auto &object : canvas.objects()) {
        if (!object.widget.has_value()) {
            continue;
        }
        const auto &widget = object.widget.value();
        const bool hasInput = widget.publicInputId.has_value() &&
            !widget.publicInputId->empty();
        const bool hasOutput = widget.publicOutputId.has_value() &&
            !widget.publicOutputId->empty();
        const bool hasSlot = widget.interfaceMapSlotId.has_value() &&
            !widget.interfaceMapSlotId->empty();

        if (hasInput && hasOutput) {
            addDocumentDiagnostic(
                readiness, DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::InterfaceContract,
                "interface_bidirectional_widget", object.id,
                "A widget cannot expose both a public input and a public output.",
                false, true, true, true);
        }
        if ((hasInput || hasOutput) != hasSlot) {
            addDocumentDiagnostic(
                readiness, DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::InterfaceContract,
                "interface_slot_binding_mismatch", object.id,
                "A public widget binding and its interface-map slot must be declared together.",
                false, true, true, true);
        }

        const auto registerUnique = [&](const std::string &id, std::string_view code) {
            if (!publicIds.insert(id).second) {
                addDocumentDiagnostic(
                    readiness, DocumentDiagnosticSeverity::Error,
                    DocumentDiagnosticDomain::InterfaceContract,
                    std::string(code), object.id,
                    "Public interface IDs must be unique within one document.",
                    false, true, true, true);
            }
        };
        if (hasInput) {
            registerUnique(*widget.publicInputId, "duplicate_public_input_id");
        }
        if (hasOutput) {
            registerUnique(*widget.publicOutputId, "duplicate_public_output_id");
        }
        if (hasSlot && !slotIds.insert(*widget.interfaceMapSlotId).second) {
            addDocumentDiagnostic(
                readiness, DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::InterfaceContract,
                "duplicate_interface_slot", object.id,
                "An interface-map slot cannot be assigned to multiple widgets.",
                false, true, true, true);
        }
    }
}

void validateFrogCallPortIdentities(const DiagramNode &node, DocumentReadiness &readiness) {
    std::unordered_set<std::string> callPortIds;
    for (const auto &port : node.callPorts) {
        if (port.id.empty() || !callPortIds.insert(port.id).second) {
            addDocumentDiagnostic(
                readiness, DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::InterfaceContract,
                "invalid_frog_call_port", node.id,
                "FROG call ports require unique, non-empty IDs.",
                false, true, true, true);
        }
    }
}

FrogDraftInterfaceInspection inspectFrogDraftInterface(const FrogDraftInterface &retainedDeclaration) {
    FrogDraftInterfaceInspection result;
    result.declaration = retainedDeclaration;
    DocumentReadiness diagnosticSink;
    using source::JsonValue;
    std::string inspectionError;
    const auto declaration = source::parseJson(retainedDeclaration.declarationUtf8(),
        &inspectionError, source::JsonNumberPolicy::PreserveLexeme);
    const auto unsupported = [&](std::string code, std::string subject, std::string message) {
        addDocumentDiagnostic(diagnosticSink, DocumentDiagnosticSeverity::Warning, DocumentDiagnosticDomain::RuntimeTarget,
            std::move(code), std::move(subject), std::move(message), false, false, true, true);
    };
    const auto invalid = [&](std::string code, std::string subject, std::string message) {
        result.contractValid = false;
        addDocumentDiagnostic(diagnosticSink, DocumentDiagnosticSeverity::Error, DocumentDiagnosticDomain::InterfaceContract,
            std::move(code), std::move(subject), std::move(message), false, true, true, true);
    };
    if (!declaration) {
        unsupported("draft_interface.inspection_unsupported", "/interface",
            "The retained private interface cannot be inspected by the strict declaration reader. Its bytes are preserved. " + inspectionError);
        result.diagnostics = std::move(diagnosticSink.diagnostics);
        return result;
    }
    result.inspectable = true;
    if (declaration->type != JsonValue::Type::Object) {
        invalid("draft_interface.invalid_declaration", "/interface", "A private declared interface must be an object.");
        result.diagnostics = std::move(diagnosticSink.diagnostics);
        return result;
    }
    result.contractValid = true;
    std::unordered_set<std::string_view> declaredIds;
    for (const bool input : {true, false}) {
        const std::string_view direction = input ? "inputs" : "outputs";
        const auto *values = declaration->member(direction);
        if (!values) continue;
        const auto directionPath = "/interface/" + std::string(direction);
        if (values->type != JsonValue::Type::Array) {
            invalid("draft_interface.invalid_ports", directionPath, "Private interface inputs and outputs must be arrays.");
            continue;
        }
        for (std::size_t index = 0U; index < values->array.size(); ++index) {
            const auto &port = values->array[index];
            const auto path = directionPath + '/' + std::to_string(index);
            const auto *id = port.member("id");
            const auto *type = port.member("type");
            const bool validTypeField = type && type->type == JsonValue::Type::String && !type->string.empty();
            FrogDraftDeclaredPort view;
            view.input = input;
            view.ordinal = index;
            view.sourcePath = path;
            view.sourcePortUtf8 = std::string(retainedDeclaration.declarationUtf8().substr(
                port.sourceBegin, port.sourceEnd - port.sourceBegin));
            if (id && id->type == JsonValue::Type::String) view.id = id->string;
            if (type && type->type == JsonValue::Type::String) view.typeExpression = type->string;
            view.parsedType = validTypeField ? binding::parseType(type->string) : binding::TypeParseResult{};
            if (const auto *name = port.member("name"); name && name->type == JsonValue::Type::String)
                view.sourceName = name->string;
            if (const auto *label = port.member("label"); label && label->type == JsonValue::Type::String)
                view.sourceLabel = label->string;
            if (const auto *policy = port.member("connection")) {
                view.connectionExplicit = true;
                if (input && policy->type == JsonValue::Type::String &&
                    (policy->string == "required" || policy->string == "recommended" || policy->string == "optional"))
                    view.connection = policy->string;
            }
            result.ports.push_back(std::move(view));
            const auto &inspectedPort = result.ports.back();
            if (port.type != JsonValue::Type::Object || !id || id->type != JsonValue::Type::String || id->string.empty()) {
                invalid("draft_interface.invalid_id", path, "A private interface port requires a nonempty string ID.");
                continue;
            }
            if (!declaredIds.insert(id->string).second) {
                invalid("draft_interface.duplicate_id", path + "/id", "Private interface IDs must be unique across inputs and outputs.");
                continue;
            }
            if (!validTypeField) {
                invalid("draft_interface.invalid_type", path + "/type", "A private interface port requires a nonempty type expression string.");
            }
            const bool supportedType = validTypeField && inspectedPort.parsedType.valid() && inspectedPort.parsedType.type.known();
            if (validTypeField && !supportedType) unsupported("draft_interface.type_unsupported", path + "/type",
                "The private execution boundary cannot interpret declared type '" + type->string + "'; its declaration is preserved.");
            if (const auto *policy = port.member("connection")) {
                if (!input || policy->type != JsonValue::Type::String ||
                    (policy->string != "required" && policy->string != "recommended" && policy->string != "optional")) {
                    invalid("draft_interface.invalid_connection", path + "/connection",
                        "Private input connection policy must be required, recommended or optional; outputs do not declare an input policy.");
                }
            }
        }
    }
    result.diagnostics = std::move(diagnosticSink.diagnostics);
    return result;
}

bool DocumentReadiness::hasDiagnostic(std::string_view code) const noexcept {
    return std::any_of(
        diagnostics.begin(),
        diagnostics.end(),
        [code](const DocumentDiagnostic &diagnostic) {
            return diagnostic.code == code;
        });
}

std::vector<DocumentDiagnostic> inspectFrogDraftInterfaceBindings(
    const std::optional<FrogDraftInterface> &declaration,
    FrogWidgetDataView canvas, std::span<const DiagramNode> diagramNodes,
    const FrogSourceProperties &properties,
    const ConditionalDisableCompilationContext &compilationContext) {
    DocumentReadiness inspection;
    const auto activeNodes = activeConditionalDisableDiagramNodes(diagramNodes, properties, compilationContext);
    validatePublicInterface(canvas, inspection);
    validateDeclaredDraftInterface(declaration, canvas, activeNodes, inspection);
    // Keep the ordinary readiness loop's first unique, nonempty node ID. Its
    // structural failures are independent from this narrow contract result.
    std::unordered_set<std::string_view> ids;
    for (const auto *node : activeNodes) {
        if (node->id.empty() || !ids.insert(node->id).second) continue;
        if (node->kind == "frog_call") validateFrogCallPortIdentities(*node, inspection);
    }
    std::erase_if(inspection.diagnostics, [](const auto &diagnostic) {
        return diagnostic.domain != DocumentDiagnosticDomain::InterfaceContract;
    });
    // Interface-contract diagnostics locate authored node/widget aliases,
    // not wire/fragment subjects. Use the same locator/order as readiness.
    locateDraftDiagnostics(diagramNodes, {}, {}, inspection);
    return std::move(inspection.diagnostics);
}

void locateDraftDiagnostics(std::span<const DiagramNode> diagramNodes,
    std::span<const DiagramEdge> diagramEdges, std::span<const DiagramWireFragment> diagramWireFragments,
    DocumentReadiness &out) {
    // Borrow identifiers for this synchronous, read-only analysis. Retain all
    // duplicate candidates and their document order, including widget aliases.
    using NodeCandidates=std::unordered_map<std::string_view,std::vector<std::size_t>>;
    NodeCandidates nodeIds,widgetIds;
    nodeIds.reserve(diagramNodes.size());
    for(std::size_t i=0;i<diagramNodes.size();++i) {
        const auto &n=diagramNodes[i];
        nodeIds[n.id].push_back(i);
        if(n.kind=="widget_value") widgetIds[n.widgetId].push_back(i);
    }
    std::unordered_map<std::string_view,const DiagramEdge*> edges;
    std::unordered_map<std::string_view,const DiagramWireFragment*> fragments;
    edges.reserve(diagramEdges.size());fragments.reserve(diagramWireFragments.size());
    for(const auto &e:diagramEdges) edges.emplace(e.id,&e); // First duplicate wins.
    for(const auto &f:diagramWireFragments) fragments.insert_or_assign(f.id,&f); // Last wins.
    std::vector<std::size_t> candidates;candidates.reserve(8);
    for(auto &diag:out.diagnostics) {
        candidates.clear();
        const std::string_view subject=diag.subjectId;
        const auto append=[&](const NodeCandidates &index,std::string_view id) {
            if(const auto found=index.find(id);found!=index.end())
                candidates.insert(candidates.end(),found->second.begin(),found->second.end());
        };
        append(nodeIds,subject);append(widgetIds,subject);
        for(auto colon=subject.find(':');colon!=std::string_view::npos;colon=subject.find(':',colon+1))
            append(nodeIds,subject.substr(0,colon));
        std::sort(candidates.begin(),candidates.end());
        candidates.erase(std::unique(candidates.begin(),candidates.end()),candidates.end());
        const DiagramNode *node=nullptr;
        // Preserve the established longest-prefix and widget-alias precedence.
        for(const auto index:candidates) {
            const auto &n=diagramNodes[index];
            if(diag.subjectId==n.id || diag.subjectId.starts_with(n.id+":"))
                if(!node || n.id.size()>node->id.size()) node=&n;
            if(n.kind=="widget_value" && n.widgetId==diag.subjectId) node=&n;
        }
        const bool wireDiagnostic=!diag.edgeId.empty() || diag.code=="invalid_diagram_edge_ports" ||
            diag.code=="incompatible_wire_types" || diag.code=="dangling_diagram_edge" ||
            diag.code=="multiple_sources_for_sink" || diag.code=="duplicate_diagram_edge_id" ||
            diag.code=="empty_diagram_edge_id" || diag.code=="structure.boundary_scope" ||
            diag.code=="sequence.backward_read" || diag.code=="conditional_disable.dangling_source_wire";
        if(const auto found=edges.find(subject);wireDiagnostic && found!=edges.end()) {
            const auto &e=*found->second;
            diag.edgeId=e.id;diag.ownerStructureId=e.diagramOwnerStructureId;diag.regionId=e.diagramOwnerRegionId;
            diag.nodeId=e.to.nodeId;node=nullptr;
        }
        if(const auto found=fragments.find(subject);found!=fragments.end() &&
            (diag.code=="incomplete_wire_fragment" || diag.code=="duplicate_wire_fragment_id" || diag.code=="empty_wire_fragment_id")) {
            const auto &f=*found->second;
            diag.edgeId=f.id;diag.ownerStructureId=f.diagramOwnerStructureId;diag.regionId=f.diagramOwnerRegionId;
            node=nullptr;
        }
        if(node) {
            diag.nodeId=node->id;
            if(diag.subjectId.starts_with(node->id+":")) diag.portId=diag.subjectId.substr(node->id.size()+1);
            if(diag.ownerStructureId.empty()) {diag.ownerStructureId=node->diagramOwnerStructureId;diag.regionId=node->diagramOwnerRegionId;}
        }
    }
    std::stable_sort(out.diagnostics.begin(),out.diagnostics.end(),[](const auto &a,const auto &b){
        if(a.severity!=b.severity)return a.severity>b.severity;
        return std::tie(a.subjectId,a.code,a.regionId)<std::tie(b.subjectId,b.code,b.regionId);
    });
}
}
