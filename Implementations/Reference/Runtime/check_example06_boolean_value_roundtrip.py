#!/usr/bin/env python3
"""Check Example 06 Boolean value roundtrip.

This check is intentionally narrow. It validates the Example 06 fixture as a
runtime/UI-binding pilot without generalizing Deriver, Lowerer, ContractEmitter,
or LLVM coverage.

The visible front panel must be derived from the published .wfrog package and
the Default Boolean realization assets. A handcrafted CSS-only fallback is not
accepted as official validation.
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
DEFAULT_ACCEPTANCE = ROOT / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example06_boolean_value_roundtrip.acceptance.json"


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


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def repo_path(path_text: str) -> Path:
    path = Path(path_text)
    return path if path.is_absolute() else ROOT / path


def repo_relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise Example06CheckError(message)


def single_unit(artifact: dict[str, Any], key: str = "units") -> dict[str, Any]:
    units = artifact.get(key)
    require(isinstance(units, list) and len(units) == 1 and isinstance(units[0], dict), f"expected exactly one {key} entry")
    return units[0]


def svg_route(asset_id: str) -> str:
    return f"/asset/{asset_id}"


def resolve_wfrog_relative(wfrog_path: Path, text_path: str) -> Path:
    candidate = Path(text_path)
    return candidate if candidate.is_absolute() else (wfrog_path.parent / candidate).resolve()


def resolve_manifest_relative(manifest_path: Path, text_path: str) -> Path:
    candidate = Path(text_path)
    return candidate if candidate.is_absolute() else (manifest_path.parent / candidate).resolve()


def asset_id_from_ref(asset_ref: str) -> str:
    require(asset_ref.startswith("asset:"), f"asset_ref must use asset:<id> syntax, got {asset_ref!r}")
    asset_id = asset_ref.removeprefix("asset:")
    require(asset_id, "asset_ref must contain a non-empty asset id")
    return asset_id


def build_wfrog_asset_map(wfrog: dict[str, Any], wfrog_path: Path) -> dict[str, dict[str, Any]]:
    assets: dict[str, dict[str, Any]] = {}
    for index, asset in enumerate(wfrog.get("svg_assets", [])):
        require(isinstance(asset, dict), f"svg_assets[{index}] must be an object")
        asset_id = asset.get("asset_id")
        require(isinstance(asset_id, str), f"svg_assets[{index}].asset_id must be a string")
        require(asset.get("kind") == "svg", f"svg_assets[{asset_id}].kind must be svg")
        path_text = asset.get("path")
        target_class = asset.get("target_class")
        require(isinstance(path_text, str), f"svg_assets[{asset_id}].path must be a string")
        require(isinstance(target_class, str), f"svg_assets[{asset_id}].target_class must be a string")
        resolved = resolve_wfrog_relative(wfrog_path, path_text)
        require(resolved.is_file(), f"asset file does not exist for {asset_id}: {repo_relative(resolved)}")
        svg_text = resolved.read_text(encoding="utf-8")
        require("<svg" in svg_text and "</svg>" in svg_text, f"asset {asset_id} must be an SVG document")
        assets[asset_id] = {
            "asset_id": asset_id,
            "path": path_text,
            "resolved": resolved,
            "route": svg_route(asset_id),
            "target_class": target_class,
            "svg_text": svg_text,
        }
    return assets


def validate_default_realization_manifest(
    manifest: dict[str, Any],
    manifest_path: Path,
    required_classes: set[str],
    required_asset_paths: set[Path],
) -> None:
    require(manifest.get("format") == "frog.wfrog", "Default Boolean realization manifest must use format frog.wfrog")
    require(manifest.get("kind") == "widget_realization_library", "Default Boolean realization manifest must be a widget_realization_library")

    targets = manifest.get("targets")
    require(isinstance(targets, list), "Default Boolean realization manifest must expose targets")
    target_classes = {target.get("class_id") for target in targets if isinstance(target, dict)}
    require(required_classes.issubset(target_classes), "Default Boolean realization manifest must target all Example 06 widget classes")

    resources = manifest.get("resources")
    require(isinstance(resources, list), "Default Boolean realization manifest must expose resources")
    resolved_resources: set[Path] = set()
    for index, resource in enumerate(resources):
        require(isinstance(resource, dict), f"default realization resources[{index}] must be an object")
        path_text = resource.get("path")
        require(isinstance(path_text, str), f"default realization resources[{index}].path must be a string")
        resolved = resolve_manifest_relative(manifest_path, path_text)
        require(resolved.is_file(), f"default realization resource missing: {repo_relative(resolved)}")
        resolved_resources.add(resolved)

    missing = required_asset_paths - resolved_resources
    require(not missing, "Default Boolean realization manifest does not publish all .wfrog-referenced asset files: " + ", ".join(sorted(repo_relative(path) for path in missing)))


def render_front_panel(
    wfrog: dict[str, Any],
    wfrog_path: Path,
    observed: dict[str, Any],
    default_manifest_path: Path,
) -> str:
    panel = wfrog["front_panels"][0]
    layout = panel["layout"]
    runtime_widgets = {widget["widget_id"]: widget for widget in observed["ui_runtime"]["widgets"]}
    asset_map = build_wfrog_asset_map(wfrog, wfrog_path)

    body: list[str] = []
    body.append("<!doctype html><html lang='en'><head><meta charset='utf-8'>")
    body.append("<title>Boolean Value Roundtrip</title>")
    body.append(
        "<style>"
        "body{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}"
        "h1{font-size:24px;margin:0 0 12px 0;}"
        "p.meta{margin:0 0 18px 0;color:#52606d;}"
        ".front-panel{position:relative;background:#fff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,.08);overflow:hidden;}"
        ".frog-widget{position:absolute;box-sizing:border-box;}"
        ".frog-boolean-widget{font-family:Segoe UI,Arial,sans-serif;}"
        ".boolean-skin{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;}"
        ".boolean-caption-overlay{position:absolute;left:10px;top:6px;right:10px;font-size:12px;font-weight:600;color:#1f2933;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;pointer-events:none;}"
        ".boolean-state-overlay{position:absolute;left:0;right:0;top:50%;transform:translateY(-50%);text-align:center;font-size:13px;font-weight:700;color:#111827;pointer-events:none;}"
        ".boolean-control-hit{position:absolute;inset:0;border:0;background:transparent;color:transparent;cursor:pointer;}"
        ".boolean-control-hit:focus{outline:2px solid #0f62fe;outline-offset:-3px;}"
        ".actions{margin-top:16px;display:flex;gap:12px;align-items:center;}"
        "button{padding:8px 14px;border:0;border-radius:6px;background:#0f62fe;color:#fff;font-weight:600;cursor:pointer;}"
        "</style>"
    )
    body.append("</head><body>")
    body.append("<h1>Boolean Value Roundtrip</h1>")
    body.append("<p class='meta'>Example 06 — .wfrog front panel + Default Boolean realization assets</p>")
    body.append("<form method='post' action='/run'>")
    body.append(
        "<div class='front-panel' "
        f"data-panel-id='{html.escape(panel['panel_id'])}' "
        f"data-coordinate-space='{html.escape(layout['coordinate_space'])}' "
        "data-render-source='wfrog_and_default_realization_assets' "
        f"style='width:{layout['width']}px;height:{layout['height']}px;'>"
    )

    for widget in panel["widgets"]:
        widget_id = widget["instance_id"]
        runtime = runtime_widgets[widget_id]["runtime"]
        w_layout = widget["layout"]
        visual = widget.get("visual", {})
        asset_ref = visual.get("asset_ref")
        require(isinstance(asset_ref, str), f"widget {widget_id} must declare visual.asset_ref")
        asset_id = asset_id_from_ref(asset_ref)
        require(asset_id in asset_map, f"widget {widget_id} references unknown asset {asset_id}")
        asset = asset_map[asset_id]
        require(asset["target_class"] == widget["class_ref"], f"asset {asset_id} target_class must match widget {widget_id} class_ref")

        value = bool(runtime["value"])
        state_text = str(widget["props"].get("state_text.true_text" if value else "state_text.false_text", "TRUE" if value else "FALSE"))
        caption = str(widget["props"].get("caption.text", widget_id))
        enabled = bool(widget["props"].get("interaction.enabled", True))

        body.append(
            "<section class='frog-widget frog-boolean-widget' "
            f"data-widget-id='{html.escape(widget_id)}' "
            f"data-class-ref='{html.escape(widget['class_ref'])}' "
            f"data-role='{html.escape(widget['role'])}' "
            f"data-asset-ref='{html.escape(asset_ref)}' "
            f"data-asset-route='{html.escape(asset['route'])}' "
            f"data-default-realization-manifest='{html.escape(repo_relative(default_manifest_path))}' "
            f"style='left:{w_layout['x']}px;top:{w_layout['y']}px;width:{w_layout['width']}px;height:{w_layout['height']}px;'>"
        )
        body.append(
            "<img class='boolean-skin' "
            f"src='{html.escape(asset['route'])}' "
            f"data-asset-id='{html.escape(asset_id)}' "
            f"data-realization-asset-path='{html.escape(repo_relative(asset['resolved']))}' "
            "alt='' aria-hidden='true' />"
        )
        body.append(f"<span class='boolean-caption-overlay' data-frog-member='caption.text'>{html.escape(caption)}</span>")
        body.append(f"<span class='boolean-state-overlay' data-frog-part='state_text' data-frog-member='state_text.true_text state_text.false_text'>{html.escape(state_text)}</span>")
        if widget["role"] == "control":
            # Hidden state carrier for the form submit. The visible widget body remains the .wfrog-declared SVG asset.
            body.append(f"<input type='hidden' name='input_value' value='{'true' if value else 'false'}' />")
            body.append(
                "<button class='boolean-control-hit' type='submit' "
                f"aria-label='{html.escape(caption)}' "
                f"data-bound-public-input='{html.escape(widget['binding']['public_input_id'])}'"
                f"{'' if enabled else ' disabled'}>Run</button>"
            )
        body.append("</section>")

    body.append("</div><div class='actions'><button type='submit'>Run Example 06</button><a href='/state.json'>state.json</a></div>")
    body.append("</form></body></html>")
    return "".join(body)


def execute_boolean_roundtrip(contract: dict[str, Any], wfrog: dict[str, Any], input_value: bool) -> dict[str, Any]:
    unit = single_unit(contract)
    panel = wfrog["front_panels"][0]
    widgets_by_id = {widget["instance_id"]: widget for widget in panel["widgets"]}

    result = bool(input_value)
    return {
        "artifact_kind": "frog_runtime_execution_result",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "status": "ok",
        "contract_ref": {
            "unit_ids": [unit["unit_id"]],
            "backend_family": contract["backend_family"],
            "source_ref": contract["source_ref"],
        },
        "execution_summary": {
            "mode": "boolean_value_roundtrip",
            "executed_unit": unit["unit_id"],
            "operation": "copy",
            "input_value": result,
            "result": result,
        },
        "outputs": {"public": {"result": result}, "ui": {"bool_input": result, "bool_result": result}},
        "ui_runtime": {
            "panel": {
                "panel_id": panel["panel_id"],
                "title": panel["title"],
                "class_ref": panel["class_ref"],
                "layout": panel["layout"],
            },
            "widgets": [
                {
                    "widget_id": "bool_input",
                    "class_ref": widgets_by_id["bool_input"]["class_ref"],
                    "role": "control",
                    "layout": widgets_by_id["bool_input"]["layout"],
                    "runtime": {
                        "value": result,
                        "label.text": widgets_by_id["bool_input"]["props"]["label.text"],
                        "caption.text": widgets_by_id["bool_input"]["props"]["caption.text"],
                        "asset_ref": widgets_by_id["bool_input"]["visual"]["asset_ref"],
                        "realization.variant": widgets_by_id["bool_input"]["props"]["realization.variant"],
                    },
                },
                {
                    "widget_id": "bool_result",
                    "class_ref": widgets_by_id["bool_result"]["class_ref"],
                    "role": "indicator",
                    "layout": widgets_by_id["bool_result"]["layout"],
                    "runtime": {
                        "value": result,
                        "label.text": widgets_by_id["bool_result"]["props"]["label.text"],
                        "caption.text": widgets_by_id["bool_result"]["props"]["caption.text"],
                        "asset_ref": widgets_by_id["bool_result"]["visual"]["asset_ref"],
                        "realization.variant": widgets_by_id["bool_result"]["props"]["realization.variant"],
                    },
                },
            ],
        },
        "diagnostics": [],
    }


def assert_no_hardcoded_fallback(rendered: str) -> None:
    forbidden = [
        ".boolean-card",
        ".boolean-face",
        "type='checkbox'",
        'type="checkbox"',
        "Current runtime snapshot",
        "<pre>",
    ]
    for marker in forbidden:
        require(marker not in rendered, f"official Example 06 rendering must not contain fallback/debug marker: {marker}")


def check_rendered_asset_fidelity(wfrog: dict[str, Any], wfrog_path: Path, rendered: str) -> None:
    asset_map = build_wfrog_asset_map(wfrog, wfrog_path)
    required_routes: set[str] = set()

    for widget in wfrog["front_panels"][0]["widgets"]:
        visual = widget.get("visual", {})
        asset_ref = visual.get("asset_ref")
        require(isinstance(asset_ref, str), f"widget {widget['instance_id']} must declare visual.asset_ref")
        asset_id = asset_id_from_ref(asset_ref)
        require(asset_id in asset_map, f"widget {widget['instance_id']} references unknown asset {asset_id}")
        route = asset_map[asset_id]["route"]
        required_routes.add(route)
        require(f"data-asset-ref='{html.escape(asset_ref)}'" in rendered, f"rendered widget {widget['instance_id']} must preserve asset_ref {asset_ref}")
        require(f"src='{html.escape(route)}'" in rendered, f"rendered widget {widget['instance_id']} must visibly use route {route}")
        require(f"data-asset-route='{html.escape(route)}'" in rendered, f"rendered widget {widget['instance_id']} must expose data-asset-route {route}")

    for route in required_routes:
        require(route in rendered, f"rendered HTML is missing required asset route {route}")


def check_acceptance(acceptance_path: Path, *, print_json: bool = False, print_html: bool = False) -> None:
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
    expected_snapshot = load_json(repo_path(refs["snapshot_path"]))
    widget_class_law_path = repo_path(refs["widget_class_law_path"])
    default_manifest_path = repo_path(refs["default_realization_manifest_path"])
    widget_class_law = widget_class_law_path.read_text(encoding="utf-8")
    default_manifest = load_json(default_manifest_path)

    require(source["metadata"]["name"] == "06_boolean_value_roundtrip", "source metadata.name mismatch")
    require(source["interface"]["inputs"] == [{"id": "input_value", "type": "bool", "binding_origin": "widget.bool_input.value"}], "unexpected source inputs")
    require(source["interface"]["outputs"] == [{"id": "result", "type": "bool"}], "unexpected source outputs")

    fir_unit = single_unit(fir)
    lowered_unit = lowering["lowered_units"][0]
    contract_unit = single_unit(contract)
    require(fir_unit["kind"] == "boolean_value_roundtrip_ui_unit", "unexpected FIR unit kind")
    require(lowered_unit["kind"] == "boolean_value_roundtrip_kernel_with_ui_bindings", "unexpected lowered unit kind")
    require(contract_unit["kind"] == "boolean_value_roundtrip_ui_unit", "unexpected contract unit kind")
    require(contract_unit["execution_kernel"] == {"operation": "copy", "src": "input_value", "dst": "result", "type": "bool"}, "unexpected execution kernel")

    require("frog.widgets.boolean_control" in widget_class_law, "Boolean class law must define boolean_control")
    require("frog.widgets.boolean_indicator" in widget_class_law, "Boolean class law must define boolean_indicator")

    require(wfrog["format"] == "frog.wfrog", "unexpected .wfrog format")
    panel = wfrog["front_panels"][0]
    require(panel["layout"] == {"width": 420, "height": 150, "coordinate_space": "panel_pixels"}, "unexpected panel layout")
    widgets = {widget["instance_id"]: widget for widget in panel["widgets"]}
    require(set(widgets) == {"bool_input", "bool_result"}, "unexpected widget ids")
    require(widgets["bool_input"]["class_ref"] == "frog.widgets.boolean_control", "bool_input class mismatch")
    require(widgets["bool_result"]["class_ref"] == "frog.widgets.boolean_indicator", "bool_result class mismatch")
    require(widgets["bool_input"]["layout"] == {"x": 20, "y": 32, "width": 160, "height": 80}, "bool_input layout mismatch")
    require(widgets["bool_result"]["layout"] == {"x": 240, "y": 32, "width": 160, "height": 80}, "bool_result layout mismatch")

    realization_refs = wfrog.get("realization_refs")
    require(isinstance(realization_refs, list) and realization_refs, ".wfrog must publish realization_refs")
    required_classes = {widgets["bool_input"]["class_ref"], widgets["bool_result"]["class_ref"]}
    require(any(ref.get("package_ref") == refs["default_realization_manifest_path"] for ref in realization_refs if isinstance(ref, dict)), ".wfrog must reference the Default Boolean realization manifest")
    for ref in realization_refs:
        require(isinstance(ref, dict), "realization_refs entries must be objects")
        target_classes = set(ref.get("target_classes", []))
        require(required_classes.issubset(target_classes), "realization_refs target_classes must include both Example 06 Boolean classes")

    asset_map = build_wfrog_asset_map(wfrog, wfrog_path)
    validate_default_realization_manifest(
        default_manifest,
        default_manifest_path,
        required_classes,
        {asset["resolved"] for asset in asset_map.values()},
    )

    input_value = acceptance["headless"]["input_value"]
    require(isinstance(input_value, bool), "acceptance input_value must be bool")
    observed = execute_boolean_roundtrip(contract, wfrog, input_value)
    if canonical_json_bytes(observed) != canonical_json_bytes(expected_snapshot):
        raise Example06CheckError("Example 06 runtime snapshot mismatch")

    rendered = render_front_panel(wfrog, wfrog_path, observed, default_manifest_path)
    assert_no_hardcoded_fallback(rendered)
    check_rendered_asset_fidelity(wfrog, wfrog_path, rendered)

    require("data-render-source='wfrog_and_default_realization_assets'" in rendered, "rendered panel must declare its .wfrog/default-realization render source")
    require("data-widget-id='bool_input'" in rendered, "rendered control widget missing")
    require("data-widget-id='bool_result'" in rendered, "rendered indicator widget missing")
    require("left:20px;top:32px;width:160px;height:80px" in rendered, "rendered control layout mismatch")
    require("left:240px;top:32px;width:160px;height:80px" in rendered, "rendered indicator layout mismatch")
    require("/state.json" in rendered, "front panel should keep state.json route available")
    require(acceptance["ui"].get("debug_snapshot_visible_by_default") is False, "acceptance must declare debug snapshot hidden by default")

    for route in acceptance["ui"]["expected_routes"]:
        if route.startswith("/asset/"):
            require(route in rendered, f"expected asset route missing from rendered panel: {route}")

    if print_json:
        json.dump(observed, sys.stdout, indent=2)
        sys.stdout.write("\n")
    if print_html:
        sys.stdout.write(rendered)
        sys.stdout.write("\n")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Check Example 06 Boolean value roundtrip.")
    parser.add_argument("--acceptance", type=Path, default=DEFAULT_ACCEPTANCE)
    parser.add_argument("--print", action="store_true", dest="print_json")
    parser.add_argument("--print-html", action="store_true", dest="print_html")
    args = parser.parse_args(argv)
    acceptance_path = args.acceptance if args.acceptance.is_absolute() else ROOT / args.acceptance
    try:
        check_acceptance(acceptance_path, print_json=args.print_json, print_html=args.print_html)
    except Example06CheckError as exc:
        print("Example 06 Boolean value roundtrip check: FAILED", file=sys.stderr)
        print(str(exc), file=sys.stderr)
        return 1
    print("Example 06 Boolean value roundtrip check: ok")
    print(f"acceptance: {acceptance_path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
