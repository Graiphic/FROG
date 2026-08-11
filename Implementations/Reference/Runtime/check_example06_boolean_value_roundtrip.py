#!/usr/bin/env python3
"""Check Example 06 Boolean value roundtrip realization fidelity.

This check is intentionally narrow. It validates Example 06 as a bounded
runtime/UI-binding and native bool-kernel pilot without generalizing Deriver,
Lowerer, ContractEmitter, widget rendering, or compiler backend behavior.

The check is intentionally stricter than an ID/layout/value smoke test:
the visible front panel must consume the .wfrog-declared asset references and
the Default Boolean realization assets. A hardcoded CSS-only fallback must not
pass this official validation.
"""

from __future__ import annotations

import argparse
import html
import json
import re
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from Implementations.Reference.Runtime.boolean_runtime import (  # noqa: E402
    build_boolean_roundtrip_artifact as execute_boolean_roundtrip,
    merge_default_widget_properties,
)

DEFAULT_ACCEPTANCE = (
    ROOT
    / "Implementations"
    / "Reference"
    / "Runtime"
    / "acceptance"
    / "example06_boolean_value_roundtrip.acceptance.json"
)


class Example06CheckError(RuntimeError):
    pass


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise Example06CheckError(f"missing file: {path.relative_to(ROOT)}") from exc
    except json.JSONDecodeError as exc:
        raise Example06CheckError(f"invalid JSON in {path.relative_to(ROOT)}: {exc}") from exc
    if not isinstance(data, dict):
        raise Example06CheckError(f"expected JSON object: {path.relative_to(ROOT)}")
    return data


def read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except FileNotFoundError as exc:
        raise Example06CheckError(f"missing file: {path.relative_to(ROOT)}") from exc


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def repo_path(path_text: str) -> Path:
    path = Path(path_text)
    return path if path.is_absolute() else ROOT / path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise Example06CheckError(message)


def single_unit(artifact: dict[str, Any], key: str = "units") -> dict[str, Any]:
    units = artifact.get(key)
    require(isinstance(units, list) and len(units) == 1 and isinstance(units[0], dict), f"expected exactly one {key} entry")
    return units[0]


def asset_id_from_ref(asset_ref: str) -> str:
    require(isinstance(asset_ref, str) and asset_ref.startswith("asset:"), f"invalid asset_ref: {asset_ref!r}")
    return asset_ref[len("asset:") :]


def asset_route(asset_id: str) -> str:
    return f"/asset/{asset_id}"


def resolve_relative(base_file: Path, relative_path: str) -> Path:
    return (base_file.parent / relative_path).resolve()


def parse_viewbox(svg_text: str, label: str) -> tuple[float, float]:
    match = re.search(r'viewBox="[^"]*?([0-9]+(?:\.[0-9]+)?)\s+([0-9]+(?:\.[0-9]+)?)"', svg_text)
    # The regex above intentionally captures the last two numbers of the viewBox.
    if not match:
        raise Example06CheckError(f"{label} must declare a viewBox")
    width = float(match.group(1))
    height = float(match.group(2))
    require(width > 0 and height > 0, f"{label} viewBox width/height must be positive")
    return width, height


def class_law_contains_boolean_surfaces(widget_class_law: str) -> None:
    require("frog.widgets.boolean_control" in widget_class_law, "Boolean class law must define boolean_control")
    require("frog.widgets.boolean_indicator" in widget_class_law, "Boolean class law must define boolean_indicator")
    require("value type: <code>bool</code>" in widget_class_law or "value type: <code>bool" in widget_class_law, "Boolean class law must expose bool value type")


def validate_default_manifest(
    default_manifest: dict[str, Any],
    manifest_path: Path,
    expected_classes: set[str],
) -> dict[str, Path]:
    require(default_manifest.get("format") == "frog.wfrog", "Default Boolean realization manifest must be a .wfrog package")
    require(default_manifest.get("kind") == "widget_realization_library", "Default Boolean realization manifest must be a realization library")

    targets = default_manifest.get("targets")
    require(isinstance(targets, list), "Default Boolean realization manifest must declare targets")
    target_classes = {target.get("class_id") for target in targets if isinstance(target, dict)}
    require(expected_classes.issubset(target_classes), "Default Boolean realization must target both Boolean classes")

    required_state_names = {
        "false",
        "true",
        "hover_false",
        "hover_true",
        "pressed_false",
        "pressed_true",
        "focused_false",
        "focused_true",
        "disabled_false",
        "disabled_true",
        "transition_false_to_true",
        "transition_true_to_false",
    }
    realization_states: set[str] = set()
    for realization in default_manifest.get("realizations", []):
        if isinstance(realization, dict):
            states = realization.get("supported_visual_states", [])
            if isinstance(states, list):
                realization_states.update(str(state) for state in states)
    require(required_state_names.issubset(realization_states), "Default Boolean realization must publish the required visual states")

    state_maps = default_manifest.get("state_maps")
    require(isinstance(state_maps, list), "Default Boolean realization must publish state_maps")
    mapped_states = {entry.get("state") for entry in state_maps if isinstance(entry, dict)}
    require(required_state_names.issubset(mapped_states), "Default Boolean state_maps must cover value, hover, pressed, focus, disabled, and transition states")

    resource_paths: dict[str, Path] = {}
    for resource in default_manifest.get("resources", []):
        if not isinstance(resource, dict):
            continue
        resource_id = resource.get("id")
        resource_path = resource.get("path")
        if isinstance(resource_id, str) and isinstance(resource_path, str):
            absolute = resolve_relative(manifest_path, resource_path)
            require(absolute.is_file(), f"default realization resource does not exist: {absolute.relative_to(ROOT)}")
            resource_paths[resource_id] = absolute

    require(
        "frog.realizations.default.boolean.assets.rectangular_svg" in resource_paths,
        "Default Boolean realization must publish the rectangular SVG resource",
    )
    require(
        "frog.realizations.default.boolean.assets.circular_svg" in resource_paths,
        "Default Boolean realization must publish the circular SVG resource",
    )
    return resource_paths


