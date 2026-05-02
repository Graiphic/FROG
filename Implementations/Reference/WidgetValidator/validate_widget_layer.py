#!/usr/bin/env python3
"""Validate the published FROG widget and Default realization layer.

This tool is non-normative. It checks repository hygiene only.
Run from the repository root:

    python Implementations/Reference/WidgetValidator/validate_widget_layer.py
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]

FAMILIES: dict[str, dict[str, Any]] = {
    "boolean": {"widget_doc": "Boolean.md", "default_doc": "Boolean.md", "manifest": "boolean.default.wfrog", "classes": ["frog.widgets.boolean_control", "frog.widgets.boolean_indicator"]},
    "string": {"widget_doc": "String.md", "default_doc": "String.md", "manifest": "string.default.wfrog", "classes": ["frog.widgets.string_control", "frog.widgets.string_indicator"]},
    "button": {"widget_doc": "Button.md", "default_doc": "Button.md", "manifest": "button.default.wfrog", "classes": ["frog.widgets.button"]},
    "numeric": {"widget_doc": "Numeric.md", "default_doc": "Numeric.md", "manifest": "numeric.default.wfrog", "classes": ["frog.widgets.numeric_control", "frog.widgets.numeric_indicator"]},
    "enum": {"widget_doc": "Enum.md", "default_doc": "Enum.md", "manifest": "enum.default.wfrog", "classes": ["frog.widgets.enum_control", "frog.widgets.enum_indicator"]},
    "path": {"widget_doc": "Path.md", "default_doc": "Path.md", "manifest": "path.default.wfrog", "classes": ["frog.widgets.path_control", "frog.widgets.path_indicator"]},
    "picture": {"widget_doc": "Picture.md", "default_doc": "Picture.md", "manifest": "picture.default.wfrog", "classes": ["frog.widgets.picture_control", "frog.widgets.picture_indicator"]},
    "chart": {"widget_doc": "Chart.md", "default_doc": "Chart.md", "manifest": "chart.default.wfrog", "classes": ["frog.widgets.waveform_chart"]},
    "listbox": {"widget_doc": "Listbox.md", "default_doc": "Listbox.md", "manifest": "listbox.default.wfrog", "classes": ["frog.widgets.listbox_control", "frog.widgets.listbox_indicator"]},
    "tab": {"widget_doc": "Tab.md", "default_doc": "Tab.md", "manifest": "tab.default.wfrog", "classes": ["frog.widgets.tab_control", "frog.widgets.tab_indicator"]},
    "tree": {"widget_doc": "Tree.md", "default_doc": "Tree.md", "manifest": "tree.default.wfrog", "classes": ["frog.widgets.tree_control", "frog.widgets.tree_indicator"]},
    "table": {"widget_doc": "Table.md", "default_doc": "Table.md", "manifest": "table.default.wfrog", "classes": ["frog.widgets.table_control", "frog.widgets.table_indicator"]},
    "array": {"widget_doc": "Array.md", "default_doc": "Array.md", "manifest": "array.default.wfrog", "classes": ["frog.widgets.array"]},
    "cluster": {"widget_doc": "Cluster.md", "default_doc": "Cluster.md", "manifest": "cluster.default.wfrog", "classes": ["frog.widgets.cluster"]},
    "label": {"widget_doc": "Label.md", "default_doc": "Label.md", "manifest": "label.default.wfrog", "classes": ["frog.widgets.label"]},
    "frame": {"widget_doc": "Frame.md", "default_doc": "Frame.md", "manifest": "frame.default.wfrog", "classes": ["frog.widgets.frame"]},
    "decorations": {"widget_doc": "Decorations.md", "default_doc": "Decorations.md", "manifest": "decorations.default.wfrog", "classes": ["frog.widgets.flat_box", "frog.widgets.horizontal_line", "frog.widgets.vertical_line"]},
    "splitter": {"widget_doc": "Splitter.md", "default_doc": "Splitter.md", "manifest": "splitter.default.wfrog", "classes": ["frog.widgets.horizontal_splitter", "frog.widgets.vertical_splitter"]},
    "panel": {"widget_doc": "Panel.md", "default_doc": "Panel.md", "manifest": "panel.default.wfrog", "classes": ["frog.widgets.panel", "frog.widgets.subpanel"]},
}


def error(errors: list[str], message: str) -> None:
    errors.append(message)


def warn(warnings: list[str], message: str) -> None:
    warnings.append(message)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_manifest(path: Path, errors: list[str]) -> dict[str, Any] | None:
    try:
        return json.loads(read_text(path))
    except Exception as exc:
        error(errors, f"{path}: invalid JSON: {exc}")
        return None


def extract_resources(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    resources = manifest.get("resources", [])
    return resources if isinstance(resources, list) else []


def extract_supported_parts(manifest: dict[str, Any]) -> set[str]:
    parts: set[str] = set()
    for realization in manifest.get("realizations", []):
        if isinstance(realization, dict):
            for part in realization.get("supported_parts", []) or []:
                if isinstance(part, str):
                    parts.add(part)
    return parts


def svg_parts(text: str) -> set[str]:
    parts = set(re.findall(r"data-frog-part=[\"']([^\"']+)[\"']", text))
    return parts


def main() -> int:
    errors: list[str] = []
    warnings: list[str] = []

    widgets_dir = ROOT / "Libraries" / "Widgets"
    default_dir = ROOT / "Libraries" / "Realizations" / "Default"

    for doc in [
        default_dir / "Package.md",
        default_dir / "Readme.md",
        widgets_dir / "Readme.md",
    ]:
        if not doc.exists():
            error(errors, f"missing required document: {doc}")
            continue
        text = read_text(doc)
        if "\\n" in text:
            error(errors, f"{doc}: contains literal escaped newline sequence '\\\\n'")

    for family, spec in FAMILIES.items():
        widget_doc = widgets_dir / spec["widget_doc"]
        default_doc = default_dir / spec["default_doc"]
        manifest_path = default_dir / spec["manifest"]

        if not widget_doc.exists():
            error(errors, f"{family}: missing widget class-law document {widget_doc}")
            continue
        if not default_doc.exists():
            error(errors, f"{family}: missing Default realization document {default_doc}")
        if not manifest_path.exists():
            error(errors, f"{family}: missing manifest {manifest_path}")
            continue

        widget_text = read_text(widget_doc)
        for class_id in spec["classes"]:
            if class_id not in widget_text:
                error(errors, f"{family}: class {class_id} not found in {widget_doc}")

        manifest_data = load_manifest(manifest_path, errors)
        if manifest_data is None:
            continue

        for key in ["format", "kind", "package", "targets", "resources", "validation_expectations"]:
            if key not in manifest_data:
                error(errors, f"{manifest_path}: missing top-level key {key}")

        targets = manifest_data.get("targets", [])
        if isinstance(targets, list):
            for target in targets:
                if not isinstance(target, dict):
                    error(errors, f"{manifest_path}: target entry is not an object")
                    continue
                class_id = target.get("class_id")
                if class_id and class_id not in spec["classes"]:
                    warn(warnings, f"{manifest_path}: target class {class_id} not listed in validator family spec")
                role = target.get("role")
                if role == "widget":
                    warn(warnings, f"{manifest_path}: target {class_id} still uses generic role 'widget'")
                if role is None:
                    warn(warnings, f"{manifest_path}: target {class_id} has no explicit role")
        else:
            error(errors, f"{manifest_path}: targets must be an array")

        resources = extract_resources(manifest_data)
        supported_parts = extract_supported_parts(manifest_data)
        all_svg_parts: set[str] = set()

        for resource in resources:
            if not isinstance(resource, dict):
                error(errors, f"{manifest_path}: resource entry is not an object")
                continue
            rel_path = resource.get("path")
            if not rel_path:
                error(errors, f"{manifest_path}: resource without path")
                continue
            res_path = (default_dir / rel_path).resolve()
            try:
                res_path.relative_to(default_dir.resolve())
            except ValueError:
                error(errors, f"{manifest_path}: resource escapes Default directory: {rel_path}")
                continue
            if not res_path.exists():
                error(errors, f"{manifest_path}: missing resource {rel_path}")
                continue
            if res_path.suffix.lower() == ".svg":
                all_svg_parts |= svg_parts(read_text(res_path))

        if all_svg_parts and supported_parts:
            # Some abstract parts may be host-native only, so this is a warning rather than an error.
            missing_markers = sorted(part for part in supported_parts if part not in all_svg_parts)
            for part in missing_markers:
                warn(warnings, f"{manifest_path}: supported part '{part}' has no data-frog-part marker in declared SVG resources")

    print("FROG widget layer validation")
    print("============================")
    print(f"Repository root: {ROOT}")
    print(f"Families checked: {len(FAMILIES)}")
    print(f"Errors: {len(errors)}")
    print(f"Warnings: {len(warnings)}")

    if warnings:
        print("\nWarnings:")
        for item in warnings:
            print(f"  - {item}")

    if errors:
        print("\nErrors:")
        for item in errors:
            print(f"  - {item}")
        return 1

    print("\nStatus: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
