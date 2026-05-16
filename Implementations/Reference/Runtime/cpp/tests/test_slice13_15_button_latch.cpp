#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

namespace {

struct LatchSpec {
    int number;
    std::string example_id;
    std::string unit_kind;
    std::string action;
    std::string title;
    std::string execution_path;
    bool requires_release_binding;
};

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

std::filesystem::path contract_path(const LatchSpec& spec) {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           (spec.example_id + ".reference_host_runtime_ui_binding.contract.json");
}

std::filesystem::path wfrog_path(const LatchSpec& spec) {
    return repo_root() / "Examples" / spec.example_id / "ui" / "button_panel.wfrog";
}

std::string read_text(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    assert(input);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

void assert_not_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) == std::string::npos);
}

bool public_output_value(const frog::json::Value& artifact, const std::string& id) {
    return artifact.as_object().at("outputs").as_object().at("public").as_object().at(id).as_bool();
}

bool ui_value(const frog::json::Value& artifact, const std::string& id) {
    return artifact.as_object().at("outputs").as_object().at("ui").as_object().at(id).as_bool();
}

bool summary_bool(const frog::json::Value& artifact, const std::string& id) {
    return artifact.as_object().at("execution_summary").as_object().at(id).as_bool();
}

std::string diagram_title_for_action(const std::string& action) {
    if (action == "latch_when_released") {
        return "Latch when released";
    }
    if (action == "latch_until_released") {
        return "Latch until released";
    }
    return "Latch when pressed";
}

std::vector<LatchSpec> specs() {
    return {
        {13, "13_button_latch_when_pressed", "button_latch_when_pressed_ui_unit", "latch_when_pressed", "Button Latch When Pressed", "cpp_button_latch_when_pressed_contract_executor", false},
        {14, "14_button_latch_when_released", "button_latch_when_released_ui_unit", "latch_when_released", "Button Latch When Released", "cpp_button_latch_when_released_contract_executor", true},
        {15, "15_button_latch_until_released", "button_latch_until_released_ui_unit", "latch_until_released", "Button Latch Until Released", "cpp_button_latch_until_released_contract_executor", true},
    };
}

void test_latch_runtime_core_consumes_source_and_default_assets(const LatchSpec& spec) {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(spec), wfrog_path(spec));

    assert(runtime.contract.source_ref.example_id == spec.example_id);
    assert(runtime.unit.kind == spec.unit_kind);
    assert(runtime.unit.public_interface.inputs.front().id == "trigger_value");
    assert(runtime.unit.public_interface.inputs.front().binding_origin.value() == "widget.trigger_button.value");
    assert(runtime.unit.public_interface.outputs.front().id == "latched");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.panel.title == spec.title);
    assert(runtime.package.kind == "widget_realization_package");

    assert(runtime.widgets.at("trigger_button").class_ref == "frog.widgets.button");
    assert(runtime.widgets.at("latched_indicator").class_ref == "frog.widgets.boolean_indicator");
    assert(runtime.widgets.at("trigger_button").asset_id == "button_rectangular_svg");
    assert(runtime.widgets.at("latched_indicator").asset_id == "boolean_circular_svg");
    assert(runtime.widgets.at("trigger_button").properties.at("binding.public_input_id").as_string() == "trigger_value");
    assert(runtime.widgets.at("latched_indicator").properties.at("binding.public_output_id").as_string() == "latched");
    assert(runtime.widgets.at("trigger_button").properties.at("behavior.mechanical_action").as_string() == spec.action);
    assert(runtime.widgets.at("trigger_button").properties.at("behavior.latch_reset_policy").as_string() == "reset_on_natural_value_consumption");
    assert(runtime.widgets.at("trigger_button").properties.at("behavior.output_pulse.duration_ms").as_i64() == 220);
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.false_text").as_string() == "OFF");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.true_text").as_string() == "ON");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.style.font_weight").as_string() == "400");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.fill_color.false").as_string() == "#e2e8f0");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.fill_color.true").as_string() == "#cbd5e1");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.fill_color.hover_false").as_string() == "#f1f5f9");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.fill_color.hover_true").as_string() == "#dbeafe");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.border_width").as_string() == "1px");
    assert(runtime.widgets.at("trigger_button").properties.at("style.pressed.inset").as_string() == "0px");
    assert(runtime.widgets.at("trigger_button").properties.at("style.pressed.apply_when_value_true").as_bool());
    assert(!runtime.widgets.at("trigger_button").properties.at("style.pressed.apply_while_active").as_bool());
    assert(!runtime.widgets.at("trigger_button").properties.at("style.hover.apply_when_value_false_only").as_bool());
    assert(runtime.widgets.at("latched_indicator").properties.at("interaction.read_only").as_bool());
    assert(std::filesystem::exists(runtime.asset_map.at("button_rectangular_svg")));
    assert(std::filesystem::exists(runtime.asset_map.at("boolean_circular_svg")));
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Libraries") != std::string::npos);
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Default") != std::string::npos);

    const auto wfrog = read_text(wfrog_path(spec));
    assert_contains(wfrog, "Libraries/Realizations/Default/button.default.wfrog");
    assert_contains(wfrog, "Libraries/Realizations/Default/boolean.default.wfrog");
    assert_contains(wfrog, "assets/button/templates/button_rectangular.svg");
    assert_contains(wfrog, "assets/boolean/templates/boolean_circular.svg");
    assert_contains(wfrog, "button_latch_reset_on_value_consumption");
    if (spec.requires_release_binding) {
        assert_contains(wfrog, "button_release_binding");
    } else {
        assert_not_contains(wfrog, "button_release_binding");
    }

    const auto button_svg = read_text(runtime.asset_map.at("button_rectangular_svg"));
    assert_contains(button_svg, "data-frog-template=\"frog.realizations.default.button.rectangular\"");
    assert_contains(button_svg, "data-frog-target-class=\"frog.widgets.button\"");
    assert_contains(button_svg, "data-frog-part=\"face\"");
    assert_contains(button_svg, "data-frog-part=\"state_face\"");
    assert_contains(button_svg, "data-frog-anchor=\"face.bounds\"");
    assert_contains(button_svg, "data-frog-bind=\"style.face.*\"");
}