def validate_wfrog_assets(
    wfrog: dict[str, Any],
    wfrog_path: Path,
    default_manifest_resources: dict[str, Path],
) -> dict[str, Path]:
    require(wfrog.get("format") == "frog.wfrog", "Example 06 package must be .wfrog")
    require(wfrog.get("kind") == "widget_realization_package", "Example 06 package must be a widget_realization_package")

    expected_classes = {"frog.widgets.boolean_control", "frog.widgets.boolean_indicator"}

    realization_refs = wfrog.get("realization_refs")
    require(isinstance(realization_refs, list) and realization_refs, ".wfrog must reference the Default Boolean realization manifest")
    referenced_targets: set[str] = set()
    for ref in realization_refs:
        require(isinstance(ref, dict), "realization_refs entries must be objects")
        package_ref = ref.get("package_ref")
        target_classes = ref.get("target_classes")
        require(package_ref == "Libraries/Realizations/Default/boolean.default.wfrog", "Example 06 must reference the Default Boolean realization manifest")
        require(isinstance(target_classes, list), "realization_refs target_classes must be an array")
        referenced_targets.update(str(item) for item in target_classes)
    require(expected_classes.issubset(referenced_targets), ".wfrog realization_refs must target both Boolean classes")

    svg_assets = wfrog.get("svg_assets")
    require(isinstance(svg_assets, list), ".wfrog must declare svg_assets")
    assets: dict[str, Path] = {}
    targets_by_id: dict[str, str] = {}
    for asset in svg_assets:
        require(isinstance(asset, dict), "svg_assets entries must be objects")
        asset_id = asset.get("asset_id")
        asset_kind = asset.get("kind")
        asset_path = asset.get("path")
        target_class = asset.get("target_class")
        require(isinstance(asset_id, str), "svg asset must expose asset_id")
        require(asset_kind == "svg", f"asset {asset_id} must be kind svg")
        require(isinstance(asset_path, str), f"asset {asset_id} must expose path")
        require(target_class in expected_classes, f"asset {asset_id} has unexpected target_class {target_class!r}")
        resolved = resolve_relative(wfrog_path, asset_path)
        require(resolved.is_file(), f".wfrog asset {asset_id} does not resolve to an existing file: {resolved.relative_to(ROOT)}")
        assets[asset_id] = resolved
        targets_by_id[asset_id] = str(target_class)

    require("boolean_rectangular_svg" in assets, ".wfrog must declare boolean_rectangular_svg")
    require("boolean_circular_svg" in assets, ".wfrog must declare boolean_circular_svg")

    # The .wfrog assets must use the same files published by the Default Boolean realization.
    require(
        assets["boolean_rectangular_svg"] == default_manifest_resources["frog.realizations.default.boolean.assets.rectangular_svg"],
        "boolean_rectangular_svg must resolve to the Default Boolean rectangular SVG template",
    )
    require(
        assets["boolean_circular_svg"] == default_manifest_resources["frog.realizations.default.boolean.assets.circular_svg"],
        "boolean_circular_svg must resolve to the Default Boolean circular SVG template",
    )

    return assets


def validate_svg_template(svg_text: str, *, label: str, expected_class: str, expected_variant: str, widget_width: int, widget_height: int) -> None:
    require(f'data-frog-variant="{expected_variant}"' in svg_text, f"{label} must declare variant {expected_variant}")
    require(expected_class in svg_text, f"{label} must target {expected_class}")
    for public_part in ("root", "label", "caption", "placement_bounds", "state_face", "state_text", "focus_ring"):
        require(f'data-frog-part="{public_part}"' in svg_text, f"{label} missing public part marker {public_part}")
    require("data-frog-bind=" in svg_text, f"{label} must expose binding markers")
    require("data-frog-anchor=" in svg_text, f"{label} must expose anchor markers")

    parse_viewbox(svg_text, label)
    require(widget_width == widget_height, f"{label} default placement must remain square")
    placement = re.search(
        r'id="placement_bounds"[\s\S]*?width="([0-9.]+)"[\s\S]*?height="([0-9.]+)"',
        svg_text,
    )
    require(placement is not None, f"{label} must expose placement_bounds geometry")
    require(float(placement.group(1)) == float(placement.group(2)), f"{label} placement_bounds must remain square")

    # The host overlay owns user-visible caption/state text in Example 06. The template
    # still exposes the public text parts, but the template text must not also render.
    require("#caption_text" in svg_text and "display:none" in svg_text.replace(" ", ""), f"{label} template caption text must be hidden by default")
    require("#state_text" in svg_text and "display:none" in svg_text.replace(" ", ""), f"{label} template state text must be hidden by default")
    compact = svg_text.replace(" ", "")
    require("#placement_bounds" in svg_text and "display:none" in compact, f"{label} placement aura must not render inside the SVG")
    require("#state_face" in svg_text, f"{label} must expose the Boolean state face")


