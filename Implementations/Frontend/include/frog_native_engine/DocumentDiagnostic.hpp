#pragma once

#include <string>
#include <string_view>

namespace frog::native_engine {

enum class DocumentDiagnosticSeverity { Information, Warning, Error };

// Existing categories retain their values. These are diagnostic causes, not
// verdicts inferred from an English message or from an installed backend.
enum class DocumentDiagnosticDomain {
    Format, Structure, Semantics, InterfaceContract, Topology, RuntimeTarget,
    Integrity, Compatibility, Dependency, FrontendCapability, Control
};

enum class DocumentDiagnosticStage {
    Format, Validation, Adaptation, Fir, Capability,
    Read, Resolve, Compilation, Deployment, Execution,
    Compatibility = Format
};

enum class DocumentValidationStatus { Valid, Invalid, Indeterminate };

[[nodiscard]] constexpr bool documentDiagnosticInvalidatesSource(DocumentDiagnosticDomain domain) noexcept {
    return domain==DocumentDiagnosticDomain::Integrity||domain==DocumentDiagnosticDomain::Structure||
        domain==DocumentDiagnosticDomain::Semantics||domain==DocumentDiagnosticDomain::InterfaceContract||
        domain==DocumentDiagnosticDomain::Topology;
}

struct DocumentDiagnostic {
    DocumentDiagnosticSeverity severity = DocumentDiagnosticSeverity::Error;
    DocumentDiagnosticDomain domain = DocumentDiagnosticDomain::Structure;
    std::string code;
    std::string subjectId;
    std::string message;
    bool blocksBuild = true;
    bool blocksRun = true;
    // Technical identities, never labels, positions or a visible-case guess.
    std::string nodeId, edgeId, ownerStructureId, regionId, portId;
    DocumentDiagnosticStage stage = DocumentDiagnosticStage::Validation;
    std::string sourceUri, sourcePath, widgetId;
    std::string capabilityId, providerId, targetId, backendId;
    std::string relatedSourceUri, relatedSourcePath, callsiteId;
};

[[nodiscard]] constexpr std::string_view documentDiagnosticDomainId(DocumentDiagnosticDomain domain) noexcept {
    switch(domain) {
    case DocumentDiagnosticDomain::Format:return "format";
    case DocumentDiagnosticDomain::Structure:return "structure";
    case DocumentDiagnosticDomain::Semantics:return "semantics";
    case DocumentDiagnosticDomain::InterfaceContract:return "interface";
    case DocumentDiagnosticDomain::Topology:return "topology";
    case DocumentDiagnosticDomain::RuntimeTarget:return "target-capability";
    case DocumentDiagnosticDomain::Integrity:return "integrity";
    case DocumentDiagnosticDomain::Compatibility:return "compatibility";
    case DocumentDiagnosticDomain::Dependency:return "dependency";
    case DocumentDiagnosticDomain::FrontendCapability:return "frontend-capability";
    case DocumentDiagnosticDomain::Control:return "control";
    }
    return {};
}
}
