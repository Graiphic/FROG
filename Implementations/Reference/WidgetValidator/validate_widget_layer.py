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
from typing import Any, Iterable

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

REQUIRED_MANIFEST_KEYS = [
    "format",
    "kind",
    "package",
    "targets",
    "resources",
    "validation_expectations",
]

RECOMMENDED_MANIFEST_KEYS = [
    "publication_role",
    "version_governance_ref",
    "ownership_boundary",
    "exports",
    "realizations",
    "part_bindings",
    "host_hints",
]

GENERIC_ROLES = {"widget"}


def error(errors: list[str], message: str) -> None:
    errors.append(message)


def warn(warnings: list[str], message: str) -> None:
    warnings.append(message)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_manifest(path: Path, errors: list[str]) -> dict[str, Any] | None:
    try:
        data = json.loads(read_text(path))
    except Exception as exc:
        error(errors, f"{path}: invalid JSON: {exc}")
        return None
    if not isinstance(data, dict):
        error(errors, f"{path}: manifest root must be a JSON object")
        return None
    return data


def as_list(value: Any) -> list[Any]:
    if value is None:
        return []
    if isinstance(value, list):
        return value
    return [value]


def string_items(values: Iterable[Any]) -> list[str]:
    return [value for value in values if isinstance(value, str)]