def normalize_source_front_panel(source: dict[str, Any]) -> dict[str, Any]:
    metadata = source.get("metadata", {})
    panel = source.get("front_panel")
    require(isinstance(panel, dict), "source.front_panel must be an object")
    normalized = {
        "panel_id": panel.get("panel_id", f"{metadata.get('name', 'frog')}_panel"),
        "title": panel.get("title", metadata.get("summary", metadata.get("name", "FROG Front Panel"))),
        "class_ref": panel.get("class_ref", "frog.front_panel"),
        "layout": panel.get("canvas", panel.get("layout", {})),
        "widgets": [],
        "host_binding_ref": panel.get("host_binding_ref", "reference_host_default"),
    }
    widgets = panel.get("widgets")
    require(isinstance(widgets, list), "source.front_panel.widgets must be an array")
    for widget in widgets:
        require(isinstance(widget, dict), "source front-panel widget must be an object")
        instance_id = widget.get("instance_ref") or widget.get("instance_id") or widget.get("id")
        require(isinstance(instance_id, str) and instance_id, "source front-panel widget must expose id/instance_ref")
        entry = dict(widget)
        entry["instance_id"] = instance_id
        entry.setdefault("layout", {})
        entry.setdefault("props", {})
        entry.setdefault("visual", {})
        normalized["widgets"].append(entry)
    return normalized


def validate_layout(panel: dict[str, Any]) -> dict[str, dict[str, Any]]:
    layout = panel.get("layout")
    require(layout == {"width": 360, "height": 160, "coordinate_space": "panel_pixels"}, "unexpected Example 06 panel layout")

    widgets_raw = panel.get("widgets")
    require(isinstance(widgets_raw, list) and len(widgets_raw) == 2, "Example 06 panel must contain two widgets")
    widgets = {widget.get("instance_id"): widget for widget in widgets_raw if isinstance(widget, dict)}

    require(set(widgets) == {"bool_input", "bool_result"}, "unexpected Example 06 widget ids")
    require(widgets["bool_input"]["class_ref"] == "frog.widgets.boolean_control", "bool_input class mismatch")
    require(widgets["bool_result"]["class_ref"] == "frog.widgets.boolean_indicator", "bool_result class mismatch")
    require(
        widgets["bool_input"]["layout"]
        == {
            "x": 56,
            "y": 56,
            "width": 72,
            "height": 72,
            "origin": "placement_bounds.top_left",
            "bounds_ref": "placement_bounds",
        },
        "bool_input layout mismatch",
    )
    require(
        widgets["bool_result"]["layout"]
        == {
            "x": 224,
            "y": 56,
            "width": 72,
            "height": 72,
            "origin": "placement_bounds.top_left",
            "bounds_ref": "placement_bounds",
        },
        "bool_result layout mismatch",
    )
    require(widgets["bool_input"]["visual"]["asset_ref"] == "asset:boolean_rectangular_svg", "bool_input asset_ref mismatch")
    require(widgets["bool_result"]["visual"]["asset_ref"] == "asset:boolean_circular_svg", "bool_result asset_ref mismatch")
    input_props = widgets["bool_input"].get("props", {})
    result_props = widgets["bool_result"].get("props", {})
    require(input_props.get("caption.align.horizontal") == "left", "bool_input caption alignment must come from the .wfrog defaults")
    require(result_props.get("caption.align.horizontal") == "left", "bool_result caption alignment must come from the .wfrog defaults")
    require(input_props.get("caption.anchor.x") == 16 and input_props.get("caption.anchor.y") == 24, "bool_input caption anchor must come from the .wfrog defaults")
    require(result_props.get("caption.anchor.x") == 16 and result_props.get("caption.anchor.y") == 24, "bool_result caption anchor must come from the .wfrog defaults")
    require(input_props.get("state_text.visible") is True, "bool_input state text must remain visible through merged .wfrog defaults")
    require(input_props.get("style.focus_ring.visible") is True, "bool_input focus ring visibility must be declared through merged .wfrog defaults")
    require(input_props.get("style.focus_ring.color") == "#00ADEF", "bool_input focus ring color must be declared through merged .wfrog defaults")
    require(input_props.get("style.focus_ring.width") == "1px", "bool_input focus ring width must be declared through merged .wfrog defaults")
    require(result_props.get("state_text.visible") is False, "bool_result state text must be hidden through merged .wfrog defaults")
    require(result_props.get("style.state_face.fill_color.false") == "#ffffff", "bool_result false fill must come from the .wfrog defaults")
    require(result_props.get("style.state_face.fill_color.true") == "#4A8DBC", "bool_result true fill must come from the .wfrog defaults")
    for props, widget_id in ((input_props, "bool_input"), (result_props, "bool_result")):
        require("style.state_face.fill_color.false" in props, f"{widget_id} must declare false fill color through .wfrog")
        require("style.state_face.fill_color.true" in props, f"{widget_id} must declare true fill color through .wfrog")
        require("style.state_face.border_color.false" in props, f"{widget_id} must declare false border color through .wfrog")
        require("style.state_face.border_color.true" in props, f"{widget_id} must declare true border color through .wfrog")
        require(props.get("style.state_face.border_width") == "1px", f"{widget_id} must declare the state-face border width through .wfrog")
        require("state_text.style.text_color.false" in props, f"{widget_id} must declare false state text color through the .frog front-panel instance")
        require("state_text.style.text_color.true" in props, f"{widget_id} must declare true state text color through the .frog front-panel instance")
        require("style.focus_ring.visible" in props, f"{widget_id} must declare focus-ring visibility through .wfrog")
    require("style.transition.duration_ms" in input_props, "bool_input must declare transition timing through .wfrog")
    for state in ("hover_false", "hover_true", "pressed_false", "pressed_true"):
        require(f"style.state_face.fill_color.{state}" in input_props, f"bool_input must declare {state} fill color through .wfrog")
        require(f"style.state_face.border_color.{state}" in input_props, f"bool_input must declare {state} border color through .wfrog")
    return widgets


