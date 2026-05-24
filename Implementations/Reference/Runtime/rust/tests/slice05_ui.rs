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
    assert!(html.contains("class='front-panel'"));
    assert!(html.contains("data-panel-id='main_panel'"));
    assert!(html.contains("data-coordinate-space='panel_pixels'"));
    assert!(html.contains("data-runtime-language='rust'"));
    assert!(html.contains("data-compiler-backend='none'"));
    assert!(html.contains("style='width:500px;height:170px;'"));
    assert!(html.contains("data-widget-id='ctrl_input'"));
    assert!(html.contains("data-widget-id='ind_result'"));
    assert!(html.contains("data-class-ref='frog.widgets.numeric_control'"));
    assert!(html.contains("data-class-ref='frog.widgets.numeric_indicator'"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("data-asset-route='/asset/numeric_rectangular_svg'"));
    assert!(html.contains("left:20px;top:24px;width:220px;height:88px;"));
    assert!(html.contains("left:260px;top:24px;width:220px;height:88px;"));
    assert!(html.contains("class='numeric-skin'"));
    assert!(html.contains("<svg"));
    assert!(html.contains("data-frog-part='caption'"));
    assert!(html.contains("data-svg-anchor='caption.anchor'"));
    assert!(html.contains("data-frog-part='text_value'"));
    assert!(html.contains("data-svg-anchor='text_value.center'"));
    assert!(html.contains("data-frog-part='increment_up'"));
    assert!(html.contains("data-frog-method='increment'"));
    assert!(html.contains("data-frog-part='increment_down'"));
    assert!(html.contains("data-frog-method='decrement'"));
    assert!(html.contains("data-frog-button-state-law='normal-pressed'"));
    assert!(html.contains("--frog-numeric-frame-fill:transparent"));
    assert!(html.contains("--frog-numeric-frame-stroke:transparent"));
    assert!(html.contains("--frog-numeric-frame-stroke-width:2px"));
    assert!(html.contains("--frog-numeric-step-fill:#5B9BD5"));
    assert!(html.contains("--frog-numeric-step-fill-pressed:#2B4F7B"));
    assert!(html.contains("--frog-numeric-caption-font-size:12px;"));
    assert!(html.contains("--frog-numeric-caption-font-weight:400;"));
    assert!(html.contains("--frog-numeric-text-color:#111827;"));
    assert!(html.contains("--frog-numeric-text-font-size:11px;"));
    assert!(html.contains("font-size:var(--frog-numeric-caption-font-size)"));
    assert!(!html.contains("font-size:12px;line-height:1;"));
    assert!(!html.contains("style='left:4.210526%;top:30.6667%;transform:translateY(-50%);text-align:left;color:#111827;"));
    assert!(!html.contains("Current runtime snapshot"));
    assert!(!html.contains("<pre>"));
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
    let asset_path = runtime.core.asset_map.get("numeric_rectangular_svg").unwrap();
    assert!(asset_path.exists());
    let asset = fs::read_to_string(asset_path).expect("read numeric asset");
    assert!(asset.contains("viewBox=\"0 0 380 150\""));
    assert!(asset.contains("id=\"caption_text\""));
    assert!(asset.contains("id=\"frame\""));
    assert!(asset.contains("width=\"222\""));
    assert!(asset.contains("id=\"value_face\""));
    assert!(asset.contains("id=\"increment_up\""));
    assert!(asset.contains("id=\"increment_down\""));
    assert!(asset.contains("data-frog-states=\"normal pressed\""));
    assert!(asset.contains("data-frog-state-law=\"normal-pressed-color-only\""));
    assert!(asset.contains("data-frog-bind=\"style.increment_button.*\""));
}
