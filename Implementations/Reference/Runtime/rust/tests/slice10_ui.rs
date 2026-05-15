use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::ButtonBrowserUiRuntime;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn contract_path() -> PathBuf {
    repo_root().join(
        "Implementations/Reference/ContractEmitter/examples/10_button_press_to_boolean.reference_host_runtime_ui_binding.contract.json",
    )
}

fn wfrog_path() -> PathBuf {
    repo_root().join("Examples/10_button_press_to_boolean/ui/button_panel.wfrog")
}

#[test]
fn rust_example10_headless_button_press_to_boolean() {
    let mut runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path(), wfrog_path(), None).expect("runtime");

    let artifact = runtime.run_once(false).expect("run false");
    assert_eq!(artifact["contract_ref"]["source_ref"]["example_id"].as_str(), Some("10_button_press_to_boolean"));
    assert_eq!(artifact["outputs"]["public"]["pressed"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["pressed_indicator"].as_bool(), Some(false));

    let artifact = runtime.run_once(true).expect("run true");
    assert_eq!(artifact["execution_summary"]["trigger_pressed"].as_bool(), Some(true));
    assert_eq!(artifact["outputs"]["public"]["pressed"].as_bool(), Some(true));
    assert_eq!(artifact["outputs"]["ui"]["trigger_button"].as_bool(), Some(false));
    assert_eq!(artifact["outputs"]["ui"]["pressed_indicator"].as_bool(), Some(true));
    let widgets = artifact["ui_runtime"]["widgets"].as_array().expect("widgets");
    let button = widgets
        .iter()
        .find(|widget| widget["widget_id"].as_str() == Some("trigger_button"))
        .expect("button widget");
    assert_eq!(button["class_ref"].as_str(), Some("frog.widgets.button"));
    assert_eq!(button["runtime"]["event.pressed"].as_bool(), Some(true));
    assert_eq!(button["runtime"]["value"].as_bool(), Some(false));
}

#[test]
fn rust_example10_browser_ui_consumes_default_svg_and_source_styles() {
    let mut runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path(), wfrog_path(), None).expect("runtime");
    let html = runtime.render_html();

    assert!(html.contains("Button Press to Boolean"));
    assert!(html.contains("Rust reference runtime"));
    assert!(html.contains("button contract executor"));
    assert!(html.contains("data-runtime-language='rust'"));
    assert!(html.contains("data-execution-path='rust_button_contract_executor'"));
    assert!(html.contains("data-class-ref='frog.widgets.button'"));
    assert!(html.contains("data-class-ref='frog.widgets.boolean_indicator'"));
    assert!(html.contains("data-asset-route='/asset/button_rectangular_svg'"));
    assert!(html.contains("data-asset-route='/asset/boolean_circular_svg'"));
    assert!(html.contains("data-frog-template=\"frog.realizations.default.button.rectangular\""));
    assert!(html.contains("class='button-skin'"));
    assert!(html.contains("data-frog-asset-consumed='true'"));
    assert!(html.contains("data-frog-mechanical-action='switch_until_released'"));
    assert!(html.contains("data-frog-part='caption' data-svg-anchor='caption.anchor'"));
    assert!(html.contains("data-frog-part='state_text' data-svg-anchor='state_text.center'"));
    assert!(html.contains("class='button-press-overlay' type='button'"));
    assert!(html.contains("data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_pressed'"));
    assert!(html.contains("data-frog-host-overlay='input' data-frog-align-to-part='face'"));
    assert!(html.contains("--frog-button-face-stroke-width:1px;"));
    assert!(html.contains("--frog-button-state-text-font-weight:400;"));
    assert!(html.contains("--frog-button-caption-font-size:18px;"));
    assert!(html.contains("--frog-button-caption-font-weight:600;"));
    assert!(html.contains("--boolean-caption-font-size:18px;"));
    assert!(html.contains("--boolean-text-font-size:12px;"));
    assert!(html.contains("--boolean-text-font-weight:400;"));
    assert!(html.contains("--boolean-inner-border-width:0px;"));
    assert!(html.contains("fetch(\"/event\""));
    assert!(html.contains("mechanicalAction !== \"switch_until_released\""));
    assert!(html.contains("pointerdown"));
    assert!(html.contains("pointerup"));
    assert!(html.contains(">OFF</span>"));
    assert!(html.contains(">FALSE</span>"));
    assert!(!html.contains("font-size:14px"));
    assert!(!html.contains("top:49px"));
    assert!(!html.contains("type='submit'"));
    assert!(!html.contains("missing-skin"));
    assert!(!html.contains("fallback"));
    assert!(!html.contains("Current runtime snapshot"));
    assert!(!html.contains("<pre>"));

    runtime.run_once(true).expect("run true");
    let html = runtime.render_html();
    assert!(html.contains("data-frog-visual-state='false'"));
    assert!(html.contains(">OFF</span>"));
    assert!(html.contains(">TRUE</span>"));
}
