"""Shared Boolean UI-runtime artifact construction for the reference workspace."""

from __future__ import annotations

from typing import Any


class BooleanRuntimeModelError(RuntimeError):
    """Raised when the Boolean realization model is incomplete."""


def merge_default_widget_properties(panel: dict[str, Any], wfrog: dict[str, Any]) -> dict[str, Any]:
    defaults = wfrog.get("default_widget_properties", [])
    if not isinstance(defaults, list):
        raise BooleanRuntimeModelError("default_widget_properties must be an array")

    widgets = panel.get("widgets")
    if not isinstance(widgets, list):
        raise BooleanRuntimeModelError("panel.widgets must be an array")

    merged_panel = dict(panel)
    merged_widgets: list[dict[str, Any]] = []
    for raw_widget in widgets:
        if not isinstance(raw_widget, dict):
            raise BooleanRuntimeModelError("panel widget entries must be objects")
        widget = dict(raw_widget)
        class_ref = str(widget.get("class_ref", ""))
        role = str(widget.get("role", ""))
        props: dict[str, Any] = {}
        visual: dict[str, Any] = {}
        for raw_default in defaults:
            if not isinstance(raw_default, dict):
                raise BooleanRuntimeModelError("default_widget_properties entries must be objects")
            default_class = raw_default.get("class_id") or raw_default.get("target_class")
            default_role = raw_default.get("role", "")
            if default_class != class_ref or (default_role and default_role != role):
                continue
            default_visual = raw_default.get("visual", {})
            default_props = raw_default.get("props", raw_default.get("properties", {}))
            if not isinstance(default_visual, dict) or not isinstance(default_props, dict):
                raise BooleanRuntimeModelError("Boolean widget defaults must contain object props and visual members")
            visual.update(default_visual)
            props.update(default_props)
        visual.update(widget.get("visual", {}))
        props.update(widget.get("props", {}))
        widget["visual"] = visual
        widget["props"] = props
        merged_widgets.append(widget)
    merged_panel["widgets"] = merged_widgets
    return merged_panel


def boolean_runtime_properties(widget: dict[str, Any], value: bool) -> dict[str, Any]:
    props = widget.get("props")
    visual = widget.get("visual")
    if not isinstance(props, dict) or not isinstance(visual, dict):
        raise BooleanRuntimeModelError("Boolean widgets must expose props and visual objects")
    runtime = {
        key: property_value
        for key, property_value in props.items()
        if not key.startswith("interaction.") and key != "value"
    }
    runtime["asset_ref"] = visual.get("asset_ref")
    runtime["value"] = value
    return runtime


def build_boolean_roundtrip_artifact(
    contract: dict[str, Any],
    panel: dict[str, Any],
    input_value: bool,
    *,
    unit: dict[str, Any] | None = None,
) -> dict[str, Any]:
    if unit is None:
        units = contract.get("units")
        if not isinstance(units, list) or len(units) != 1 or not isinstance(units[0], dict):
            raise BooleanRuntimeModelError("Boolean contract must contain exactly one unit")
        unit = units[0]

    widgets = panel.get("widgets")
    if not isinstance(widgets, list):
        raise BooleanRuntimeModelError("Boolean panel.widgets must be an array")
    widgets_by_id = {
        str(widget.get("instance_id")): widget
        for widget in widgets
        if isinstance(widget, dict) and isinstance(widget.get("instance_id"), str)
    }
    for widget_id in ("bool_input", "bool_result"):
        if widget_id not in widgets_by_id:
            raise BooleanRuntimeModelError(f"Boolean panel is missing widget {widget_id}")

    result = bool(input_value)
    return {
        "artifact_kind": "frog_runtime_execution_result",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "status": "ok",
        "contract_ref": {
            "unit_ids": [unit.get("unit_id")],
            "backend_family": contract.get("backend_family"),
            "source_ref": contract.get("source_ref"),
        },
        "execution_summary": {
            "mode": "boolean_value_roundtrip",
            "executed_unit": unit.get("unit_id"),
            "operation": "copy",
            "input_value": result,
            "result": result,
        },
        "outputs": {"public": {"result": result}, "ui": {"bool_input": result, "bool_result": result}},
        "ui_runtime": {
            "panel": {
                "panel_id": panel.get("panel_id"),
                "title": panel.get("title"),
                "class_ref": panel.get("class_ref"),
                "layout": panel.get("layout"),
            },
            "widgets": [
                {
                    "widget_id": "bool_input",
                    "class_ref": widgets_by_id["bool_input"].get("class_ref"),
                    "role": "control",
                    "layout": widgets_by_id["bool_input"].get("layout"),
                    "runtime": boolean_runtime_properties(widgets_by_id["bool_input"], result),
                },
                {
                    "widget_id": "bool_result",
                    "class_ref": widgets_by_id["bool_result"].get("class_ref"),
                    "role": "indicator",
                    "layout": widgets_by_id["bool_result"].get("layout"),
                    "runtime": boolean_runtime_properties(widgets_by_id["bool_result"], result),
                },
            ],
        },
        "diagnostics": [],
    }
