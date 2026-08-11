#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "json.hpp"

namespace frog::runtime {

inline constexpr const char* REFERENCE_BACKEND_FAMILY = "reference_host_runtime_ui_binding";
inline constexpr const char* EXPECTED_OVERFLOW_BEHAVIOR = "reject_execution_on_u16_overflow";

struct ArtifactReference {
    std::string path;
};

struct SourceRef {
    std::string example_id;
    std::string path;
    std::string entry_unit;
};

struct UiBindingAssumptions {
    bool widget_value_binding = false;
    bool widget_reference_binding = false;
};

struct RuntimeFamilyAssumptions {
    std::string name;
    std::string host_model;
    UiBindingAssumptions ui_binding;
};

struct NumericBehaviorAssumptions {
    std::string value_domain;
    std::string overflow_behavior;
};

struct ContractAssumptions {
    RuntimeFamilyAssumptions runtime_family;
    NumericBehaviorAssumptions numeric_behavior;
};

struct InterfacePort {
    std::string id;
    std::string port_type;
    std::optional<std::string> binding_origin;
    std::optional<std::string> binding_target;
};

struct PublicInterface {
    std::vector<InterfacePort> inputs;
    std::vector<InterfacePort> outputs;
};

struct WidgetBindingMode {
    std::string mode;
    std::optional<std::string> public_input_id;
    std::optional<std::string> public_output_id;
};

struct WidgetBinding {
    std::string widget_id;
    std::string widget_class;
    std::string value_type;
    std::string role;
    WidgetBindingMode binding;
};

struct WidgetReferenceSupport {
    std::string widget_id;
    std::vector<std::string> supported_members;
};

struct ContractUiBinding {
    std::vector<std::string> package_refs;
    std::vector<WidgetBinding> widgets;
    std::vector<WidgetReferenceSupport> widget_reference_support;
};

struct StateCarrier {
    std::string primitive;
    std::string state_id;
    std::string state_type;
    std::uint16_t initial_value;
};

struct StateModel {
    bool explicit_state;
    StateCarrier carrier;
    std::string commit_rule;
};

struct BodyRule {
    std::string kind;
    std::string expression;
};

struct ExecutionModel {
    std::string structure;
    std::uint32_t iteration_count;
    std::optional<std::string> iteration_variable;
    BodyRule body_rule;
    std::string final_result_rule;
};

struct PropertyWriteValue {
    std::string value_type;
    std::string value;
};

struct PropertyWrite {
    std::string operation;
    std::string widget_id;
    std::string member;
    PropertyWriteValue value;
};

struct PublicOutputPublication {
    std::string output_id;
    std::string source;
};

struct ContractUnit {
    std::string unit_id;
    std::string kind;
    PublicInterface public_interface;
    ContractUiBinding ui_binding;
    StateModel state_model;
    ExecutionModel execution_model;
    std::vector<PropertyWrite> property_writes;
    PublicOutputPublication public_output_publication;
};

struct BackendContract {
    std::string artifact_kind;
    std::optional<ArtifactReference> artifact_governance_ref;
    std::string backend_family;
    SourceRef source_ref;
    ContractAssumptions assumptions;
    std::vector<ContractUnit> units;
};

struct WidgetProperty {
    std::string name;
};

struct WidgetMethod {
    std::string name;
};

struct WidgetClass {
    std::string class_id;
    std::vector<WidgetProperty> properties;
    std::vector<WidgetMethod> methods;
};

struct SvgAsset {
    std::string asset_id;
    std::string path;
    std::string kind;
};

struct HostBinding {
    std::string binding_id;
    std::string target;
    std::vector<std::string> required_capabilities;
    std::vector<std::string> optional_capabilities;
};

struct PanelWidget {
    std::string instance_id;
    std::string class_ref;
    frog::json::Value layout;
    frog::json::Object props;
    frog::json::Object visual;
};

struct FrontPanel {
    std::string panel_id;
    std::string title;
    std::string class_ref;
    frog::json::Value layout;
    std::vector<PanelWidget> widgets;
    std::string host_binding_ref;
};

struct WfrogPackage {
    std::string format;
    std::string kind;
    std::vector<WidgetClass> widget_classes;
    std::vector<SvgAsset> svg_assets;
    std::vector<HostBinding> host_bindings;
    std::vector<FrontPanel> front_panels;
};

std::filesystem::path find_repo_root(const std::filesystem::path& start);
std::filesystem::path default_contract_path();
std::filesystem::path default_wfrog_path();

BackendContract load_contract_from_path(const std::filesystem::path& path);
WfrogPackage load_wfrog_from_path(const std::filesystem::path& path);
FrontPanel load_front_panel_from_frog_source_path(const std::filesystem::path& path);

} // namespace frog::runtime