def extract_resources(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    resources = manifest.get("resources", [])
    return resources if isinstance(resources, list) else []


def extract_supported_parts(manifest: dict[str, Any]) -> set[str]:
    parts: set[str] = set()
    for realization in manifest.get("realizations", []) or []:
        if isinstance(realization, dict):
            parts.update(string_items(realization.get("supported_parts", []) or []))
    return parts


def extract_declared_resource_ids(manifest: dict[str, Any]) -> set[str]:
    ids: set[str] = set()
    for resource in extract_resources(manifest):
        resource_id = resource.get("id")
        if isinstance(resource_id, str):
            ids.add(resource_id)
    return ids


def extract_exported_resource_ids(manifest: dict[str, Any]) -> set[str]:
    exports = manifest.get("exports", {})
    if not isinstance(exports, dict):
        return set()
    return set(string_items(exports.get("resources", []) or []))


def extract_resource_part_refs(resource: dict[str, Any]) -> set[str]:
    refs: set[str] = set()
    target_part = resource.get("target_part")
    if isinstance(target_part, str):
        refs.add(target_part)
    refs.update(string_items(resource.get("target_parts", []) or []))
    return refs


def extract_composition_resource_refs(manifest: dict[str, Any]) -> set[str]:
    refs: set[str] = set()
    composition = manifest.get("composition")
    if not isinstance(composition, dict):
        return refs

    shell = composition.get("shell")
    if isinstance(shell, str):
        refs.add(shell)

    for section in ["slots", "nested_slots"]:
        for slot in composition.get(section, []) or []:
            if isinstance(slot, dict):
                resource_ref = slot.get("resource_ref")
                if isinstance(resource_ref, str):
                    refs.add(resource_ref)
    return refs


def svg_parts(text: str) -> set[str]:
    return set(re.findall(r"data-frog-part=[\"']([^\"']+)[\"']", text))


def svg_slots(text: str) -> set[str]:
    return set(re.findall(r"data-frog-slot=[\"']([^\"']+)[\"']", text))


def validate_document_text(path: Path, errors: list[str]) -> None:
    if not path.exists():
        error(errors, f"missing required document: {path}")
        return
    text = read_text(path)
    if "\\n" in text:
        error(errors, f"{path}: contains literal escaped newline sequence '\\\\n'")


def validate_manifest_shape(manifest_path: Path, manifest: dict[str, Any], warnings: list[str], errors: list[str]) -> None:
    for key in REQUIRED_MANIFEST_KEYS:
        if key not in manifest:
            error(errors, f"{manifest_path}: missing top-level key {key}")
    for key in RECOMMENDED_MANIFEST_KEYS:
        if key not in manifest:
            warn(warnings, f"{manifest_path}: missing recommended top-level key {key}")

    if manifest.get("format") != "frog.wfrog":
        warn(warnings, f"{manifest_path}: unexpected format {manifest.get('format')!r}")
    if manifest.get("kind") != "widget_realization_library":
        warn(warnings, f"{manifest_path}: unexpected kind {manifest.get('kind')!r}")

    package = manifest.get("package")
    if not isinstance(package, dict):
        error(errors, f"{manifest_path}: package must be an object")
    else:
        for key in ["id", "name", "namespace", "family", "category", "role_posture", "summary"]:
            if key not in package:
                warn(warnings, f"{manifest_path}: package missing recommended key {key}")

    if "version_governance_ref" in manifest and not isinstance(manifest.get("version_governance_ref"), str):
        error(errors, f"{manifest_path}: version_governance_ref must be a string")

    ownership_boundary = manifest.get("ownership_boundary")
    if ownership_boundary is not None and not isinstance(ownership_boundary, dict):
        error(errors, f"{manifest_path}: ownership_boundary must be an object")


def validate_targets(
    family: str,
    spec: dict[str, Any],
    widget_text: str,
    manifest_path: Path,
    manifest: dict[str, Any],
    warnings: list[str],
    errors: list[str],
) -> None:
    targets = manifest.get("targets", [])
    if not isinstance(targets, list):
        error(errors, f"{manifest_path}: targets must be an array")
        return

    seen_classes: set[str] = set()
    for target in targets:
        if not isinstance(target, dict):
            error(errors, f"{manifest_path}: target entry is not an object")
            continue
        class_id = target.get("class_id")
        if not isinstance(class_id, str):
            error(errors, f"{manifest_path}: target entry missing string class_id")
            continue
        seen_classes.add(class_id)
        if class_id not in spec["classes"]:
            warn(warnings, f"{manifest_path}: target class {class_id} not listed in validator family spec for {family}")
        if class_id not in widget_text:
            error(errors, f"{manifest_path}: target class {class_id} not found in {spec['widget_doc']}")

        role = target.get("role")
        if not isinstance(role, str):
            warn(warnings, f"{manifest_path}: target {class_id} has no explicit role")
        elif role in GENERIC_ROLES:
            warn(warnings, f"{manifest_path}: target {class_id} still uses generic role {role!r}")

    missing_targets = sorted(set(spec["classes"]) - seen_classes)
    for class_id in missing_targets:
        warn(warnings, f"{manifest_path}: validator family class {class_id} is not declared as a target")


def validate_public_parts(
    widget_text: str,
    manifest_path: Path,
    manifest: dict[str, Any],
    warnings: list[str],
    errors: list[str],
) -> None:
    supported_parts = extract_supported_parts(manifest)
    if not supported_parts:
        warn(warnings, f"{manifest_path}: no supported_parts declared under realizations")

    for part in sorted(supported_parts):
        if part not in widget_text:
            warn(warnings, f"{manifest_path}: supported part {part!r} not found in widget class-law document")

    for binding in manifest.get("part_bindings", []) or []:
        if not isinstance(binding, dict):
            error(errors, f"{manifest_path}: part binding entry is not an object")
            continue
        part = binding.get("part")
        if not isinstance(part, str):
            error(errors, f"{manifest_path}: part binding without string part")
            continue
        if supported_parts and part not in supported_parts:
            error(errors, f"{manifest_path}: part binding references unsupported public part {part!r}")

    for binding_group, part_key in [
        ("event_bindings", "source_part"),
        ("property_bindings", "target_part"),
    ]:
        for binding in manifest.get(binding_group, []) or []:
            if not isinstance(binding, dict):
                error(errors, f"{manifest_path}: {binding_group} entry is not an object")
                continue
            part = binding.get(part_key)
            if part is None:
                continue
            if not isinstance(part, str):
                error(errors, f"{manifest_path}: {binding_group} {part_key} must be a string")
                continue
            if supported_parts and part not in supported_parts:
                error(errors, f"{manifest_path}: {binding_group} references unsupported public part {part!r}")


def validate_resources(
    default_dir: Path,
    manifest_path: Path,
    manifest: dict[str, Any],
    warnings: list[str],
    errors: list[str],
) -> None:
    resources = extract_resources(manifest)
    if not isinstance(manifest.get("resources", []), list):
        error(errors, f"{manifest_path}: resources must be an array")
        return

    supported_parts = extract_supported_parts(manifest)
    declared_resource_ids = extract_declared_resource_ids(manifest)

    exported_resource_ids = extract_exported_resource_ids(manifest)
    for resource_id in sorted(exported_resource_ids - declared_resource_ids):
        error(errors, f"{manifest_path}: exported resource {resource_id!r} is not declared in resources")

    for resource_id in sorted(extract_composition_resource_refs(manifest) - declared_resource_ids):
        error(errors, f"{manifest_path}: composition references undeclared resource {resource_id!r}")

    all_svg_parts: set[str] = set()
    all_svg_slots: set[str] = set()

    for resource in resources:
        if not isinstance(resource, dict):
            error(errors, f"{manifest_path}: resource entry is not an object")
            continue

        resource_id = resource.get("id")
        if not isinstance(resource_id, str):
            error(errors, f"{manifest_path}: resource without string id")
        kind = resource.get("kind")
        if not isinstance(kind, str):
            warn(warnings, f"{manifest_path}: resource {resource_id!r} has no string kind")

        for part in sorted(extract_resource_part_refs(resource)):
            if supported_parts and part not in supported_parts:
                error(errors, f"{manifest_path}: resource {resource_id!r} references unsupported target part {part!r}")

        rel_path = resource.get("path")
        if not isinstance(rel_path, str) or not rel_path:
            error(errors, f"{manifest_path}: resource {resource_id!r} without path")
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
            svg_text = read_text(res_path)
            all_svg_parts |= svg_parts(svg_text)
            all_svg_slots |= svg_slots(svg_text)
            target_parts = extract_resource_part_refs(resource)
            for part in sorted(target_parts):
                if part not in svg_text:
                    warn(warnings, f"{manifest_path}: resource {rel_path} targets part {part!r} but the SVG does not mention that part")

    if all_svg_parts and supported_parts:
        missing_markers = sorted(part for part in supported_parts if part not in all_svg_parts)
        for part in missing_markers:
            warn(warnings, f"{manifest_path}: supported part {part!r} has no data-frog-part marker in declared SVG resources")

    composition = manifest.get("composition")
    if isinstance(composition, dict):
        for section in ["slots", "nested_slots"]:
            for slot in composition.get(section, []) or []:
                if not isinstance(slot, dict):
                    error(errors, f"{manifest_path}: composition {section} entry is not an object")
                    continue
                slot_name = slot.get("slot")
                if isinstance(slot_name, str) and supported_parts and slot_name not in supported_parts:
                    error(errors, f"{manifest_path}: composition slot {slot_name!r} is not a supported public part")
                parent_part = slot.get("parent_part")
                if isinstance(parent_part, str) and supported_parts and parent_part not in supported_parts:
                    error(errors, f"{manifest_path}: composition parent_part {parent_part!r} is not a supported public part")


def main() -> int:
    errors: list[str] = []
    warnings: list[str] = []

    widgets_dir = ROOT / "Libraries" / "Widgets"
    default_dir = ROOT / "Libraries" / "Realizations" / "Default"

    for doc in [
        widgets_dir / "Readme.md",
        default_dir / "Readme.md",
        default_dir / "Package.md",
        default_dir / "Validation.md",
    ]:
        validate_document_text(doc, errors)

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

        validate_manifest_shape(manifest_path, manifest_data, warnings, errors)
        validate_targets(family, spec, widget_text, manifest_path, manifest_data, warnings, errors)
        validate_public_parts(widget_text, manifest_path, manifest_data, warnings, errors)
        validate_resources(default_dir, manifest_path, manifest_data, warnings, errors)

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