def bool_text(value: bool, runtime: dict[str, Any]) -> str:
    return str(runtime["state_text.true_text" if value else "state_text.false_text"])


def state_runtime_value(runtime: dict[str, Any], base: str, state: str, fallback: str) -> str:
    return str(runtime.get(f"{base}.{state}", fallback))


def caption_anchor_style(runtime: dict[str, Any]) -> str:
    x = float(runtime.get("caption.anchor.x", 8))
    y = float(runtime.get("caption.anchor.y", 15))
    align = str(runtime.get("caption.align.horizontal", "left"))
    if align == "center":
        transform = "translate(-50%,-50%)"
        text_align = "center"
    elif align in {"right", "end"}:
        transform = "translate(-100%,-50%)"
        text_align = "right"
    else:
        transform = "translateY(-50%)"
        text_align = "left"
    visible = "display:none;" if runtime.get("caption.visible", True) is False else ""
    return (
        f"left:{x:g}px;"
        f"top:{y:g}px;"
        f"transform:{transform};"
        f"text-align:{text_align};"
        f"{visible}"
    )


def render_boolean_widget(widget: dict[str, Any], *, default_manifest_path: str) -> str:
    widget_id = str(widget["widget_id"])
    role = str(widget["role"])
    class_ref = str(widget["class_ref"])
    layout = widget["layout"]
    runtime = widget["runtime"]
    value = bool(runtime["value"])
    value_text = bool_text(value, runtime)
    asset_ref = str(runtime["asset_ref"])
    aid = asset_id_from_ref(asset_ref)
    route = asset_route(aid)
    caption = str(runtime.get("caption.text", widget_id))
    next_value = "false" if value else "true"
    visual_state = "true" if value else "false"
    hover_state = "hover_true" if value else "hover_false"
    pressed_state = "pressed_true" if value else "pressed_false"
    transition_state = "transition_true_to_false" if value else "transition_false_to_true"
    variant = str(runtime.get("realization.variant", "circular" if role == "indicator" else "rectangular"))
    state_fill = state_runtime_value(runtime, "style.state_face.fill_color", visual_state, "#4A8DBC" if value else "#ffffff")
    hover_fill = state_runtime_value(runtime, "style.state_face.fill_color", hover_state, state_fill)
    pressed_fill = state_runtime_value(runtime, "style.state_face.fill_color", pressed_state, state_fill)
    state_border = state_runtime_value(runtime, "style.state_face.border_color", visual_state, "#A8ABAE")
    hover_border = state_runtime_value(runtime, "style.state_face.border_color", hover_state, state_border)
    pressed_border = state_runtime_value(runtime, "style.state_face.border_color", pressed_state, state_border)
    state_border_width = str(runtime.get("style.state_face.border_width", "1px"))
    text_color = state_runtime_value(runtime, "state_text.style.text_color", visual_state, "#0b3d19" if value else "#111827")
    state_font_size = str(runtime.get("state_text.style.font_size", "13px"))
    state_font_weight = str(runtime.get("state_text.style.font_weight", "400"))
    caption_color = str(runtime.get("caption.style.text_color", "#2B3033"))
    caption_font_family = str(runtime.get("caption.style.font_family", "Segoe UI, Arial, sans-serif"))
    caption_font_size = str(runtime.get("caption.style.font_size", "12px"))
    caption_font_weight = str(runtime.get("caption.style.font_weight", "400"))
    transition_ms = str(runtime.get("style.transition.duration_ms", 120))
    transition_timing = str(runtime.get("style.transition.timing", "ease-out"))
    state_text_visible = bool(runtime.get("state_text.visible", True))
    focus_color = str(runtime.get("style.focus_ring.color", "#2563eb"))
    focus_width = str(runtime.get("style.focus_ring.width", "3px")) if runtime.get("style.focus_ring.visible", True) else "0px"

    style = (
        f"left:{layout['x']}px;top:{layout['y']}px;"
        f"width:{layout['width']}px;height:{layout['height']}px;"
        f"--boolean-fill:{state_fill};"
        f"--boolean-hover-fill:{hover_fill};"
        f"--boolean-pressed-fill:{pressed_fill};"
        f"--boolean-border:{state_border};"
        f"--boolean-hover-border:{hover_border};"
        f"--boolean-pressed-border:{pressed_border};"
        f"--boolean-border-width:{state_border_width};"
        f"--boolean-text:{text_color};"
        f"--boolean-state-font-size:{state_font_size};"
        f"--boolean-state-font-weight:{state_font_weight};"
        f"--boolean-caption-color:{caption_color};"
        f"--boolean-caption-font-family:{caption_font_family};"
        f"--boolean-caption-font-size:{caption_font_size};"
        f"--boolean-caption-font-weight:{caption_font_weight};"
        f"--boolean-focus-color:{focus_color};"
        f"--boolean-focus-width:{focus_width};"
        f"--boolean-transition:{transition_ms}ms {transition_timing};"
    )

    attrs = (
        f" data-widget-id='{html.escape(widget_id)}'"
        f" data-class-ref='{html.escape(class_ref)}'"
        f" data-role='{html.escape(role)}'"
        f" data-asset-ref='{html.escape(asset_ref)}'"
        f" data-asset-route='{html.escape(route)}'"
        f" data-current-value='{str(value).lower()}'"
        f" data-realization-variant='{html.escape(variant)}'"
        f" data-frog-visual-law='wfrog-realization-state-map'"
        f" data-frog-visual-state='{visual_state}'"
        f" data-frog-hover-state='{hover_state}'"
        f" data-frog-pressed-state='{pressed_state}'"
        f" data-frog-transition-state='{transition_state}'"
        f" data-frog-state-text-visible='{str(state_text_visible).lower()}'"
        f" data-default-realization-manifest='{html.escape(default_manifest_path)}'"
        f" style='position:absolute;{style}'"
    )

    skin = (
        "<span class='boolean-state-face' data-frog-part='state_face' aria-hidden='true'></span>"
        f"<img class='boolean-skin' src='{html.escape(route)}' "
        f"alt='' aria-hidden='true' />"
    )
    overlays = (
        "<span class='boolean-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor' "
        f"style='{caption_anchor_style(runtime)}'>{html.escape(caption)}</span>"
    )
    if state_text_visible:
        overlays += f"<span class='boolean-state-overlay' data-frog-part='state_text'>{html.escape(value_text)}</span>"

    if role == "control":
        return (
            f"<button class='frog-widget boolean-widget boolean-control' type='submit' "
            f"name='input_value' value='{next_value}' data-toggle-target='{next_value}' "
            f"aria-pressed='{str(value).lower()}'{attrs}>{skin}{overlays}</button>"
        )

    return (
        f"<section class='frog-widget boolean-widget boolean-indicator' aria-readonly='true'{attrs}>"
        f"{skin}{overlays}</section>"
    )


