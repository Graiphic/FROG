use std::ffi::{c_char, c_void, CString};
use std::fs;
use std::path::{Path, PathBuf};

use serde_json::Value;

use crate::diagnostics::{ensure, Result, RuntimeError};

#[repr(C)]
#[derive(Debug, Default, Clone, Copy)]
pub struct FrogRunResult {
    pub ok: u8,
    pub result: u16,
    pub error_code: u16,
}

#[repr(C)]
#[derive(Debug, Default, Clone, Copy)]
pub struct FrogBoolRunResult {
    pub ok: u8,
    pub result: u8,
    pub error_code: u16,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FrogStringRunResult {
    pub ok: u8,
    pub error_code: u16,
    pub result_len: u32,
    pub result_buffer: [u8; 256],
}

#[repr(C)]
#[derive(Debug, Default, Clone, Copy)]
pub struct FrogEnumRunResult {
    pub ok: u8,
    pub result: u16,
    pub error_code: u16,
}

impl Default for FrogStringRunResult {
    fn default() -> Self {
        Self {
            ok: 0,
            error_code: 0,
            result_len: 0,
            result_buffer: [0; 256],
        }
    }
}

type FrogNativeKernelFunction = unsafe extern "C" fn(u16, *mut FrogRunResult);
type FrogNativeBoolKernelFunction = unsafe extern "C" fn(u8, *mut FrogBoolRunResult);
type FrogNativeStringKernelFunction = unsafe extern "C" fn(*const u8, u32, *mut FrogStringRunResult);
type FrogNativeEnumKernelFunction = unsafe extern "C" fn(u16, *mut FrogEnumRunResult);

#[derive(Debug, Clone)]
pub struct NativeKernelManifest {
    pub manifest_path: PathBuf,
    pub source_lowered_unit: String,
    pub entry_symbol: String,
    pub abi: String,
    pub diagnostics_by_code: std::collections::BTreeMap<u16, String>,
}

#[derive(Debug, Clone, Copy)]
pub struct NativeKernelResult {
    pub ok: bool,
    pub result: u16,
    pub error_code: u16,
}

#[derive(Debug, Clone, Copy)]
pub struct NativeBoolKernelResult {
    pub ok: bool,
    pub result: bool,
    pub error_code: u16,
}

#[derive(Debug, Clone)]
pub struct NativeStringKernelResult {
    pub ok: bool,
    pub result: String,
    pub error_code: u16,
}

#[derive(Debug, Clone, Copy)]
pub struct NativeEnumKernelResult {
    pub ok: bool,
    pub result_numeric_value: u16,
    pub error_code: u16,
}

pub struct NativeKernelBridge {
    manifest: NativeKernelManifest,
    library: DynamicLibrary,
    entry_point: FrogNativeKernelFunction,
}

pub struct NativeBoolKernelBridge {
    manifest: NativeKernelManifest,
    library: DynamicLibrary,
    entry_point: FrogNativeBoolKernelFunction,
}

pub struct NativeStringKernelBridge {
    manifest: NativeKernelManifest,
    library: DynamicLibrary,
    entry_point: FrogNativeStringKernelFunction,
}

pub struct NativeEnumKernelBridge {
    manifest: NativeKernelManifest,
    library: DynamicLibrary,
    entry_point: FrogNativeEnumKernelFunction,
}

pub fn load_native_kernel_manifest(path: impl AsRef<Path>) -> Result<NativeKernelManifest> {
    let manifest_path = path.as_ref().to_path_buf();
    let data: Value = serde_json::from_str(&fs::read_to_string(&manifest_path)?)?;
    ensure(
        data["artifact_kind"].as_str() == Some("frog_native_kernel_manifest"),
        "unexpected native kernel manifest artifact_kind",
    )?;
    let kernel = data["kernel"]
        .as_object()
        .ok_or_else(|| RuntimeError::Message("native kernel manifest requires kernel object".to_string()))?;
    let entry = kernel
        .get("entry")
        .and_then(Value::as_object)
        .ok_or_else(|| RuntimeError::Message("native kernel manifest requires kernel.entry object".to_string()))?;
    let mut diagnostics_by_code = std::collections::BTreeMap::new();
    if let Some(error_codes) = kernel
        .get("error_model")
        .and_then(|item| item.get("error_codes"))
        .and_then(Value::as_array)
    {
        for item in error_codes {
            if let Some(code) = item.get("code").and_then(Value::as_u64) {
                let diagnostic = item
                    .get("diagnostic")
                    .or_else(|| item.get("meaning"))
                    .and_then(Value::as_str)
                    .unwrap_or("")
                    .to_string();
                diagnostics_by_code.insert(code as u16, diagnostic);
            }
        }
    }

    Ok(NativeKernelManifest {
        manifest_path,
        source_lowered_unit: kernel
            .get("source_lowered_unit")
            .and_then(Value::as_str)
            .unwrap_or("")
            .to_string(),
        entry_symbol: entry.get("symbol").and_then(Value::as_str).unwrap_or("").to_string(),
        abi: entry.get("abi").and_then(Value::as_str).unwrap_or("").to_string(),
        diagnostics_by_code,
    })
}

impl NativeKernelManifest {
    pub fn diagnostic(&self, error_code: u16) -> String {
        self.diagnostics_by_code
            .get(&error_code)
            .cloned()
            .unwrap_or_else(|| "native kernel execution failed.".to_string())
    }
}

impl NativeKernelBridge {
    pub fn from_paths(manifest_path: impl AsRef<Path>, library_path: impl AsRef<Path>) -> Result<Self> {
        ensure(
            std::mem::size_of::<FrogRunResult>() == 6,
            "FrogRunResult ABI layout must be 6 bytes",
        )?;
        let manifest = load_native_kernel_manifest(manifest_path)?;
        ensure(
            manifest.entry_symbol == "frog_example05_run",
            "unexpected native kernel entry symbol",
        )?;
        ensure(
            manifest.abi == "frog_u16_to_result_status_outptr",
            "NativeKernelBridge requires frog_u16_to_result_status_outptr",
        )?;
        let library = DynamicLibrary::open(library_path.as_ref())?;
        let entry_point = unsafe { library.symbol::<FrogNativeKernelFunction>(&manifest.entry_symbol)? };
        Ok(Self {
            manifest,
            library,
            entry_point,
        })
    }

