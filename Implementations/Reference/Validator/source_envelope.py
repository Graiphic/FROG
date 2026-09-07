"""Bounded source-envelope preflight; not a general JSON Schema interpreter.

The public schema owns required sections, allowed sections and current versions.
Section-local semantic checks remain in the reference validator. Tests compare
this projection against Draft 2020-12 so schema evolution cannot silently drift.
"""
from __future__ import annotations

import json
from functools import lru_cache
from pathlib import Path
from typing import Any


@lru_cache(maxsize=1)
def _schema() -> dict:
    root = Path(__file__).resolve().parents[3]
    return json.loads((root / "Expression/schema/frog.schema.json").read_text(encoding="utf-8"))


def validate_envelope(document: dict[str, Any]) -> tuple[str, list[dict[str, Any]]]:
    schema = _schema()

    def reject(code: str, message: str, location: str, status: str = "structural_invalid"):
        return status, [{"code": code, "message": message, "location": location, "severity": "error"}]

    missing = [key for key in schema["required"] if key not in document]
    if "spec_version" in missing:
        return reject("missing_top_level_sections", f"Missing top-level sections: {', '.join(missing)}.", "/")

    version = document["spec_version"]
    alternatives = schema["properties"]["spec_version"]["oneOf"]
    accepted_version = any(
        ((choice["type"] == "string" and isinstance(version, str)) or
         (choice["type"] == "number" and type(version) in (int, float))) and
        version in choice["enum"]
        for choice in alternatives
    )
    if not accepted_version:
        if isinstance(version, str) or type(version) in (int, float):
            return reject("unsupported_spec_version", "Source version is outside this reference reader's declared profile; "
                          "no validity or execution claim is made.", "/spec_version", "unsupported_source")
        return reject("invalid_spec_version", "spec_version must be a supported string or number.", "/spec_version")

    # An unknown version may define a different envelope. Apply current-version
    # required sections only after capability selection, never to future source.
    if missing:
        return reject("missing_top_level_sections", f"Missing top-level sections: {', '.join(missing)}.", "/")

    for key, value in document.items():
        if key not in schema["properties"]:
            return reject("unknown_top_level_section", f"Unknown section in the current source profile: {key}.", f"/{key}")
        if key != "spec_version" and not isinstance(value, dict):
            return reject("invalid_top_level_section_shape", f"Section '{key}' must be an object.", f"/{key}")

    # Metadata.name is required by Expression/Metadata.md, beyond the deliberately
    # conservative top-level schema. Fail with a diagnostic, not a late KeyError.
    metadata = document["metadata"]
    for key in ("name", "description"):
        if not isinstance(metadata.get(key), str):
            return reject(f"invalid_metadata_{key}", f"metadata.{key} must be present and be a string.", f"/metadata/{key}")
    for key in ("summary", "author", "program_version", "created", "updated", "license"):
        if key in metadata and not isinstance(metadata[key], str):
            return reject("invalid_metadata_field", f"metadata.{key} must be a string.", f"/metadata/{key}")
    if "tags" in metadata and (not isinstance(metadata["tags"], list) or
                                not all(isinstance(tag, str) for tag in metadata["tags"])):
        return reject("invalid_metadata_tags", "metadata.tags must be an array of strings.", "/metadata/tags")
    if "is_example" in metadata and not isinstance(metadata["is_example"], bool):
        return reject("invalid_metadata_field", "metadata.is_example must be Boolean.", "/metadata/is_example")

    for section in ("host", "execution_policy"):
        if section not in document:
            continue
        policy = document[section]
        policy_version = policy.get("version")
        if type(policy_version) not in (int, float) or policy_version != 1:
            return reject("unsupported_policy_version", f"Unsupported or malformed {section} version; no policy is guessed.",
                          f"/{section}/version", "unsupported_source")

    execution = document.get("execution_policy", {})
    for key in execution:
        if key not in {"version", "debugging_allowed"}:
            return reject("unsupported_execution_policy", f"Unsupported execution policy field: {key}.",
                          f"/execution_policy/{key}", "unsupported_source")
    if "debugging_allowed" in execution and not isinstance(execution["debugging_allowed"], bool):
        return reject("invalid_execution_policy", "debugging_allowed must be Boolean.", "/execution_policy/debugging_allowed")

    host = document.get("host", {})
    if "launch" in host:
        launch = host["launch"]
        if not isinstance(launch, dict) or ("trigger" in launch and launch["trigger"] not in ("manual", "on_open")):
            return reject("invalid_host_launch", "host.launch.trigger must be manual or on_open.", "/host/launch")
    if "front_panel_window" in host:
        window = host["front_panel_window"]
        if not isinstance(window, dict):
            return reject("invalid_host_window", "front_panel_window must be an object.", "/host/front_panel_window")
        enumerations = {
            "style": ("standard", "dialog"), "title_mode": ("program_name", "custom"),
            "initial_position": ("unchanged", "centered", "maximized", "minimized", "custom"),
        }
        for key, choices in enumerations.items():
            if key in window and window[key] not in choices:
                return reject("invalid_host_window", f"Invalid host window {key}.", f"/host/front_panel_window/{key}")
        for key in ("allow_close", "allow_resize", "allow_minimize", "menu_bar_visible", "toolbar_visible",
                    "vertical_scrollbar_visible", "horizontal_scrollbar_visible", "scroll_to_origin"):
            if key in window and not isinstance(window[key], bool):
                return reject("invalid_host_window", f"Host window {key} must be Boolean.", f"/host/front_panel_window/{key}")
        for key in ("minimum_width", "minimum_height"):
            if key in window and (type(window[key]) not in (int, float) or window[key] < 0):
                return reject("invalid_host_window", f"Host window {key} must be nonnegative.", f"/host/front_panel_window/{key}")
        if "custom_title" in window and not isinstance(window["custom_title"], str):
            return reject("invalid_host_window", "custom_title must be a string.", "/host/front_panel_window/custom_title")
    return "ok", []
