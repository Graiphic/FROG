use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::BooleanBrowserUiRuntime;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn resolve_repo_path(relative_path: &str) -> PathBuf {
    repo_root().join(relative_path)
}

#[test]
fn slice06_ui_surface_consumes_wfrog_realization() {
    let contract_path = resolve_repo_path(
        "Implementations/Reference/ContractEmitter/examples/06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog_path = resolve_repo_path("Examples/06_boolean_value_roundtrip/ui/boolean_panel.wfrog");

    let mut runtime = BooleanBrowserUiRuntime::new(contract_path, wfrog_path).expect("build runtime");
    let html = runtime.render_html();

    assert!(html.contains("Rust reference runtime"));
    assert!(html.contains("boolean contract executor"));
    assert!(html.contains("data-runtime-language='rust'"));
    assert!(html.contains("data-execution-path='rust_boolean_contract_executor'"));
    assert!(html.contains("/asset/boolean_rectangular_svg"));
    assert!(html.contains("/asset/boolean_circular_svg"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("--boolean-caption-left:50%;"));
    assert!(html.contains("--boolean-inner-width:40px;"));
    assert!(html.contains("--boolean-inner-height:40px;"));
    assert!(!html.contains("Current runtime snapshot"));

    runtime.run_once(false).expect("execute false");
    let html = runtime.render_html();
    assert!(html.contains("value='true'"));
    assert!(html.contains("--boolean-fill:#ef4444;"));
}
