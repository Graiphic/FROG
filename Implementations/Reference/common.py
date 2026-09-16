from __future__ import annotations

import hashlib
import json
import math
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple


DEMO_ARTIFACT_VERSION = "0.1"

# Conservative internal reference family used by the current Python runtime path.
DEFAULT_BACKEND_FAMILY = "reference_host_runtime_ui_binding"

# First explicit LLVM-oriented backend-family identifier for downstream contract emission.
LLVM_BACKEND_FAMILY = "llvm_cpu_v1"

SUPPORTED_SCALAR_TYPES = {
    "u16",
    "i32",
    "i64",
    "f64",
    "string",
    "frog.ui.color",
}


class FrogPipelineError(Exception):
    def __init__(
        self,
        stage: str,
        error_code: str,
        message: str,
        diagnostics: Optional[List[Dict[str, Any]]] = None,
    ) -> None:
        super().__init__(message)
        self.stage = stage
        self.error_code = error_code
        self.message = message
        self.diagnostics = diagnostics or []

    def as_dict(self) -> Dict[str, Any]:
        return {
            "artifact_kind": "frog_pipeline_error",
            "artifact_version": DEMO_ARTIFACT_VERSION,
            "status": "error",
            "stage": self.stage,
            "error_code": self.error_code,
            "message": self.message,
            "diagnostics": self.diagnostics,
        }


@dataclass
class LoadedSource:
    artifact: Dict[str, Any]


@dataclass
class ValidationResult:
    artifact: Dict[str, Any]


@dataclass
class DerivedIR:
    artifact: Dict[str, Any]


@dataclass
class LoweredForm:
    artifact: Dict[str, Any]


@dataclass
class BackendContract:
    artifact: Dict[str, Any]


@dataclass
class RuntimeResult:
    artifact: Dict[str, Any]


def sha256_text(text: str) -> str:
    return "sha256:" + hashlib.sha256(text.encode("utf-8")).hexdigest()


def load_json_file(path: Path) -> Tuple[str, Any]:
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise FrogPipelineError(
            stage="load",
            error_code="invalid_utf8",
            message=f"Source must be valid UTF-8: {path}",
        ) from exc
    except FileNotFoundError as exc:
        raise FrogPipelineError(
            stage="load",
            error_code="source_not_found",
            message=f"Source file not found: {path}",
        ) from exc
    except OSError as exc:
        raise FrogPipelineError(
            stage="load",
            error_code="source_read_error",
            message=f"Unable to read source file: {path}",
        ) from exc

    try:
        document = json.loads(
            text,
            object_pairs_hook=_unique_json_members,
            parse_constant=_reject_non_json_number,
            parse_float=_finite_json_float,
            parse_int=_exact_json_integer,
        )
    except RecursionError as exc:
        raise FrogPipelineError(
            stage="load", error_code="source_resource_limit",
            message="JSON nesting exceeds the reference reader's capacity.",
        ) from exc
    except json.JSONDecodeError as exc:
        raise FrogPipelineError(
            stage="load",
            error_code="invalid_json",
            message=f"Invalid JSON in source file: {path}",
            diagnostics=[
                {
                    "severity": "error",
                    "message": str(exc),
                    "source_anchor": {
                        "line": exc.lineno,
                        "column": exc.colno,
                    },
                }
            ],
        ) from exc

    if not isinstance(document, dict):
        raise FrogPipelineError(
            stage="load",
            error_code="invalid_top_level",
            message="A .frog document must decode to a JSON object.",
        )

    return text, document


def _unique_json_members(pairs: List[Tuple[str, Any]]) -> Dict[str, Any]:
    result: Dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise FrogPipelineError(
                stage="load", error_code="duplicate_json_member",
                message=f"Duplicate JSON object member: {key!r}",
            )
        result[key] = value
    return result


def _reject_non_json_number(token: str) -> Any:
    raise FrogPipelineError(
        stage="load", error_code="invalid_json_number",
        message=f"Non-JSON numeric token is not permitted: {token}",
    )


def _finite_json_float(token: str) -> float:
    value = float(token)
    nonzero_underflow = value == 0.0 and any(
        digit in "123456789" for digit in token.lower().split("e", 1)[0]
    )
    if not math.isfinite(value) or nonzero_underflow:
        raise FrogPipelineError(
            stage="load", error_code="unsupported_json_number",
            message="JSON number exceeds the reference reader's finite binary64 range; "
                    "it must not silently become infinity or zero.",
        )
    return value


def _exact_json_integer(token: str) -> int:
    try:
        return int(token)
    except ValueError as exc:
        raise FrogPipelineError(
            stage="load", error_code="unsupported_json_number",
            message="JSON integer exceeds the reference reader's configured digit limit.",
        ) from exc


def require_keys(obj: Dict[str, Any], keys: List[str], *, stage: str, context: str) -> None:
    missing = [key for key in keys if key not in obj]
    if missing:
        raise FrogPipelineError(
            stage=stage,
            error_code="missing_required_keys",
            message=f"Missing required keys in {context}: {', '.join(missing)}",
        )


def ensure(
    condition: bool,
    *,
    stage: str,
    error_code: str,
    message: str,
    diagnostics: Optional[List[Dict[str, Any]]] = None,
) -> None:
    if not condition:
        raise FrogPipelineError(
            stage=stage,
            error_code=error_code,
            message=message,
            diagnostics=diagnostics,
        )


def program_id_from_path(path: Path) -> str:
    return f"prog:{path.parent.name or path.stem}"
