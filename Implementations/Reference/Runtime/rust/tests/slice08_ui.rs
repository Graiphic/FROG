use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::EnumBrowserUiRuntime;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn resolve_repo_path(relative_path: &str) -> PathBuf {
    repo_root().join(relative_path)
}

#[test]
fn slice08_ui_surface_consumes_frog_instance_and_wfrog_realization() {
    let contract_path = resolve_repo_path(
        "Implementations/Reference/ContractEmitter/examples/08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog_path = resolve_repo_path("Examples/08_enum_value_roundtrip/ui/enum_panel.wfrog");

    let mut runtime = EnumBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None).expect("build runtime");
    let html = runtime.render_html();

    assert!(html.contains("Rust reference runtime"));
    assert!(html.contains("enum contract executor"));
    assert!(html.contains("data-runtime-language='rust'"));
    assert!(html.contains("data-execution-path='rust_enum_contract_executor'"));
    assert!(html.contains("/asset/enum_rectangular_ring_svg"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("class='enum-value-display-overlay enum-display-button'"));
    assert!(html.contains("class='enum-selector-overlay enum-selector-button'"));
    assert!(html.contains("class='enum-dropdown'"));
    assert!(html.contains("--frog-enum-selector-hover-fill:#60a5fa;"));
    assert!(html.contains("--frog-enum-dropdown-option-hover-fill:#2563eb;"));
    assert!(!html.contains("enum-card"));
    assert!(!html.contains("Current runtime snapshot"));

    let artifact = runtime.run_once("fault".to_string()).expect("execute enum");
    assert_eq!(artifact["outputs"]["public"]["result_mode"].as_str(), Some("fault"));
    assert_eq!(artifact["outputs"]["ui"]["mode_result"].as_str(), Some("fault"));
}
