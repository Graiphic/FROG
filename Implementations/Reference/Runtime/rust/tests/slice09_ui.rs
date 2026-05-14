use std::collections::BTreeMap;
use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::PathBrowserUiRuntime;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn contract_path() -> PathBuf {
    repo_root().join(
        "Implementations/Reference/ContractEmitter/examples/09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    )
}

fn wfrog_path() -> PathBuf {
    repo_root().join("Examples/09_path_value_roundtrip/ui/path_panel.wfrog")
}

#[test]
fn rust_example09_headless_and_browser_ui_consume_wfrog_realization() {
    let mut runtime = PathBrowserUiRuntime::with_native_kernel_bridge(contract_path(), wfrog_path(), None).expect("runtime");
    let mut values = BTreeMap::new();
    values.insert("input_path".to_string(), "C:/FROG/top.txt".to_string());
    values.insert("input_path_no_icon".to_string(), "C:/FROG/bottom.txt".to_string());
    let artifact = runtime.run_all(values).expect("run path");

    assert_eq!(artifact["outputs"]["public"]["result_path"].as_str(), Some("C:/FROG/top.txt"));
    assert_eq!(artifact["outputs"]["ui"]["path_result"].as_str(), Some("C:/FROG/top.txt"));
    assert_eq!(artifact["outputs"]["ui"]["path_result_no_icon"].as_str(), Some("C:/FROG/bottom.txt"));

    let html = runtime.render_html();
    assert!(html.contains("Path Value Roundtrip"));
    assert!(html.contains("data-runtime-language='rust'"));
    assert!(html.contains("data-compiler-backend='none'"));
    assert!(html.contains("data-execution-path='rust_path_contract_executor'"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("data-asset-route='/asset/path_rectangular_field_svg'"));
    assert!(html.contains("class='path-skin'"));
    assert!(html.contains("data-frog-part='browse_button'"));
    assert!(html.contains("name='input_path' type='text'"));
    assert!(html.contains("name='input_path_no_icon' type='text'"));
    assert!(html.contains("data-frog-input-id='input_path'"));
    assert!(html.contains("data-frog-input-id='input_path_no_icon'"));
    assert!(html.contains("--frog-path-icon-display:none"));
    assert!(html.contains("--frog-path-face-stroke-width:2px"));
    assert!(html.contains("frogPathPicked"));
    assert!(!html.contains("path-card"));
    assert!(!html.contains("fallback"));
    assert!(!html.contains("Current runtime snapshot"));
    assert!(!html.contains("<pre>"));
}
