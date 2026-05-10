from __future__ import annotations

import ctypes
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any


class NativeKernelError(RuntimeError):
    """Raised when a native kernel manifest or call cannot be used safely."""


class FrogRunResult(ctypes.Structure):
    _fields_ = [
        ("ok", ctypes.c_uint8),
        ("result", ctypes.c_uint16),
        ("error_code", ctypes.c_uint16),
    ]


class FrogBoolRunResult(ctypes.Structure):
    _fields_ = [
        ("ok", ctypes.c_uint8),
        ("result", ctypes.c_uint8),
        ("error_code", ctypes.c_uint16),
    ]


@dataclass(frozen=True)
class NativeKernelManifest:
    manifest_path: Path
    source_lowered_unit: str
    entry_symbol: str
    abi: str
    diagnostics_by_code: dict[int, str]


@dataclass(frozen=True)
class NativeKernelResult:
    ok: bool
    result: int
    error_code: int
    diagnostic: str


@dataclass(frozen=True)
class NativeBoolKernelResult:
    ok: bool
    result: bool
    error_code: int
    diagnostic: str


def _load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_native_kernel_manifest(manifest_path: str | Path) -> NativeKernelManifest:
    path = Path(manifest_path).resolve()
    data = _load_json(path)
    if data.get("artifact_kind") != "frog_native_kernel_manifest":
        raise NativeKernelError("unexpected native kernel manifest artifact_kind")
    kernel = data.get("kernel")
    if not isinstance(kernel, dict):
        raise NativeKernelError("native kernel manifest requires kernel object")
    entry = kernel.get("entry")
    if not isinstance(entry, dict):
        raise NativeKernelError("native kernel manifest requires kernel.entry object")
    diagnostics: dict[int, str] = {}
    error_model = kernel.get("error_model")
    if isinstance(error_model, dict):
        for item in error_model.get("error_codes", []):
            if isinstance(item, dict) and isinstance(item.get("code"), int):
                diagnostics[int(item["code"])] = str(item.get("diagnostic") or item.get("meaning") or "")
    return NativeKernelManifest(
        manifest_path=path,
        source_lowered_unit=str(kernel.get("source_lowered_unit", "")),
        entry_symbol=str(entry.get("symbol", "")),
        abi=str(entry.get("abi", "")),
        diagnostics_by_code=diagnostics,
    )


def _diagnostic(manifest: NativeKernelManifest, error_code: int) -> str:
    return manifest.diagnostics_by_code.get(error_code, "native kernel execution failed.")


class NativeKernelBridge:
    def __init__(self, manifest: NativeKernelManifest, library_path: str | Path) -> None:
        if ctypes.sizeof(FrogRunResult) != 6:
            raise NativeKernelError("FrogRunResult ABI layout must be 6 bytes")
        if manifest.entry_symbol != "frog_example05_run":
            raise NativeKernelError("unexpected native kernel entry symbol")
        if manifest.abi != "frog_u16_to_result_status_outptr":
            raise NativeKernelError("NativeKernelBridge requires frog_u16_to_result_status_outptr")
        self._manifest = manifest
        self._library_path = Path(library_path).resolve()
        self._library = ctypes.CDLL(str(self._library_path))
        self._entry = getattr(self._library, manifest.entry_symbol)
        self._entry.argtypes = [ctypes.c_uint16, ctypes.POINTER(FrogRunResult)]
        self._entry.restype = None

    @property
    def manifest(self) -> NativeKernelManifest:
        return self._manifest

    @property
    def library_path(self) -> Path:
        return self._library_path

    def run(self, input_value: int) -> NativeKernelResult:
        if input_value < 0 or input_value > 65535:
            raise NativeKernelError("input_value must remain in the u16 domain.")
        raw = FrogRunResult()
        self._entry(ctypes.c_uint16(input_value), ctypes.byref(raw))
        error_code = int(raw.error_code)
        ok = bool(raw.ok) and error_code == 0
        return NativeKernelResult(
            ok=ok,
            result=int(raw.result),
            error_code=error_code,
            diagnostic="" if ok else _diagnostic(self._manifest, error_code),
        )


class NativeBoolKernelBridge:
    def __init__(self, manifest: NativeKernelManifest, library_path: str | Path) -> None:
        if ctypes.sizeof(FrogBoolRunResult) != 4:
            raise NativeKernelError("FrogBoolRunResult ABI layout must be 4 bytes")
        if manifest.entry_symbol != "frog_example06_run":
            raise NativeKernelError("unexpected native bool kernel entry symbol")
        if manifest.abi != "frog_bool_to_result_status_outptr":
            raise NativeKernelError("NativeBoolKernelBridge requires frog_bool_to_result_status_outptr")
        self._manifest = manifest
        self._library_path = Path(library_path).resolve()
        self._library = ctypes.CDLL(str(self._library_path))
        self._entry = getattr(self._library, manifest.entry_symbol)
        self._entry.argtypes = [ctypes.c_uint8, ctypes.POINTER(FrogBoolRunResult)]
        self._entry.restype = None

    @property
    def manifest(self) -> NativeKernelManifest:
        return self._manifest

    @property
    def library_path(self) -> Path:
        return self._library_path

    def run(self, input_value: bool) -> NativeBoolKernelResult:
        raw = FrogBoolRunResult()
        self._entry(ctypes.c_uint8(1 if input_value else 0), ctypes.byref(raw))
        error_code = int(raw.error_code)
        ok = bool(raw.ok) and error_code == 0
        return NativeBoolKernelResult(
            ok=ok,
            result=bool(raw.result),
            error_code=error_code,
            diagnostic="" if ok else _diagnostic(self._manifest, error_code),
        )


def load_native_kernel_bridge(manifest_path: str | Path, library_path: str | Path) -> NativeKernelBridge:
    return NativeKernelBridge(load_native_kernel_manifest(manifest_path), library_path)


def load_native_bool_kernel_bridge(manifest_path: str | Path, library_path: str | Path) -> NativeBoolKernelBridge:
    return NativeBoolKernelBridge(load_native_kernel_manifest(manifest_path), library_path)
