use std::fs;
use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::ui::BrowserUiRuntime;
use pretty_assertions::assert_eq;
use serde_json::Value;

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn load_json(path: &Path) -> Value {
    serde_json::from_str(&fs::read_to_string(path).expect("read file")).expect("parse json")
}

fn acceptance() -> Value {
    load_json(
        &repo_root()
            .join("Implementations")
            .join("Reference")
            .join("Runtime")
            .join("acceptance")
            .join("example05_runtime_family.acceptance.json"),
    )
}

fn resolve_repo_path(relative_path: &str) -> PathBuf {
    repo_root().join(relative_path)
}

#[test]
fn slice05_ui_surface_matches_shared_acceptance() {
    let acceptance = acceptance();
    let refs = acceptance["artifact_refs"].as_object().unwrap();
    let routes = acceptance["ui"]["expected_routes"].as_array().unwrap();
    let contract_path = resolve_repo_path(refs["contract_path"].as_str().unwrap());
    let wfrog_path = resolve_repo_path(refs["wfrog_path"].as_str().unwrap());
    let snapshot_path = resolve_repo_path(refs["snapshot_path"].as_str().unwrap());
    let expected_snapshot = load_json(&snapshot_path);

    let mut runtime = BrowserUiRuntime::new(Some(contract_path), Some(wfrog_path)).expect("build runtime");
    let html = runtime.render_html();
    assert!(html.contains("Rust reference runtime"));
    assert!(html.contains("contract executor"));
    assert!(html.contains("none in runtime path"));
    for route in routes {
        let route = route.as_str().unwrap();
        if route == "/" {
            continue;
        }
        assert!(html.contains(route), "missing route in html: {route}");
    }

    runtime
        .core
        .execute(Some(acceptance["headless"]["input_value"].as_u64().unwrap() as u16))
        .expect("execute runtime");
    assert_eq!(runtime.core.execution_artifact(), expected_snapshot);
    assert!(runtime.core.asset_map.get("numeric_control_svg").unwrap().exists());
    assert!(runtime.core.asset_map.get("numeric_indicator_svg").unwrap().exists());
}
