use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::ButtonBrowserUiRuntime;

struct LatchSpec {
    number: u8,
    example_id: &'static str,
    action: &'static str,
    title: &'static str,
    execution_path: &'static str,
    requires_release_binding: bool,
}

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn specs() -> Vec<LatchSpec> {
    vec![
        LatchSpec {
            number: 13,
            example_id: "13_button_latch_when_pressed",
            action: "latch_when_pressed",
            title: "Button Latch When Pressed",
            execution_path: "rust_button_latch_when_pressed_contract_executor",
            requires_release_binding: false,
        },
        LatchSpec {
            number: 14,
            example_id: "14_button_latch_when_released",
            action: "latch_when_released",
            title: "Button Latch When Released",
            execution_path: "rust_button_latch_when_released_contract_executor",
            requires_release_binding: true,
        },
        LatchSpec {
            number: 15,
            example_id: "15_button_latch_until_released",
            action: "latch_until_released",
            title: "Button Latch Until Released",
            execution_path: "rust_button_latch_until_released_contract_executor",
            requires_release_binding: true,
        },
    ]
}

fn contract_path(spec: &LatchSpec) -> PathBuf {
    repo_root().join(format!(
        "Implementations/Reference/ContractEmitter/examples/{}.reference_host_runtime_ui_binding.contract.json",
        spec.example_id
    ))
}

fn wfrog_path(spec: &LatchSpec) -> PathBuf {
    repo_root().join(format!("Examples/{}/ui/button_panel.wfrog", spec.example_id))
}

fn runtime(spec: &LatchSpec) -> ButtonBrowserUiRuntime {
    ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path(spec), wfrog_path(spec), None).expect("runtime")
}

#[test]
fn rust_latch_examples_consume_source_and_default_assets() {
    for spec in specs() {
        let runtime = runtime(&spec);
        let artifact = runtime.execution_artifact().expect("artifact");
        assert_eq!(artifact["contract_ref"]["source_ref"]["example_id"].as_str(), Some(spec.example_id));
        assert_eq!(artifact["ui_runtime"]["panel"]["title"].as_str(), Some(spec.title));

        let wfrog = std::fs::read_to_string(wfrog_path(&spec)).expect("wfrog");
        assert!(wfrog.contains("Libraries/Realizations/Default/button.default.wfrog"));
        assert!(wfrog.contains("Libraries/Realizations/Default/boolean.default.wfrog"));
        assert!(wfrog.contains("assets/button/templates/button_rectangular.svg"));
        assert!(wfrog.contains("assets/boolean/templates/boolean_circular.svg"));
        assert!(wfrog.contains("button_latch_reset_on_value_consumption"));
        assert_eq!(wfrog.contains("button_release_binding"), spec.requires_release_binding);

        let button_asset = runtime.asset_map.get("button_rectangular_svg").expect("button asset");
        assert!(button_asset.exists());
        assert!(button_asset.to_string_lossy().contains("Libraries"));
        assert!(button_asset.to_string_lossy().contains("Default"));
    }
}

#[test]
fn rust_latch_examples_follow_press_release_read_contract() {
    for spec in specs() {
        let mut runtime = runtime(&spec);
        let artifact = runtime.execution_artifact().expect("initial");
        assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
        assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
        assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));

        if spec.action == "latch_when_pressed" {
            let artifact = runtime.press_control().expect("press");
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.read_program_value().expect("read");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.read_program_value().expect("read false");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));
        } else if spec.action == "latch_when_released" {
            let artifact = runtime.press_control().expect("press");
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));

            let artifact = runtime.release_control().expect("release");
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.read_program_value().expect("read");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));
        } else {
            let artifact = runtime.press_control().expect("press");
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.read_program_value().expect("read while pressed");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.release_control().expect("release after read");
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));
        }
    }
}

