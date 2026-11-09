from __future__ import annotations

from pathlib import Path
from typing import Any, Dict

from Implementations.Reference.common import (
    DEMO_ARTIFACT_VERSION,
    FrogPipelineError,
    LoadedSource,
    load_json_file,
    sha256_text,
)


def _normalize_path(path: Path) -> str:
    return str(path).replace("\\", "/")


def load_source(file_path: str) -> LoadedSource:
    path = Path(file_path)

    try:
        text, document = load_json_file(path)
    except FrogPipelineError as exc:
        artifact = {
            "artifact_kind": "frog_loaded_source",
            "artifact_version": DEMO_ARTIFACT_VERSION,
            "stage_ref": "load",
            "status": "load_failed",
            "source": {
                "path": _normalize_path(path),
                "content_hash": None,
                "spec_version": None,
            },
            "document": None,
            "diagnostics": [exc.as_dict()],
        }
        return LoadedSource(artifact=artifact)

    artifact: Dict[str, Any] = {
        "artifact_kind": "frog_loaded_source",
        "artifact_version": DEMO_ARTIFACT_VERSION,
        "stage_ref": "load",
        "status": "ok",
        "source": {
            "path": _normalize_path(path),
            "content_hash": sha256_text(text),
            "spec_version": document.get("spec_version"),
        },
        "document": document,
        "diagnostics": [],
    }

    return LoadedSource(artifact=artifact)