    pub fn manifest(&self) -> &NativeKernelManifest {
        &self.manifest
    }

    pub fn run(&self, input_value: u16) -> NativeKernelResult {
        let _keep_alive = &self.library;
        let mut raw = FrogRunResult::default();
        unsafe {
            (self.entry_point)(input_value, &mut raw);
        }
        NativeKernelResult {
            ok: raw.ok != 0 && raw.error_code == 0,
            result: raw.result,
            error_code: raw.error_code,
        }
    }
}

impl NativeBoolKernelBridge {
    pub fn from_paths(manifest_path: impl AsRef<Path>, library_path: impl AsRef<Path>) -> Result<Self> {
        ensure(
            std::mem::size_of::<FrogBoolRunResult>() == 4,
            "FrogBoolRunResult ABI layout must be 4 bytes",
        )?;
        let manifest = load_native_kernel_manifest(manifest_path)?;
        ensure(
            manifest.entry_symbol == "frog_example06_run",
            "unexpected native bool kernel entry symbol",
        )?;
        ensure(
            manifest.abi == "frog_bool_to_result_status_outptr",
            "NativeBoolKernelBridge requires frog_bool_to_result_status_outptr",
        )?;
        let library = DynamicLibrary::open(library_path.as_ref())?;
        let entry_point = unsafe { library.symbol::<FrogNativeBoolKernelFunction>(&manifest.entry_symbol)? };
        Ok(Self {
            manifest,
            library,
            entry_point,
        })
    }

    pub fn manifest(&self) -> &NativeKernelManifest {
        &self.manifest
    }

    pub fn run(&self, input_value: bool) -> NativeBoolKernelResult {
        let _keep_alive = &self.library;
        let mut raw = FrogBoolRunResult::default();
        unsafe {
            (self.entry_point)(if input_value { 1 } else { 0 }, &mut raw);
        }
        NativeBoolKernelResult {
            ok: raw.ok != 0 && raw.error_code == 0,
            result: raw.result != 0,
            error_code: raw.error_code,
        }
    }
}

impl NativeStringKernelBridge {
    pub fn from_paths(manifest_path: impl AsRef<Path>, library_path: impl AsRef<Path>) -> Result<Self> {
        ensure(
            std::mem::size_of::<FrogStringRunResult>() == 264,
            "FrogStringRunResult ABI layout must be 264 bytes",
        )?;
        let manifest = load_native_kernel_manifest(manifest_path)?;
        ensure(
            manifest.entry_symbol == "frog_example07_run",
            "unexpected native string kernel entry symbol",
        )?;
        ensure(
            manifest.abi == "frog_string_utf8_256_to_result_status_outptr",
            "NativeStringKernelBridge requires frog_string_utf8_256_to_result_status_outptr",
        )?;
        let library = DynamicLibrary::open(library_path.as_ref())?;
        let entry_point = unsafe { library.symbol::<FrogNativeStringKernelFunction>(&manifest.entry_symbol)? };
        Ok(Self {
            manifest,
            library,
            entry_point,
        })
    }

    pub fn manifest(&self) -> &NativeKernelManifest {
        &self.manifest
    }

