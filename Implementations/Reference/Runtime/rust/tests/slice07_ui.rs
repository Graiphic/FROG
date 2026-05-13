use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::StringBrowserUiRuntime;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn resolve_repo_path(relative_path: &str) -> PathBuf {
    repo_root().join(relative_path)
}

#[test]
fn slice07_ui_surface_consumes_frog_instance_and_wfrog_realization() {
    let contract_path = resolve_repo_path(
        "Implementations/Reference/ContractEmitter/examples/07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog_path = resolve_repo_path("Examples/07_string_value_roundtrip/ui/string_panel.wfrog");

    let mut runtime = StringBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None).expect("build runtime");
    let html = runtime.render_html();

    assert!(html.contains("Rust reference runtime"));
    assert!(html.contains("string contract executor"));
    assert!(html.contains("data-runtime-language='rust'"));
    assert!(html.contains("data-execution-path='rust_string_contract_executor'"));
    assert!(html.contains("/asset/string_rectangular_svg"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("--frog-string-text-region-fill-hover:#eef6ff;"));
    assert!(html.contains("--frog-string-text-region-stroke-hover:#2563eb;"));
    assert!(html.contains(".string-control:hover .string-skin svg"));
    assert!(!html.contains("string-card"));
    assert!(!html.contains("Current runtime snapshot"));
    assert!(!html.contains("<pre>"));
    assert!(!html.contains("outline:2px solid #2563eb"));

    let artifact = runtime.run_once("edited hello".to_string()).expect("execute string");
    assert_eq!(artifact["outputs"]["public"]["result_text"].as_str(), Some("edited hello"));
    assert_eq!(artifact["outputs"]["ui"]["str_result"].as_str(), Some("edited hello"));
}
