use std::path::{Path, PathBuf};
use std::process::Command;

use frog_reference_runtime_rust::contract::{default_contract_path, default_wfrog_path, find_repo_root};
use frog_reference_runtime_rust::native_kernel::{
    NativeBoolKernelBridge, NativeEnumKernelBridge, NativeKernelBridge, NativeStringKernelBridge,
};
use frog_reference_runtime_rust::runtime::RuntimeCore;
use frog_reference_runtime_rust::ui::{BooleanBrowserUiRuntime, BrowserUiRuntime, EnumBrowserUiRuntime, PathBrowserUiRuntime, StringBrowserUiRuntime};

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
    assert!(html.contains("--boolean-focus-color:#2563eb;"));
    assert!(html.contains("--boolean-focus-width:3px;"));
    assert!(html.contains("outline:var(--boolean-focus-width) solid var(--boolean-focus-color);"));
    assert!(!html.contains("outline:2px solid #2563eb"));
}

#[test]
fn rust_dynamic_native_kernel_bridge_executes_example07() {
    let root = repo_root();
    let manifest = root.join("Implementations/Reference/LLVM/examples/07_string_value_roundtrip/native_kernel_manifest.json");
    let kernel_ll = root.join("Implementations/Reference/LLVM/examples/07_string_value_roundtrip/kernel.ll");
    let contract = root.join(
        "Implementations/Reference/ContractEmitter/examples/07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog = root.join("Examples/07_string_value_roundtrip/ui/string_panel.wfrog");
    let Some(library) = build_native_library("07", &kernel_ll) else {
        return;
    };

    let bridge = NativeStringKernelBridge::from_paths(&manifest, &library).expect("load native string bridge");
    let result = bridge.run("hello world");
    assert!(result.ok);
    assert_eq!(result.result, "hello world");

    let mut runtime = StringBrowserUiRuntime::with_native_kernel_bridge(contract, wfrog, Some(bridge)).expect("runtime core");
    let artifact = runtime.run_once("hello world".to_string()).expect("execute native string kernel");
    assert_eq!(artifact["outputs"]["public"]["result_text"].as_str(), Some("hello world"));
    assert_eq!(artifact["outputs"]["ui"]["str_result"].as_str(), Some("hello world"));
    let html = runtime.render_html();
    assert!(html.contains("native kernel bridge"));
    assert!(html.contains("LLVM native string kernel artifact"));
    assert!(html.contains("data-compiler-backend='llvm'"));
    assert!(html.contains("data-execution-path='native_kernel_bridge'"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
    assert!(html.contains("--frog-string-text-region-fill-hover:#eef6ff;"));
    assert!(html.contains("--frog-string-text-region-stroke-hover:#2563eb;"));
    assert!(html.contains(".string-control:hover .string-skin svg"));
    assert!(!html.contains("Current runtime snapshot"));
    assert!(!html.contains("<pre>"));
    assert!(!html.contains("outline:2px solid #2563eb"));
}

#[test]
fn rust_dynamic_native_kernel_bridge_executes_example08() {
    let root = repo_root();
    let manifest = root.join("Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/native_kernel_manifest.json");
    let kernel_ll = root.join("Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/kernel.ll");
    let contract = root.join(
        "Implementations/Reference/ContractEmitter/examples/08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog = root.join("Examples/08_enum_value_roundtrip/ui/enum_panel.wfrog");
    let Some(library) = build_native_library("08", &kernel_ll) else {
        return;
    };

    let bridge = NativeEnumKernelBridge::from_paths(&manifest, &library).expect("load native enum bridge");
    let result = bridge.run(2);
    assert!(result.ok);
    assert_eq!(result.result_numeric_value, 2);

    let mut runtime = EnumBrowserUiRuntime::with_native_kernel_bridge(contract, wfrog, Some(bridge)).expect("runtime core");
    let artifact = runtime.run_once("fault".to_string()).expect("execute native enum kernel");
    assert_eq!(artifact["outputs"]["public"]["result_mode"].as_str(), Some("fault"));
    assert_eq!(artifact["outputs"]["ui"]["mode_result"].as_str(), Some("fault"));
    let html = runtime.render_html();
    assert!(html.contains("native kernel bridge"));
    assert!(html.contains("LLVM native enum kernel artifact"));
    assert!(html.contains("data-compiler-backend='llvm'"));
    assert!(html.contains("data-execution-path='native_kernel_bridge'"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
}

#[test]
fn rust_dynamic_native_kernel_bridge_executes_example09() {
    let root = repo_root();
    let manifest = root.join("Implementations/Reference/LLVM/examples/09_path_value_roundtrip/native_kernel_manifest.json");
    let kernel_ll = root.join("Implementations/Reference/LLVM/examples/09_path_value_roundtrip/kernel.ll");
    let contract = root.join(
        "Implementations/Reference/ContractEmitter/examples/09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json",
    );
    let wfrog = root.join("Examples/09_path_value_roundtrip/ui/path_panel.wfrog");
    let Some(library) = build_native_library("09", &kernel_ll) else {
        return;
    };

    let bridge = NativeStringKernelBridge::from_paths(&manifest, &library).expect("load native path bridge");
    let result = bridge.run("C:/FROG/from_rust_native.txt");
    assert!(result.ok);
    assert_eq!(result.result, "C:/FROG/from_rust_native.txt");

    let mut runtime = PathBrowserUiRuntime::with_native_kernel_bridge(contract, wfrog, Some(bridge)).expect("runtime core");
    let artifact = runtime.run_once("C:/FROG/from_rust_native.txt".to_string()).expect("execute native path kernel");
    assert_eq!(artifact["outputs"]["public"]["result_path"].as_str(), Some("C:/FROG/from_rust_native.txt"));
    assert_eq!(artifact["outputs"]["ui"]["path_result"].as_str(), Some("C:/FROG/from_rust_native.txt"));
    let html = runtime.render_html();
    assert!(html.contains("native kernel bridge"));
    assert!(html.contains("LLVM native path kernel artifact"));
    assert!(html.contains("data-compiler-backend='llvm'"));
    assert!(html.contains("data-execution-path='native_kernel_bridge'"));
    assert!(html.contains("data-frog-visual-law='wfrog-realization-state-map'"));
}