void test_headless_latch_roundtrip(const LatchSpec& spec) {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(spec), wfrog_path(spec));

    auto artifact = runtime.execution_artifact();
    assert(!public_output_value(artifact, "latched"));
    assert(!ui_value(artifact, "trigger_button"));
    assert(!ui_value(artifact, "latched_indicator"));
    assert(artifact.as_object().at("execution_summary").as_object().at("mode").as_string() == spec.unit_kind.substr(0, spec.unit_kind.size() - std::string("_ui_unit").size()));

    if (spec.action == "latch_when_pressed") {
        artifact = runtime.press_control();
        assert(summary_bool(artifact, "trigger_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.release_control();
        assert(!summary_bool(artifact, "trigger_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "latched_indicator"));
    } else if (spec.action == "latch_when_released") {
        artifact = runtime.press_control();
        assert(summary_bool(artifact, "trigger_pressed"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(!ui_value(artifact, "latched_indicator"));

        artifact = runtime.release_control();
        assert(!summary_bool(artifact, "trigger_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "latched_indicator"));
    } else {
        artifact = runtime.press_control();
        assert(summary_bool(artifact, "trigger_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.release_control();
        assert(!summary_bool(artifact, "trigger_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "latched_indicator"));

        frog::runtime::Slice10ButtonRuntimeCore read_before_release(contract_path(spec), wfrog_path(spec));
        artifact = read_before_release.press_control();
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = read_before_release.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = read_before_release.release_control();
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(!ui_value(artifact, "latched_indicator"));

        artifact = read_before_release.read_program_value();
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "latched_indicator"));
    }
}

void test_latch_browser_ui_surface(const LatchSpec& spec) {
    frog::runtime::ButtonBrowserUiRuntime runtime(contract_path(spec), wfrog_path(spec));
    std::string html = runtime.render_html();

    assert_contains(html, spec.title);
    assert_contains(html, "Example " + std::to_string(spec.number) + " - .frog " + spec.action + " Button value");
    assert_contains(html, "button latch contract executor");
    assert_contains(html, "none for Example " + std::to_string(spec.number));
    assert_contains(html, "data-execution-path='" + spec.execution_path + "'");
    assert_contains(html, "data-widget-id='trigger_button'");
    assert_contains(html, "data-widget-id='latched_indicator'");
    assert_contains(html, "data-class-ref='frog.widgets.button'");
    assert_contains(html, "data-class-ref='frog.widgets.boolean_indicator'");
    assert_contains(html, "data-asset-route='/asset/button_rectangular_svg'");
    assert_contains(html, "data-asset-route='/asset/boolean_circular_svg'");
    assert_contains(html, "data-frog-asset-consumed='true'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "data-frog-mechanical-action='" + spec.action + "'");
    assert_contains(html, "data-frog-physical-pressed='false'");
    assert_contains(html, "data-frog-output-pulse-duration-ms='220'");
    assert_contains(html, "data-frog-pressed-applies-when-value-true='true'");
    assert_contains(html, "data-frog-pressed-applies-while-active='false'");
    assert_contains(html, "data-frog-hover-applies-when-value-false-only='false'");
    assert_contains(html, "data-frog-state-text-false='OFF'");
    assert_contains(html, "data-frog-state-text-true='ON'");
    assert_contains(html, "data-frog-button-face-fill-false='#e2e8f0'");
    assert_contains(html, "data-frog-button-face-fill-true='#cbd5e1'");
    assert_contains(html, "data-frog-button-face-hover-fill-false='#f1f5f9'");
    assert_contains(html, "data-frog-button-face-hover-fill-true='#dbeafe'");
    assert_contains(html, "data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_value'");
    assert_contains(html, "data-frog-host-overlay='input' data-frog-align-to-part='face'");
    assert_contains(html, "name='trigger_value' value='true'");
    assert_contains(html, "aria-pressed='false'");
    assert_contains(html, "--frog-button-face-fill:#e2e8f0;");
    assert_contains(html, "--frog-button-face-hover-fill:#f1f5f9;");
    assert_contains(html, "--frog-button-face-pressed-fill:#e2e8f0;");
    assert_contains(html, "--frog-button-pressed-inset:0px;");
    assert_contains(html, "mechanicalAction !== \"" + spec.action + "\"");
    assert_contains(html, "publishEvent(\"press\")");
    assert_contains(html, "publishEvent(\"release\")");
    assert_contains(html, "publishEvent(\"read\")");
    assert_contains(html, "pulseDurationMs");
    assert_contains(html, "const latchPulseVisible = latchAction");
    assert_contains(html, "applyButton(latchPulseVisible ? true : buttonValue");
    assert_contains(html, "const shouldResetLatchPulse = latchPulseVisible");
    assert_contains(html, "mechanicalAction === \"latch_when_pressed\" || !summary.button_physical_pressed");
    assert_contains(html, "buttonWidget.style.setProperty(\"--frog-button-face-fill\", buttonProperty(\"frogButtonFaceFill\", value));");
    assert_contains(html, "program-read-action");
    assert_contains(html, diagram_title_for_action(spec.action));
    assert_contains(html, "fetch(\"/event\"");
    assert_contains(html, ".button-state-overlay{position:absolute;transform:translate(-50%,-50%);font-size:var(--frog-button-state-text-font-size);font-weight:var(--frog-button-state-text-font-weight);line-height:1;color:var(--frog-button-state-text-fill);pointer-events:none;z-index:6;");
    assert_contains(html, ">OFF</span>");
    assert_contains(html, ">FALSE</span>");
    assert_not_contains(html, "fallback");
    assert_not_contains(html, "type='checkbox'");
    assert_not_contains(html, "type='submit'");

    runtime.core.press_control();
    if (spec.action == "latch_when_released") {
        runtime.core.release_control();
    }
    html = runtime.render_html();
    if (spec.action == "latch_when_released") {
        assert_contains(html, "data-frog-physical-pressed='false'");
        assert_contains(html, "aria-pressed='false'");
    } else {
        assert_contains(html, "data-frog-physical-pressed='true'");
        assert_contains(html, "aria-pressed='true'");
    }
    assert_contains(html, "--frog-button-face-fill:#cbd5e1;");
    assert_contains(html, "--frog-button-face-hover-fill:#dbeafe;");
    assert_contains(html, "--frog-button-face-pressed-fill:#cbd5e1;");
    assert_contains(html, ">ON</span>");
    assert_contains(html, ">TRUE</span>");

    if (spec.action == "latch_when_pressed" || spec.action == "latch_until_released") {
        runtime.core.release_control();
    }
    runtime.core.read_program_value();
    html = runtime.render_html();
    assert_contains(html, "aria-pressed='false'");
    assert_contains(html, ">OFF</span>");
    assert_contains(html, ">TRUE</span>");
}

void test_latch_browser_event_semantics(const LatchSpec& spec) {
    frog::runtime::ButtonBrowserUiRuntime runtime(contract_path(spec), wfrog_path(spec));

    if (spec.action == "latch_when_pressed") {
        auto artifact = runtime.apply_event("press");
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(summary_bool(artifact, "button_physical_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.apply_event("release");
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(!summary_bool(artifact, "button_physical_pressed"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(!ui_value(artifact, "latched_indicator"));
        return;
    }

    if (spec.action == "latch_when_released") {
        auto artifact = runtime.apply_event("press");
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(summary_bool(artifact, "button_physical_pressed"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(!ui_value(artifact, "latched_indicator"));

        artifact = runtime.apply_event("release");
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(!summary_bool(artifact, "button_physical_pressed"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = runtime.apply_event("read");
        assert(summary_bool(artifact, "program_read_performed"));
        assert(!summary_bool(artifact, "program_read_value"));
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(!ui_value(artifact, "latched_indicator"));
        return;
    }

    auto artifact = runtime.apply_event("press");
    assert(summary_bool(artifact, "program_read_performed"));
    assert(summary_bool(artifact, "program_read_value"));
    assert(summary_bool(artifact, "button_physical_pressed"));
    assert(public_output_value(artifact, "latched"));
    assert(ui_value(artifact, "trigger_button"));
    assert(ui_value(artifact, "latched_indicator"));

    artifact = runtime.apply_event("release");
    assert(summary_bool(artifact, "program_read_performed"));
    assert(!summary_bool(artifact, "program_read_value"));
    assert(!summary_bool(artifact, "button_physical_pressed"));
    assert(!public_output_value(artifact, "latched"));
    assert(!ui_value(artifact, "trigger_button"));
    assert(!ui_value(artifact, "latched_indicator"));
}

} // namespace

int main() {
    for (const auto& spec : specs()) {
        test_latch_runtime_core_consumes_source_and_default_assets(spec);
        test_headless_latch_roundtrip(spec);
        test_latch_browser_ui_surface(spec);
        test_latch_browser_event_semantics(spec);
    }
    std::cout << "slice13-15 Button latch C++ runtime acceptance passed" << std::endl;
    return 0;
}