def render_front_panel(snapshot: dict[str, Any], *, default_manifest_path: str) -> str:
    panel = snapshot["ui_runtime"]["panel"]
    widgets = snapshot["ui_runtime"]["widgets"]
    layout = panel["layout"]
    body: list[str] = []
    body.append("<!doctype html><html lang='en'><head><meta charset='utf-8'><title>Boolean Value Roundtrip</title>")
    body.append(
        "<style>"
        "body{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}"
        "h1{font-size:24px;margin:0 0 12px 0}.meta{margin:0 0 20px 0;color:#52606d;}"
        ".front-panel{position:relative;background:#fff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,.08);overflow:hidden;}"
        ".frog-widget{position:absolute;box-sizing:border-box;}"
        ".boolean-widget{border:0;padding:0;background:transparent;font:inherit;color:inherit;overflow:visible;}"
        ".boolean-control{cursor:pointer;}"
        ".boolean-indicator{pointer-events:none;}"
        ".boolean-skin{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;pointer-events:none;z-index:2;}"
        ".boolean-caption-overlay{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-family:var(--boolean-caption-font-family);font-size:var(--boolean-caption-font-size);font-weight:var(--boolean-caption-font-weight);line-height:1;color:var(--boolean-caption-color);white-space:nowrap;pointer-events:none;z-index:3;}"
        ".boolean-state-face{position:absolute;left:4px;top:4px;width:64px;height:64px;box-sizing:border-box;border:var(--boolean-border-width) solid var(--boolean-border);border-radius:0;background:var(--boolean-fill);transition:background var(--boolean-transition),border-color var(--boolean-transition);z-index:1;}"
        ".boolean-widget[data-realization-variant='circular'] .boolean-state-face{border-radius:50%;}"
        ".boolean-control:hover .boolean-state-face{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-border);}"
        ".boolean-control:active .boolean-state-face{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-border);}"
        ".boolean-control:focus-visible .boolean-state-face{outline:var(--boolean-focus-width) solid var(--boolean-focus-color);outline-offset:2px;}"
        ".boolean-state-overlay{position:absolute;left:4px;right:4px;top:36px;transform:translateY(-50%);text-align:center;font-size:var(--boolean-state-font-size);font-weight:var(--boolean-state-font-weight);line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;}"
        ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
        ".state-link{font-size:16px;}"
        "</style>"
    )
    body.append("</head><body><h1>Boolean Value Roundtrip</h1>")
    body.append("<p class='meta'>Example 06 — .frog front panel + Default Boolean .wfrog realization assets</p>")
    body.append("<form method='post' action='/run'>")
    body.append(
        f"<div class='front-panel' data-panel-id='{html.escape(panel['panel_id'])}' "
        f"data-coordinate-space='{html.escape(layout['coordinate_space'])}' "
        f"style='width:{layout['width']}px;height:{layout['height']}px;'>"
    )
    for widget in widgets:
        body.append(render_boolean_widget(widget, default_manifest_path=default_manifest_path))
    body.append("</div><div class='actions'><a class='state-link' href='/state.json'>state.json</a></div></form></body></html>")
    return "".join(body)