#[test]
fn rust_latch_browser_surface_is_svg_and_source_owned() {
    for spec in specs() {
        let html = runtime(&spec).render_html();
        assert!(html.contains(spec.title));
        assert!(html.contains(&format!("Example {} - .frog {} Button value", spec.number, spec.action)));
        assert!(html.contains("Rust reference runtime"));
        assert!(html.contains("button latch contract executor"));
        assert!(html.contains(&format!("data-execution-path='{}'", spec.execution_path)));
        assert!(html.contains("data-widget-id='trigger_button'"));
        assert!(html.contains("data-widget-id='latched_indicator'"));
        assert!(html.contains("data-class-ref='frog.widgets.button'"));
        assert!(html.contains("data-class-ref='frog.widgets.boolean_indicator'"));
        assert!(html.contains("data-asset-route='/asset/button_rectangular_svg'"));
        assert!(html.contains("data-asset-route='/asset/boolean_circular_svg'"));
        assert!(html.contains("data-frog-template=\"frog.realizations.default.button.rectangular\""));
        assert!(html.contains("data-frog-asset-consumed='true'"));
        assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
        assert!(html.contains(&format!("data-frog-mechanical-action='{}'", spec.action)));
        assert!(html.contains("data-frog-output-pulse-duration-ms='220'"));
        assert!(html.contains("data-frog-pressed-applies-when-value-true='true'"));
        assert!(html.contains("data-frog-pressed-applies-while-active='false'"));
        assert!(html.contains("data-frog-hover-applies-when-value-false-only='false'"));
        assert!(html.contains("data-frog-state-text-false='OFF'"));
        assert!(html.contains("data-frog-state-text-true='ON'"));
        assert!(html.contains("data-frog-button-face-fill-false='#e2e8f0'"));
        assert!(html.contains("data-frog-button-face-fill-true='#cbd5e1'"));
        assert!(html.contains("data-frog-button-face-hover-fill-false='#f1f5f9'"));
        assert!(html.contains("data-frog-button-face-hover-fill-true='#dbeafe'"));
        assert!(html.contains("--frog-button-pressed-inset:0px;"));
        assert!(html.contains("publishEvent(\"press\")"));
        assert!(html.contains("publishEvent(\"release\")"));
        assert!(html.contains("publishEvent(\"read\")"));
        assert!(html.contains("const latchPulseVisible = latchAction"));
        assert!(html.contains("applyButton(latchPulseVisible ? true : buttonValue"));
        assert!(html.contains("buttonWidget.style.setProperty(\"--frog-button-face-fill\", buttonProperty(\"frogButtonFaceFill\", value));"));
        assert!(html.contains("program-read-action"));
        assert!(html.contains(">OFF</span>"));
        assert!(html.contains(">FALSE</span>"));
        assert!(!html.contains("fallback"));
        assert!(!html.contains("type='checkbox'"));
        assert!(!html.contains("type='submit'"));
    }
}

#[test]
fn rust_latch_browser_event_semantics() {
    for spec in specs() {
        let mut runtime = runtime(&spec);

        if spec.action == "latch_when_pressed" {
            let artifact = runtime.apply_event("press").expect("press event");
            assert_eq!(artifact["execution_summary"]["program_read_performed"].as_bool(), Some(true));
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(true));
            assert_eq!(artifact["execution_summary"]["button_physical_pressed"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.apply_event("release").expect("release event");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(false));
            assert_eq!(artifact["execution_summary"]["button_physical_pressed"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));
        } else if spec.action == "latch_when_released" {
            let artifact = runtime.apply_event("press").expect("press event");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(false));
            assert_eq!(artifact["execution_summary"]["button_physical_pressed"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));

            let artifact = runtime.apply_event("release").expect("release event");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(true));
            assert_eq!(artifact["execution_summary"]["button_physical_pressed"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));
        } else {
            let artifact = runtime.apply_event("press").expect("press event");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(true));
            assert_eq!(artifact["execution_summary"]["button_physical_pressed"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(true));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(true));

            let artifact = runtime.apply_event("release").expect("release event");
            assert_eq!(artifact["execution_summary"]["program_read_value"].as_bool(), Some(false));
            assert_eq!(artifact["execution_summary"]["button_physical_pressed"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["public"]["latched"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
            assert_eq!(artifact["outputs"]["ui"]["latched_indicator"].as_bool(), Some(false));
        }
    }
}
