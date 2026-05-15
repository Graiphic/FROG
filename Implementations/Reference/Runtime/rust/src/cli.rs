use std::fs;
use std::path::{Path, PathBuf};

use serde_json::{json, to_string_pretty, Value};

use crate::contract::{default_contract_path, default_wfrog_path, find_repo_root};
use crate::diagnostics::{Result, RuntimeError};
use crate::execute::{execute_contract, execute_reference_contract_case};
use crate::native_kernel::{NativeBoolKernelBridge, NativeEnumKernelBridge, NativeKernelBridge, NativeStringKernelBridge};
use crate::runtime::RuntimeCore;
use crate::ui::{BooleanBrowserUiRuntime, BrowserUiRuntime, ButtonBrowserUiRuntime, EnumBrowserUiRuntime, PathBrowserUiRuntime, StringBrowserUiRuntime};

fn repo_root() -> Result<PathBuf> {
    find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR")))
}

fn example06_contract_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json"))
}

fn example06_wfrog_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Examples")
        .join("06_boolean_value_roundtrip")
        .join("ui")
        .join("boolean_panel.wfrog"))
}

fn example07_contract_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json"))
}

fn example07_wfrog_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Examples")
        .join("07_string_value_roundtrip")
        .join("ui")
        .join("string_panel.wfrog"))
}

fn example08_contract_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json"))
}

fn example08_wfrog_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Examples")
        .join("08_enum_value_roundtrip")
        .join("ui")
        .join("enum_panel.wfrog"))
}

fn example09_contract_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json"))
}

fn example09_wfrog_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Examples")
        .join("09_path_value_roundtrip")
        .join("ui")
        .join("path_panel.wfrog"))
}

fn example10_contract_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("10_button_press_to_boolean.reference_host_runtime_ui_binding.contract.json"))
}

fn example10_wfrog_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Examples")
        .join("10_button_press_to_boolean")
        .join("ui")
        .join("button_panel.wfrog"))
}

fn example11_contract_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("11_button_switch_when_pressed.reference_host_runtime_ui_binding.contract.json"))
}

fn example11_wfrog_path() -> Result<PathBuf> {
    Ok(repo_root()?
        .join("Examples")
        .join("11_button_switch_when_pressed")
        .join("ui")
        .join("button_panel.wfrog"))
}

fn wants_example06(value: &str) -> bool {
    matches!(value, "06" | "6" | "example06" | "06_boolean_value_roundtrip")
}

fn wants_example07(value: &str) -> bool {
    matches!(value, "07" | "7" | "example07" | "07_string_value_roundtrip")
}

fn wants_example08(value: &str) -> bool {
    matches!(value, "08" | "8" | "example08" | "08_enum_value_roundtrip")
}

fn wants_example09(value: &str) -> bool {
    matches!(value, "09" | "9" | "example09" | "09_path_value_roundtrip")
}

fn wants_example10(value: &str) -> bool {
    matches!(value, "10" | "example10" | "10_button_press_to_boolean")
}

fn wants_example11(value: &str) -> bool {
    matches!(value, "11" | "example11" | "11_button_switch_when_pressed")
}

fn parse_bool_input(value: &str) -> Result<bool> {
    match value.trim().to_ascii_lowercase().as_str() {
        "true" | "1" | "on" => Ok(true),
        "false" | "0" | "" => Ok(false),
        _ => Err(RuntimeError::Message("Boolean input must be true or false.".to_string())),
    }
}

fn load_json(path: &Path) -> Result<Value> {
    Ok(serde_json::from_str(&fs::read_to_string(path)?)?)
}

fn contract_is_example06(path: &Path) -> bool {
    let Ok(contract) = load_json(path) else {
        return false;
    };
    contract["source_ref"]["example_id"].as_str() == Some("06_boolean_value_roundtrip")
        || contract["example_id"].as_str() == Some("06_boolean_value_roundtrip")
}

fn contract_is_example07(path: &Path) -> bool {
    let Ok(contract) = load_json(path) else {
        return false;
    };
    contract["source_ref"]["example_id"].as_str() == Some("07_string_value_roundtrip")
        || contract["example_id"].as_str() == Some("07_string_value_roundtrip")
}

