use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::ButtonBrowserUiRuntime;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn contract_path() -> PathBuf {
    repo_root().join(
        "Implementations/Reference/ContractEmitter/examples/11_button_switch_when_pressed.reference_host_runtime_ui_binding.contract.json",
    )
}

fn wfrog_path() -> PathBuf {
    repo_root().join("Examples/11_button_switch_when_pressed/ui/button_panel.wfrog")
}

#[test]
fn rust_example11_headless_button_switch_when_pressed() {
    let mut runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path(), wfrog_path(), None).expect("runtime");

    let artifact = runtime.execution_artifact().expect("initial artifact");
    assert_eq!(artifact["contract_ref"]["source_ref"]["example_id"].as_str(), Some("11_button_switch_when_pressed"));
    assert_eq!(artifact["outputs"]["public"]["switched"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["switched_indicator"].as_bool(), Some(false));

    let artifact = runtime.run_once(true).expect("switch true");
    assert_eq!(artifact["execution_summary"]["mode"].as_str(), Some("button_switch_when_pressed"));
    assert_eq!(artifact["execution_summary"]["trigger_pressed"].as_bool(), Some(true));
    assert_eq!(artifact["outputs"]["public"]["switched"].as_bool(), Some(true));
    assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(true));
    assert_eq!(artifact["outputs"]["ui"]["switched_indicator"].as_bool(), Some(true));

    let artifact = runtime.run_once(false).expect("switch false");
    assert_eq!(artifact["execution_summary"]["trigger_pressed"].as_bool(), Some(true));
    assert_eq!(artifact["outputs"]["public"]["switched"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["switched_indicator"].as_bool(), Some(false));
}

#[test]
fn rust_example11_browser_ui_consumes_default_svg_and_source_styles() {
    let mut runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path(), wfrog_path(), None).expect("runtime");
    let html = runtime.render_html();

    assert!(html.contains("Button Switch When Pressed"));
    assert!(html.contains("Example 11 - .frog switch_when_pressed Button value"));
    assert!(html.contains("Rust reference runtime"));
    assert!(html.contains("button switch contract executor"));
    assert!(html.contains("data-execution-path='rust_button_switch_when_pressed_contract_executor'"));
    assert!(html.contains("data-widget-id='trigger_button'"));
    assert!(html.contains("data-widget-id='switched_indicator'"));
    assert!(!html.contains("data-widget-id='pressed_indicator'"));
    assert!(html.contains("data-class-ref='frog.widgets.button'"));
    assert!(html.contains("data-class-ref='frog.widgets.boolean_indicator'"));
    assert!(html.contains("data-asset-route='/asset/button_rectangular_svg'"));
    assert!(html.contains("data-asset-route='/asset/boolean_circular_svg'"));
    assert!(html.contains("data-frog-template=\"frog.realizations.default.button.rectangular\""));
    assert!(html.contains("data-frog-asset-consumed='true'"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("data-frog-mechanical-action='switch_when_pressed'"));
    assert!(html.contains("data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_value'"));
    assert!(html.contains("name='trigger_value' value='true'"));
    assert!(html.contains("--frog-button-face-stroke-width:1px;"));
    assert!(html.contains("--frog-button-state-text-font-weight:400;"));
    assert!(html.contains("--boolean-text-font-weight:400;"));
    assert!(html.contains(".boolean-indicator[data-class-ref='frog.widgets.boolean_indicator']"));
    assert!(html.contains("mechanicalAction === \"switch_when_pressed\""));
    assert!(html.contains("setPressed(!(buttonWidget.dataset.currentValue === \"true\"));"));
    assert!(html.contains("fetch(\"/event\""));
    assert!(html.contains("pointerdown"));
    assert!(html.contains(">OFF</span>"));
    assert!(html.contains(">FALSE</span>"));
    assert!(!html.contains("fallback"));
    assert!(!html.contains("type='checkbox'"));
    assert!(!html.contains("type='submit'"));
    assert!(!html.contains("Current runtime snapshot"));
    assert!(!html.contains("<pre>"));

    runtime.run_once(true).expect("run true");
    let html = runtime.render_html();
    assert!(html.contains("data-frog-visual-state='true'"));
    assert!(html.contains("aria-pressed='true'"));
    assert!(html.contains(">ON</span>"));
    assert!(html.contains(">TRUE</span>"));
}