    pub fn run(&self, input_value: &str) -> NativeStringKernelResult {
        let _keep_alive = &self.library;
        let input = input_value.as_bytes();
        if input.len() > 256 {
            return NativeStringKernelResult {
                ok: false,
                result: String::new(),
                error_code: 1,
            };
        }
        let mut raw = FrogStringRunResult::default();
        unsafe {
            (self.entry_point)(input.as_ptr(), input.len() as u32, &mut raw);
        }
        let length = raw.result_len.min(256) as usize;
        NativeStringKernelResult {
            ok: raw.ok != 0 && raw.error_code == 0 && raw.result_len <= 256,
            result: String::from_utf8_lossy(&raw.result_buffer[..length]).to_string(),
            error_code: raw.error_code,
        }
    }
}

impl NativeEnumKernelBridge {
    pub fn from_paths(manifest_path: impl AsRef<Path>, library_path: impl AsRef<Path>) -> Result<Self> {
        ensure(
            std::mem::size_of::<FrogEnumRunResult>() == 6,
            "FrogEnumRunResult ABI layout must be 6 bytes",
        )?;
        let manifest = load_native_kernel_manifest(manifest_path)?;
        ensure(
            manifest.entry_symbol == "frog_example08_run",
            "unexpected native enum kernel entry symbol",
        )?;
        ensure(
            manifest.abi == "frog_enum_u16_to_result_status_outptr",
            "NativeEnumKernelBridge requires frog_enum_u16_to_result_status_outptr",
        )?;
        let library = DynamicLibrary::open(library_path.as_ref())?;
        let entry_point = unsafe { library.symbol::<FrogNativeEnumKernelFunction>(&manifest.entry_symbol)? };
        Ok(Self {
            manifest,
            library,
            entry_point,
        })
    }

    pub fn manifest(&self) -> &NativeKernelManifest {
        &self.manifest
    }

    pub fn run(&self, input_numeric_value: u16) -> NativeEnumKernelResult {
        let _keep_alive = &self.library;
        let mut raw = FrogEnumRunResult::default();
        unsafe {
            (self.entry_point)(input_numeric_value, &mut raw);
        }
        NativeEnumKernelResult {
            ok: raw.ok != 0 && raw.error_code == 0,
            result_numeric_value: raw.result,
            error_code: raw.error_code,
        }
    }
}

#[cfg(windows)]
struct DynamicLibrary {
    handle: *mut c_void,
}

#[cfg(windows)]
impl DynamicLibrary {
    fn open(path: &Path) -> Result<Self> {
        use std::os::windows::ffi::OsStrExt;

        let wide: Vec<u16> = path.as_os_str().encode_wide().chain(std::iter::once(0)).collect();
        let handle = unsafe { LoadLibraryW(wide.as_ptr()) };
        if handle.is_null() {
            return Err(RuntimeError::Message(format!(
                "unable to load native kernel library: {}",
                path.display()
            )));
        }
        Ok(Self { handle })
    }

    unsafe fn symbol<T: Copy>(&self, name: &str) -> Result<T> {
        let c_name = CString::new(name)
            .map_err(|_| RuntimeError::Message("native kernel symbol contains NUL byte".to_string()))?;
        let ptr = GetProcAddress(self.handle, c_name.as_ptr());
        if ptr.is_null() {
            return Err(RuntimeError::Message(format!("missing native kernel symbol: {name}")));
        }
        Ok(std::mem::transmute_copy::<*mut c_void, T>(&ptr))
    }
}

#[cfg(windows)]
impl Drop for DynamicLibrary {
    fn drop(&mut self) {
        if !self.handle.is_null() {
            unsafe {
                FreeLibrary(self.handle);
            }
        }
    }
}

#[cfg(windows)]
#[link(name = "kernel32")]
extern "system" {
    fn LoadLibraryW(lp_lib_file_name: *const u16) -> *mut c_void;
    fn GetProcAddress(h_module: *mut c_void, lp_proc_name: *const c_char) -> *mut c_void;
    fn FreeLibrary(h_lib_module: *mut c_void) -> i32;
}

#[cfg(not(windows))]
struct DynamicLibrary {
    handle: *mut c_void,
}

#[cfg(not(windows))]
impl DynamicLibrary {
    fn open(path: &Path) -> Result<Self> {
        let c_path = CString::new(path.to_string_lossy().as_bytes())
            .map_err(|_| RuntimeError::Message("native kernel path contains NUL byte".to_string()))?;
        let handle = unsafe { dlopen(c_path.as_ptr(), RTLD_NOW) };
        if handle.is_null() {
            return Err(RuntimeError::Message(format!(
                "unable to load native kernel library: {}",
                path.display()
            )));
        }
        Ok(Self { handle })
    }

    unsafe fn symbol<T: Copy>(&self, name: &str) -> Result<T> {
        let c_name = CString::new(name)
            .map_err(|_| RuntimeError::Message("native kernel symbol contains NUL byte".to_string()))?;
        let ptr = dlsym(self.handle, c_name.as_ptr());
        if ptr.is_null() {
            return Err(RuntimeError::Message(format!("missing native kernel symbol: {name}")));
        }
        Ok(std::mem::transmute_copy::<*mut c_void, T>(&ptr))
    }
}

#[cfg(not(windows))]
impl Drop for DynamicLibrary {
    fn drop(&mut self) {
        if !self.handle.is_null() {
            unsafe {
                dlclose(self.handle);
            }
        }
    }
}

#[cfg(not(windows))]
const RTLD_NOW: i32 = 2;

#[cfg(not(windows))]
#[link(name = "dl")]
extern "C" {
    fn dlopen(filename: *const c_char, flags: i32) -> *mut c_void;
    fn dlsym(handle: *mut c_void, symbol: *const c_char) -> *mut c_void;
    fn dlclose(handle: *mut c_void) -> i32;
}