fn contract_is_example08(path: &Path) -> bool {
    let Ok(contract) = load_json(path) else {
        return false;
    };
    contract["source_ref"]["example_id"].as_str() == Some("08_enum_value_roundtrip")
        || contract["example_id"].as_str() == Some("08_enum_value_roundtrip")
}

fn contract_is_example09(path: &Path) -> bool {
    let Ok(contract) = load_json(path) else {
        return false;
    };
    contract["source_ref"]["example_id"].as_str() == Some("09_path_value_roundtrip")
        || contract["example_id"].as_str() == Some("09_path_value_roundtrip")
}

fn contract_is_example10(path: &Path) -> bool {
    let Ok(contract) = load_json(path) else {
        return false;
    };
    contract["source_ref"]["example_id"].as_str() == Some("10_button_press_to_boolean")
        || contract["example_id"].as_str() == Some("10_button_press_to_boolean")
}

fn contract_is_example11(path: &Path) -> bool {
    let Ok(contract) = load_json(path) else {
        return false;
    };
    contract["source_ref"]["example_id"].as_str() == Some("11_button_switch_when_pressed")
        || contract["example_id"].as_str() == Some("11_button_switch_when_pressed")
}

pub fn run_cli() -> Result<()> {
    let mut args = std::env::args().skip(1).collect::<Vec<String>>();
    if args.first().map(|value| value.as_str()) == Some("ui") {
        args.remove(0);
        let mut contract_path: Option<PathBuf> = None;
        let mut wfrog_path: Option<PathBuf> = None;
        let mut example: Option<String> = None;
        let mut host = "127.0.0.1".to_string();
        let mut port: u16 = 0;
        let mut open_browser = true;
        let mut native_kernel_manifest: Option<PathBuf> = None;
        let mut native_kernel_library: Option<PathBuf> = None;

        let mut index = 0usize;
        while index < args.len() {
            match args[index].as_str() {
                "--example" => {
                    index += 1;
                    example = Some(args.get(index).cloned().ok_or_else(|| RuntimeError::Message("Missing --example value.".to_string()))?);
                }
                "--contract" => {
                    index += 1;
                    contract_path = Some(PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --contract value.".to_string()))?));
                }
                "--wfrog" => {
                    index += 1;
                    wfrog_path = Some(PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --wfrog value.".to_string()))?));
                }
                "--host" => {
                    index += 1;
                    host = args.get(index).cloned().ok_or_else(|| RuntimeError::Message("Missing --host value.".to_string()))?;
                }
                "--port" => {
                    index += 1;
                    port = args
                        .get(index)
                        .ok_or_else(|| RuntimeError::Message("Missing --port value.".to_string()))?
                        .parse::<u16>()?;
                }
                "--no-open-browser" => open_browser = false,
                "--native-kernel-manifest" => {
                    index += 1;
                    native_kernel_manifest = Some(PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --native-kernel-manifest value.".to_string()))?));
                }
                "--native-kernel-library" => {
                    index += 1;
                    native_kernel_library = Some(PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --native-kernel-library value.".to_string()))?));
                }
                value => return Err(RuntimeError::Message(format!("Unknown ui argument: {value}"))),
            }
            index += 1;
        }

        if example.as_deref().is_some_and(wants_example06) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeBoolKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = BooleanBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap_or(example06_contract_path()?),
                wfrog_path.unwrap_or(example06_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if contract_path.as_deref().is_some_and(contract_is_example06) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeBoolKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = BooleanBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap(),
                wfrog_path.unwrap_or(example06_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if example.as_deref().is_some_and(wants_example07) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeStringKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = StringBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap_or(example07_contract_path()?),
                wfrog_path.unwrap_or(example07_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if contract_path.as_deref().is_some_and(contract_is_example07) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeStringKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = StringBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap(),
                wfrog_path.unwrap_or(example07_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if example.as_deref().is_some_and(wants_example08) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeEnumKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = EnumBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap_or(example08_contract_path()?),
                wfrog_path.unwrap_or(example08_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if contract_path.as_deref().is_some_and(contract_is_example08) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeEnumKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = EnumBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap(),
                wfrog_path.unwrap_or(example08_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if example.as_deref().is_some_and(wants_example09) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeStringKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = PathBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap_or(example09_contract_path()?),
                wfrog_path.unwrap_or(example09_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if contract_path.as_deref().is_some_and(contract_is_example09) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeStringKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = PathBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap(),
                wfrog_path.unwrap_or(example09_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if example.as_deref().is_some_and(wants_example10) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeBoolKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap_or(example10_contract_path()?),
                wfrog_path.unwrap_or(example10_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if contract_path.as_deref().is_some_and(contract_is_example10) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeBoolKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap(),
                wfrog_path.unwrap_or(example10_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if example.as_deref().is_some_and(wants_example11) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeBoolKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap_or(example11_contract_path()?),
                wfrog_path.unwrap_or(example11_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }
        if contract_path.as_deref().is_some_and(contract_is_example11) {
            let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
                (Some(manifest), Some(library)) => Some(NativeBoolKernelBridge::from_paths(manifest, library)?),
                _ => None,
            };
            let runtime = ButtonBrowserUiRuntime::with_native_kernel_bridge(
                contract_path.unwrap(),
                wfrog_path.unwrap_or(example11_wfrog_path()?),
                native_bridge,
            )?;
            return runtime.serve(&host, port, open_browser);
        }

        let native_bridge = match (&native_kernel_manifest, &native_kernel_library) {
            (Some(manifest), Some(library)) => Some(NativeKernelBridge::from_paths(manifest, library)?),
            _ => None,
        };
        let runtime = BrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, native_bridge)?;
        return runtime.serve(&host, port, open_browser);
    }

    let input_value_text = if args.is_empty() || args[0] == "run" {
        if !args.is_empty() && args[0] == "run" {
            args.remove(0);
        }
        if args.is_empty() {
            "3".to_string()
        } else {
            args.remove(0)
        }
    } else {
        args.remove(0)
    };

    let mut contract_path = default_contract_path()?;
    let mut wfrog_path = default_wfrog_path()?;
    let mut example: Option<String> = None;
    let mut native_kernel_manifest: Option<PathBuf> = None;
    let mut native_kernel_library: Option<PathBuf> = None;
    let mut index = 0usize;
    while index < args.len() {
        match args[index].as_str() {
            "--example" => {
                index += 1;
                example = Some(args.get(index).cloned().ok_or_else(|| RuntimeError::Message("Missing --example value.".to_string()))?);
            }
            "--contract" => {
                index += 1;
                contract_path = PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --contract value.".to_string()))?);
            }
            "--wfrog" => {
                index += 1;
                wfrog_path = PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --wfrog value.".to_string()))?);
            }
            "--native-kernel-manifest" => {
                index += 1;
                native_kernel_manifest = Some(PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --native-kernel-manifest value.".to_string()))?));
            }
            "--native-kernel-library" => {
                index += 1;
                native_kernel_library = Some(PathBuf::from(args.get(index).ok_or_else(|| RuntimeError::Message("Missing --native-kernel-library value.".to_string()))?));
            }
            value => return Err(RuntimeError::Message(format!("Unknown argument: {value}"))),
        }
        index += 1;
    }

    let artifact = if example.as_deref().is_some_and(wants_example06) || contract_is_example06(&contract_path) {
        let contract_path = if example.as_deref().is_some_and(wants_example06) && contract_path == default_contract_path()? {
            example06_contract_path()?
        } else {
            contract_path
        };
        let wfrog_path = if example.as_deref().is_some_and(wants_example06) && wfrog_path == default_wfrog_path()? {
            example06_wfrog_path()?
        } else {
            wfrog_path
        };
        if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
            let bridge = NativeBoolKernelBridge::from_paths(manifest, library)?;
            let mut runtime = BooleanBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, Some(bridge))?;
            runtime.run_once(parse_bool_input(&input_value_text)?)?
        } else {
            let contract = load_json(&contract_path)?;
            let wfrog = load_json(&wfrog_path)?;
            execute_reference_contract_case(
                &contract,
                &json!({"input_value": parse_bool_input(&input_value_text)?}),
                Some(&wfrog),
            )?
        }
    } else if example.as_deref().is_some_and(wants_example07) || contract_is_example07(&contract_path) {
        let contract_path = if example.as_deref().is_some_and(wants_example07) && contract_path == default_contract_path()? {
            example07_contract_path()?
        } else {
            contract_path
        };
        let wfrog_path = if example.as_deref().is_some_and(wants_example07) && wfrog_path == default_wfrog_path()? {
            example07_wfrog_path()?
        } else {
            wfrog_path
        };
        let mut runtime = if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
            let bridge = NativeStringKernelBridge::from_paths(manifest, library)?;
            StringBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, Some(bridge))?
        } else {
            StringBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None)?
        };
        runtime.run_once(input_value_text)?
    } else if example.as_deref().is_some_and(wants_example08) || contract_is_example08(&contract_path) {
        let contract_path = if example.as_deref().is_some_and(wants_example08) && contract_path == default_contract_path()? {
            example08_contract_path()?
        } else {
            contract_path
        };
        let wfrog_path = if example.as_deref().is_some_and(wants_example08) && wfrog_path == default_wfrog_path()? {
            example08_wfrog_path()?
        } else {
            wfrog_path
        };
        let mut runtime = if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
            let bridge = NativeEnumKernelBridge::from_paths(manifest, library)?;
            EnumBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, Some(bridge))?
        } else {
            EnumBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None)?
        };
        runtime.run_once(input_value_text)?
    } else if example.as_deref().is_some_and(wants_example09) || contract_is_example09(&contract_path) {
        let contract_path = if example.as_deref().is_some_and(wants_example09) && contract_path == default_contract_path()? {
            example09_contract_path()?
        } else {
            contract_path
        };
        let wfrog_path = if example.as_deref().is_some_and(wants_example09) && wfrog_path == default_wfrog_path()? {
            example09_wfrog_path()?
        } else {
            wfrog_path
        };
        let mut runtime = if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
            let bridge = NativeStringKernelBridge::from_paths(manifest, library)?;
            PathBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, Some(bridge))?
        } else {
            PathBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None)?
        };
        runtime.run_once(input_value_text)?
    } else if example.as_deref().is_some_and(wants_example10) || contract_is_example10(&contract_path) {
        let contract_path = if example.as_deref().is_some_and(wants_example10) && contract_path == default_contract_path()? {
            example10_contract_path()?
        } else {
            contract_path
        };
        let wfrog_path = if example.as_deref().is_some_and(wants_example10) && wfrog_path == default_wfrog_path()? {
            example10_wfrog_path()?
        } else {
            wfrog_path
        };
        let mut runtime = if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
            let bridge = NativeBoolKernelBridge::from_paths(manifest, library)?;
            ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, Some(bridge))?
        } else {
            ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None)?
        };
        runtime.run_once(parse_bool_input(&input_value_text)?)?
    } else if example.as_deref().is_some_and(wants_example11) || contract_is_example11(&contract_path) {
        let contract_path = if example.as_deref().is_some_and(wants_example11) && contract_path == default_contract_path()? {
            example11_contract_path()?
        } else {
            contract_path
        };
        let wfrog_path = if example.as_deref().is_some_and(wants_example11) && wfrog_path == default_wfrog_path()? {
            example11_wfrog_path()?
        } else {
            wfrog_path
        };
        let mut runtime = if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
            let bridge = NativeBoolKernelBridge::from_paths(manifest, library)?;
            ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, Some(bridge))?
        } else {
            ButtonBrowserUiRuntime::with_native_kernel_bridge(contract_path, wfrog_path, None)?
        };
        runtime.run_once(parse_bool_input(&input_value_text)?)?
    } else if let (Some(manifest), Some(library)) = (&native_kernel_manifest, &native_kernel_library) {
        let bridge = NativeKernelBridge::from_paths(manifest, library)?;
        let mut runtime = RuntimeCore::from_paths(&contract_path, &wfrog_path)?;
        runtime.execute_with_native_kernel_bridge(&bridge, Some(input_value_text.parse::<u16>()?))?
    } else {
        execute_contract(input_value_text.parse::<u16>()?, Some(contract_path), Some(wfrog_path))?
    };
    println!("{}", to_string_pretty(&artifact).unwrap());
    Ok(())
}
