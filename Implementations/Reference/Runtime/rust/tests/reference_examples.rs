use std::fs;
use std::path::{Path, PathBuf};

use frog_reference_runtime_rust::contract::find_repo_root;
use frog_reference_runtime_rust::execute::{execute_reference_acceptance, execute_reference_contract_case};
use pretty_assertions::assert_eq;
use serde_json::{json, Value};

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn load_json(path: &Path) -> Value {
    serde_json::from_str(&fs::read_to_string(path).expect("read file")).expect("parse json")
}

fn resolve_repo_path(relative_path: &str) -> PathBuf {
    repo_root().join(relative_path)
}

fn check_acceptance_file(relative_path: &str) {
    let acceptance = load_json(&resolve_repo_path(relative_path));
    let refs = acceptance["artifact_refs"].as_object().unwrap();
    let contract = load_json(&resolve_repo_path(refs["contract_path"].as_str().unwrap()));
    let expected = load_json(&resolve_repo_path(refs["snapshot_path"].as_str().unwrap()));
    let wfrog = refs
        .get("wfrog_path")
        .and_then(Value::as_str)
        .map(|path| load_json(&resolve_repo_path(path)));
    let actual = execute_reference_acceptance(&acceptance, &contract, wfrog.as_ref()).expect("execute acceptance");
    assert_eq!(actual, expected);
}

#[test]
fn reference_runtime_executes_published_examples() {
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example03_ui_property_write.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json");
    check_acceptance_file("Implementations/Reference/Runtime/acceptance/example06_boolean_value_roundtrip.acceptance.json");
}

#[test]
fn reference_runtime_executes_example06_true_and_false_cases() {
    let acceptance = load_json(&resolve_repo_path(
        "Implementations/Reference/Runtime/acceptance/example06_boolean_value_roundtrip.acceptance.json",
    ));
    let refs = acceptance["artifact_refs"].as_object().unwrap();
    let contract = load_json(&resolve_repo_path(refs["contract_path"].as_str().unwrap()));
    let wfrog = load_json(&resolve_repo_path(refs["wfrog_path"].as_str().unwrap()));

    for case in acceptance["cases"].as_array().unwrap() {
        let expected = load_json(&resolve_repo_path(case["snapshot_path"].as_str().unwrap()));
        let actual = execute_reference_contract_case(
            &contract,
            &json!({"input_value": case["input_value"].as_bool().unwrap()}),
            Some(&wfrog),
        )
        .expect("execute example06 case");
        assert_eq!(actual, expected);
    }
}