def validate_rendered_front_panel(rendered: str, *, expected_value: bool, widgets: dict[str, dict[str, Any]]) -> None:
    require("boolean-card" not in rendered, "hardcoded boolean-card fallback must not pass validation")
    require("boolean-face" not in rendered, "hardcoded boolean-face fallback must not pass validation")
    require("type='checkbox'" not in rendered and 'type="checkbox"' not in rendered, "native checkbox fallback must not pass validation")
    require("Current runtime snapshot" not in rendered, "front panel must not show debug snapshot by default")
    require("<pre>" not in rendered, "front panel must not include raw snapshot preformatted output by default")
    require("/state.json" in rendered, "front panel should keep state.json route available")

    expected_routes = {
        "asset:boolean_rectangular_svg": "/asset/boolean_rectangular_svg",
        "asset:boolean_circular_svg": "/asset/boolean_circular_svg",
    }
    for asset_ref, route in expected_routes.items():
        require(asset_ref in rendered, f"rendered front panel must preserve {asset_ref}")
        require(route in rendered, f"rendered front panel must use asset route {route}")

    require("data-widget-id='bool_input'" in rendered, "rendered control widget missing")
    require("data-widget-id='bool_result'" in rendered, "rendered indicator widget missing")
    require("left:56px;top:56px;width:72px;height:72px" in rendered, "rendered control layout mismatch")
    require("left:224px;top:56px;width:72px;height:72px" in rendered, "rendered indicator layout mismatch")
    require("class='boolean-skin'" in rendered, "rendered widgets must use the Boolean SVG skin assets")
    require("class='boolean-state-face'" in rendered, "rendered widgets must materialize the realization state_face surface")
    require("data-frog-part='state_face'" in rendered, "rendered widgets must expose the modern state_face realization part")
    require("data-frog-visual-law='wfrog-realization-state-map'" in rendered, "rendered widgets must declare the .wfrog realization state-map law")
    require("data-realization-variant='rectangular'" in rendered, "control must preserve the .wfrog rectangular realization variant")
    require("data-realization-variant='circular'" in rendered, "indicator must preserve the .wfrog circular realization variant")
    require("data-frog-hover-state='hover_true'" in rendered or "data-frog-hover-state='hover_false'" in rendered, "control must expose hover visual-state mapping")
    require("data-frog-pressed-state='pressed_true'" in rendered or "data-frog-pressed-state='pressed_false'" in rendered, "control must expose pressed visual-state mapping")
    require("data-frog-transition-state='transition_true_to_false'" in rendered or "data-frog-transition-state='transition_false_to_true'" in rendered, "widgets must expose transition visual-state mapping")
    require("--boolean-fill:" in rendered and "--boolean-border:" in rendered and "--boolean-border-width:" in rendered and "--boolean-transition:" in rendered, "visual state styles must come from .wfrog Boolean properties")
    require(rendered.count("--boolean-border:#A8ABAE;") == 2, "both Boolean widgets must render the published state-face border color")
    require(rendered.count("--boolean-border-width:1px;") == 2, "both Boolean widgets must render the published state-face border width")
    require(rendered.count("data-svg-anchor='caption.anchor'") == 2, "both Boolean captions must expose the realization caption anchor")
    require(rendered.count("left:16px;top:24px;transform:translateY(-50%);text-align:left;") == 2, "both Boolean caption positions must come from .wfrog anchor properties")
    require("data-frog-state-text-visible='false'" in rendered, "bool_result must expose hidden state text from .wfrog")
    expected_fill = "#4A8DBC" if expected_value else "#ffffff"
    require(rendered.count(f"--boolean-fill:{expected_fill};") == 2, "Boolean fill must come from the selected .wfrog true/false state")
    require("transform:translateY(1px)" not in rendered, "pressed movement must not be hardcoded by the runtime")
    require("outline:2px solid #2563eb" not in rendered, "focus styling must be driven by .wfrog focus properties")
    require(rendered.count("--boolean-caption-font-size:12px;") == 2, "caption typography must come from .wfrog")
    require(rendered.count("--boolean-state-font-size:13px;") == 2, "state typography must come from .wfrog")

    state_word = "On" if expected_value else "Off"
    other_word = "Off" if expected_value else "On"
    require(rendered.count(f">{state_word}</span>") == 1, f"rendered front panel must show only the control {state_word} state overlay")
    require(rendered.count(f">{other_word}</span>") == 0, f"rendered front panel must not duplicate inactive {other_word} state text")
    require(rendered.count(f"data-frog-visual-state='{str(expected_value).lower()}'") == 2, "rendered widgets must expose the active true/false realization state")

    if expected_value:
        require("data-toggle-target='false'" in rendered and "name='input_value' value='false'" in rendered, "true control must submit false when clicked")
    else:
        require("data-toggle-target='true'" in rendered and "name='input_value' value='true'" in rendered, "false control must submit true when clicked")

    require("aria-readonly='true'" in rendered, "indicator must remain read-only")
    require("name='input_value'" in rendered, "visible control body must submit the Boolean value")


