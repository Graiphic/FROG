use std::fs;
use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::execute::execute_contract;
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
fn slice05_headless_matches_shared_snapshot() {
    let acceptance = acceptance();
    let refs = acceptance["artifact_refs"].as_object().unwrap();
    let headless = acceptance["headless"].as_object().unwrap();
    let contract_path = resolve_repo_path(refs["contract_path"].as_str().unwrap());
    let wfrog_path = resolve_repo_path(refs["wfrog_path"].as_str().unwrap());
    let snapshot_path = resolve_repo_path(refs["snapshot_path"].as_str().unwrap());
    let expected = load_json(&snapshot_path);

    let actual = execute_contract(
        headless["input_value"].as_u64().unwrap() as u16,
        Some(contract_path),
        Some(wfrog_path),
    )
    .expect("execute runtime");

    assert_eq!(actual, expected);
}

#[test]
fn slice05_overflow_matches_shared_acceptance() {
    let acceptance = acceptance();
    let refs = acceptance["artifact_refs"].as_object().unwrap();
    let overflow = acceptance["overflow"].as_object().unwrap();
    let contract_path = resolve_repo_path(refs["contract_path"].as_str().unwrap());
    let wfrog_path = resolve_repo_path(refs["wfrog_path"].as_str().unwrap());

    let error = execute_contract(
        overflow["input_value"].as_u64().unwrap() as u16,
        Some(contract_path),
        Some(wfrog_path),
    )
    .expect_err("overflow must be rejected");

    assert_eq!(error.to_string(), overflow["expected_error"].as_str().unwrap());
}
