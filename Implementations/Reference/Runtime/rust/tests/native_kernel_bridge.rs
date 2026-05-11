use std::path::{Path, PathBuf};
use std::process::Command;

use frog_reference_runtime_rust::contract::{default_contract_path, default_wfrog_path, find_repo_root};
use frog_reference_runtime_rust::native_kernel::{NativeBoolKernelBridge, NativeKernelBridge};
use frog_reference_runtime_rust::runtime::RuntimeCore;
use frog_reference_runtime_rust::ui::{BooleanBrowserUiRuntime, BrowserUiRuntime};

fn repo_root() -> PathBuf {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR"))).expect("repo root")
}

fn shared_library_suffix() -> &'static str {
    if cfg!(windows) {
        ".dll"
    } else if cfg!(target_os = "macos") {
        ".dylib"
    } else {
        ".so"
    }
}

fn clang_available() -> bool {
    Command::new("clang")
        .arg("--version")
        .status()
        .map(|status| status.success())
        .unwrap_or(false)
}

fn build_native_library(example: &str, kernel_ll: &Path) -> Option<PathBuf> {
    if !clang_available() {
        eprintln!("skipping Rust dynamic native kernel bridge test: clang is not on PATH");
        return None;
    }
    let root = repo_root();
    let output_dir = root.join("build").join("frog_native_kernels_rust_tests");
    std::fs::create_dir_all(&output_dir).expect("create native test output dir");
    let output_path = output_dir.join(format!("example{example}_kernel{}", shared_library_suffix()));
    let mut command = Command::new("clang");
    command.arg("-shared");
    if cfg!(windows) {
        command.arg("-Wl,--export-all-symbols");
    }
    let status = command
        .arg("-o")
        .arg(&output_path)
        .arg(kernel_ll)
        .status()
        .expect("run clang");
    assert!(status.success(), "clang failed to build {}", output_path.display());
    Some(output_path)
}

#[test]
fn rust_dynamic_native_kernel_bridge_executes_example05() {
    let root = repo_root();
    let manifest = root.join("Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json");
    let kernel_ll = root.join("Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll");
    let Some(library) = build_native_library("05", &kernel_ll) else {
        return;
    };

    let bridge = NativeKernelBridge::from_paths(&manifest, &library).expect("load native bridge");
    let result = bridge.run(6);
    assert!(result.ok);
    assert_eq!(result.result, 30);

    let mut runtime = RuntimeCore::from_paths(default_contract_path().unwrap(), default_wfrog_path().unwrap()).expect("runtime core");
    let artifact = runtime
        .execute_with_native_kernel_bridge(&bridge, Some(6))
        .expect("execute native kernel");
    assert_eq!(artifact["outputs"]["public"]["result"].as_u64(), Some(30));

    let ui = BrowserUiRuntime::with_native_kernel_bridge(
        Some(default_contract_path().unwrap()),
        Some(default_wfrog_path().unwrap()),
        Some(bridge),
    )
    .expect("native ui runtime");
    let html = ui.render_html();
    assert!(html.contains("native kernel bridge"));
    assert!(html.contains("LLVM native kernel artifact"));
}

#[test]
fn rust_dynamic_native_kernel_bridge_executes_example06() {
    let root = repo_root();
    let manifest = root.join("Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json");
    let kernel_ll = root.join("Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/kernel.ll");
    let contract = root.join(
        "Implementations/Reference/ContractEmitter/examples/06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog = root.join("Examples/06_boolean_value_roundtrip/ui/boolean_panel.wfrog");
    let Some(library) = build_native_library("06", &kernel_ll) else {
        return;
    };

    let bridge = NativeBoolKernelBridge::from_paths(&manifest, &library).expect("load native bool bridge");
    assert!(bridge.run(true).result);
    assert!(!bridge.run(false).result);

    let mut runtime = BooleanBrowserUiRuntime::with_native_kernel_bridge(contract, wfrog, Some(bridge)).expect("runtime core");
    let artifact = runtime.run_once(true).expect("execute native bool kernel");
    assert_eq!(artifact["outputs"]["public"]["result"].as_bool(), Some(true));
    let html = runtime.render_html();
    assert!(html.contains("native kernel bridge"));
    assert!(html.contains("LLVM native bool kernel artifact"));
    assert!(html.contains("data-compiler-backend='llvm'"));
    assert!(html.contains("data-execution-path='native_kernel_bridge'"));
}