def check_source_fir_lowering_contract(source: dict[str, Any], fir: dict[str, Any], lowering: dict[str, Any], contract: dict[str, Any]) -> None:
    require(source["metadata"]["name"] == "06_boolean_value_roundtrip", "source metadata.name mismatch")
    require(source["interface"]["inputs"] == [{"id": "input_value", "type": "bool", "binding_origin": "widget.bool_input.value"}], "unexpected source inputs")
    require(source["interface"]["outputs"] == [{"id": "result", "type": "bool"}], "unexpected source outputs")

    fir_unit = single_unit(fir)
    lowered_unit = lowering["lowered_units"][0]
    contract_unit = single_unit(contract)

    require(fir_unit["kind"] == "boolean_value_roundtrip_ui_unit", "unexpected FIR unit kind")
    require(lowered_unit["kind"] == "boolean_value_roundtrip_kernel_with_ui_bindings", "unexpected lowered unit kind")
    require(
        lowering.get("lowering_intent", {}).get("compiler_family_targets") == ["llvm_native_kernel_bridge"],
        "Example 06 lowering must declare the LLVM native kernel bridge target",
    )
    require(contract_unit["kind"] == "boolean_value_roundtrip_ui_unit", "unexpected contract unit kind")
    require(contract_unit["execution_kernel"] == {"operation": "copy", "src": "input_value", "dst": "result", "type": "bool"}, "unexpected execution kernel")


def check_native_kernel_publication(refs: dict[str, Any]) -> None:
    manifest = load_json(repo_path(refs["native_kernel_manifest_path"]))
    kernel_text = read_text(repo_path(refs["llvm_kernel_path"]))
    module_text = read_text(repo_path(refs["llvm_module_path"]))

    require(manifest.get("artifact_kind") == "frog_native_kernel_manifest", "unexpected native manifest artifact_kind")
    kernel = manifest.get("kernel")
    require(isinstance(kernel, dict), "native manifest kernel must be an object")
    require(kernel.get("id") == "example06_boolean_roundtrip_kernel", "unexpected Example 06 kernel id")
    require(kernel.get("source_lowered_unit") == "Examples/06_boolean_value_roundtrip/main.lowering.json", "unexpected Example 06 native source lowering")

    backend = kernel.get("backend")
    require(isinstance(backend, dict), "native manifest backend must be an object")
    require(backend.get("family") == "llvm", "Example 06 native backend family must be llvm")
    require(backend.get("source_artifact") == refs["llvm_kernel_path"], "Example 06 native source artifact mismatch")

    entry = kernel.get("entry")
    require(isinstance(entry, dict), "native manifest entry must be an object")
    require(entry.get("symbol") == "frog_example06_run", "unexpected Example 06 native entry symbol")
    require(entry.get("abi") == "frog_bool_to_result_status_outptr", "unexpected Example 06 native ABI")

    io = kernel.get("io")
    require(isinstance(io, dict), "native manifest io must be an object")
    require(io.get("inputs") == [{"id": "input_value", "type": "bool"}], "unexpected Example 06 native input surface")
    require(io.get("outputs") == [{"id": "result", "type": "bool"}], "unexpected Example 06 native output surface")

    layout = kernel.get("result_status_layout")
    require(isinstance(layout, dict), "native manifest result_status_layout must be an object")
    require(layout.get("carrier") == "out_parameter", "Example 06 native ABI must use an out parameter")
    require(layout.get("c_signature") == "void frog_example06_run(uint8_t input_value, FrogBoolRunResult* out_result)", "unexpected Example 06 native C signature")

    error_model = kernel.get("error_model")
    require(isinstance(error_model, dict), "native manifest error_model must be an object")
    require(error_model.get("overflow") == "not_applicable", "Example 06 bool kernel must not claim u16 overflow behavior")

    require("%FrogBoolRunResult = type { i8, i8, i16 }" in kernel_text, "kernel.ll must declare FrogBoolRunResult layout")
    require("define void @frog_example06_run(i8 %input_value, ptr %out_result)" in kernel_text, "kernel.ll must export frog_example06_run")
    require("store i8 %result" in kernel_text, "kernel.ll must write the bool result through the out parameter")
    require("store i16 0" in kernel_text, "kernel.ll must publish error_code 0")
    require("define i1 @frog_example06_copy_bool" in module_text, "module.ll must expose the lowered bool copy proof")
    require("public_output=%s" in module_text and "status=ok" in module_text, "module.ll must publish the bool proof output surface")


def snapshot_path_for_case(case: dict[str, Any], acceptance_refs: dict[str, Any]) -> Path:
    case_snapshot = case.get("snapshot_path")
    if isinstance(case_snapshot, str):
        return repo_path(case_snapshot)
    # Backward compatibility with the initial Example 06 fixture.
    return repo_path(str(acceptance_refs["snapshot_path"]))


def check_acceptance(acceptance_path: Path, *, print_json: bool = False) -> None:
    acceptance = load_json(acceptance_path)
    require(acceptance.get("artifact_kind") == "frog_reference_runtime_acceptance", "unexpected acceptance artifact_kind")
    require(acceptance.get("example_id") == "06_boolean_value_roundtrip", "unexpected example_id")
    require(acceptance.get("runtime_family") == "reference_host_runtime_ui_binding", "unexpected runtime_family")

    refs = acceptance["artifact_refs"]
    source = load_json(repo_path(refs["source_path"]))
    fir = load_json(repo_path(refs["fir_path"]))
    lowering = load_json(repo_path(refs["lowering_path"]))
    contract = load_json(repo_path(refs["contract_path"]))
    wfrog_path = repo_path(refs["wfrog_path"])
    wfrog = load_json(wfrog_path)
    widget_class_law = read_text(repo_path(refs["widget_class_law_path"]))
    default_manifest_path = repo_path(refs["default_realization_manifest_path"])
    default_manifest = load_json(default_manifest_path)

    check_source_fir_lowering_contract(source, fir, lowering, contract)
    check_native_kernel_publication(refs)
    class_law_contains_boolean_surfaces(widget_class_law)

    expected_classes = {"frog.widgets.boolean_control", "frog.widgets.boolean_indicator"}
    default_resources = validate_default_manifest(default_manifest, default_manifest_path, expected_classes)
    assets = validate_wfrog_assets(wfrog, wfrog_path, default_resources)

    source = load_json(repo_path(refs["source_path"]))
    source_panel = normalize_source_front_panel(source)
    panel = merge_default_widget_properties(source_panel, wfrog)
    widgets = validate_layout(panel)

    rectangular_svg = read_text(assets["boolean_rectangular_svg"])
    circular_svg = read_text(assets["boolean_circular_svg"])
    validate_svg_template(
        rectangular_svg,
        label="boolean_rectangular.svg",
        expected_class="frog.widgets.boolean_control",
        expected_variant="rectangular",
        widget_width=widgets["bool_input"]["layout"]["width"],
        widget_height=widgets["bool_input"]["layout"]["height"],
    )
    validate_svg_template(
        circular_svg,
        label="boolean_circular.svg",
        expected_class="frog.widgets.boolean_indicator",
        expected_variant="circular",
        widget_width=widgets["bool_result"]["layout"]["width"],
        widget_height=widgets["bool_result"]["layout"]["height"],
    )

    cases = acceptance.get("cases")
    if not isinstance(cases, list):
        # Backward compatibility with the initial Example 06 fixture.
        cases = [
            {
                "id": "input_true",
                "input_value": acceptance["headless"]["input_value"],
                "expected_result": acceptance["headless"]["expected_result"],
                "snapshot_path": acceptance["artifact_refs"]["snapshot_path"],
            },
            {"id": "input_false", "input_value": False, "expected_result": False},
        ]

    observed_by_case: dict[str, dict[str, Any]] = {}
    for case in cases:
        require(isinstance(case, dict), "acceptance cases must be objects")
        case_id = str(case.get("id"))
        input_value = case.get("input_value")
        expected_result = case.get("expected_result")
        require(isinstance(input_value, bool), f"{case_id}.input_value must be bool")
        require(isinstance(expected_result, bool), f"{case_id}.expected_result must be bool")

        observed = execute_boolean_roundtrip(contract, panel, input_value)
        require(observed["outputs"]["public"]["result"] is expected_result, f"{case_id} public result mismatch")
        require(observed["outputs"]["ui"]["bool_result"] is expected_result, f"{case_id} indicator result mismatch")
        observed_by_case[case_id] = observed

        if "snapshot_path" in case:
            expected_snapshot = load_json(snapshot_path_for_case(case, refs))
            if canonical_json_bytes(observed) != canonical_json_bytes(expected_snapshot):
                raise Example06CheckError(f"Example 06 runtime snapshot mismatch for {case_id}")

        rendered = render_front_panel(
            observed,
            default_manifest_path=refs["default_realization_manifest_path"],
        )
        validate_rendered_front_panel(rendered, expected_value=input_value, widgets=widgets)

    require("input_true" in observed_by_case, "acceptance must validate true state")
    require("input_false" in observed_by_case, "acceptance must validate false state")

    # Explicit toggle posture checks: clicking the visible control submits the opposite value.
    rendered_true = render_front_panel(observed_by_case["input_true"], default_manifest_path=refs["default_realization_manifest_path"])
    rendered_false = render_front_panel(observed_by_case["input_false"], default_manifest_path=refs["default_realization_manifest_path"])
    require("name='input_value' value='false'" in rendered_true, "toggling from true must post false")
    require("name='input_value' value='true'" in rendered_false, "toggling from false must post true")

    if print_json:
        json.dump(observed_by_case["input_true"], sys.stdout, indent=2)
        sys.stdout.write("\n")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Check Example 06 Boolean value roundtrip.")
    parser.add_argument("--acceptance", type=Path, default=DEFAULT_ACCEPTANCE)
    parser.add_argument("--print", action="store_true", dest="print_json")
    args = parser.parse_args(argv)
    acceptance_path = args.acceptance if args.acceptance.is_absolute() else ROOT / args.acceptance

    try:
        check_acceptance(acceptance_path, print_json=args.print_json)
    except Example06CheckError as exc:
        print("Example 06 Boolean value roundtrip check: FAILED", file=sys.stderr)
        print(str(exc), file=sys.stderr)
        return 1

    print("Example 06 Boolean value roundtrip check: ok")
    print(f"acceptance: {acceptance_path.relative_to(ROOT)}")
    print("rendering: .frog front panel + Default Boolean .wfrog realization assets")
    print("states: input_false and input_true checked")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
