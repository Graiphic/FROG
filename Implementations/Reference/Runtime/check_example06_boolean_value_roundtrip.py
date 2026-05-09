#!/usr/bin/env python3
"""Check Example 06 Boolean value roundtrip.

This check is intentionally narrow. It validates the Example 06 fixture as a
runtime/UI-binding pilot without generalizing Deriver, Lowerer, ContractEmitter,
or LLVM coverage.
"""

from __future__ import annotations

import argparse
import html
import json
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


def require(condition: bool, message: str) -> None:
    if not condition:
        raise Example06CheckError(message)


def single_unit(artifact: dict[str, Any], key: str = "units") -> dict[str, Any]:
    units = artifact.get(key)
    require(isinstance(units, list) and len(units) == 1 and isinstance(units[0], dict), f"expected exactly one {key} entry")
    return units[0]


def render_front_panel(snapshot: dict[str, Any]) -> str:
    panel = snapshot["ui_runtime"]["panel"]
    widgets = snapshot["ui_runtime"]["widgets"]
    body: list[str] = []
    body.append("<!doctype html><html lang='en'><head><meta charset='utf-8'><title>Boolean Value Roundtrip</title>")
    body.append("<style>body{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}h1{font-size:24px;margin:0 0 12px 0}.front-panel{position:relative;background:#fff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,.08);overflow:hidden}.frog-widget{position:absolute;box-sizing:border-box}.boolean-card{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:8px;border:1px solid #cbd5e1;border-radius:10px;background:#f8fafc}.boolean-face{width:48px;height:32px;border-radius:999px;border:2px solid #64748b;background:#e2e8f0;position:relative}.boolean-face[data-value='true']{background:#bfdbfe;border-color:#2563eb}.boolean-face::after{content:'';position:absolute;width:20px;height:20px;top:4px;left:5px;border-radius:999px;background:#fff;box-shadow:0 1px 3px rgba(15,23,42,.2)}.boolean-face[data-value='true']::after{left:21px}.boolean-caption{font-size:13px;font-weight:600}.boolean-state{font-size:12px;color:#475569}.actions{margin-top:16px;display:flex;gap:12px;align-items:center}button{padding:8px 14px;border:0;border-radius:6px;background:#0f62fe;color:#fff;font-weight:600}</style>")
    body.append("</head><body><h1>Boolean Value Roundtrip</h1>")
    body.append("<form method='post' action='/run'>")
    layout = panel["layout"]
    body.append(f"<div class='front-panel' data-panel-id='{html.escape(panel['panel_id'])}' data-coordinate-space='{html.escape(layout['coordinate_space'])}' style='width:{layout['width']}px;height:{layout['height']}px;'>")
    for widget in widgets:
        w_layout = widget["layout"]
        runtime = widget["runtime"]
        value = "true" if runtime["value"] else "false"
        body.append(f"<section class='frog-widget boolean-card' data-widget-id='{html.escape(widget['widget_id'])}' data-class-ref='{html.escape(widget['class_ref'])}' data-role='{html.escape(widget['role'])}' style='left:{w_layout['x']}px;top:{w_layout['y']}px;width:{w_layout['width']}px;height:{w_layout['height']}px;'>")
        body.append(f"<span class='boolean-caption'>{html.escape(str(runtime.get('caption.text', widget['widget_id'])))}</span>")
        if widget["role"] == "control":
            checked = " checked" if runtime["value"] else ""
            body.append(f"<label><input name='input_value' type='checkbox' value='true'{checked}> <span class='boolean-face' data-value='{value}'></span></label>")
        else:
            body.append(f"<output class='boolean-face' data-value='{value}' aria-label='{value}'></output>")
        body.append(f"<span class='boolean-state'>{value.upper()}</span></section>")
    body.append("</div><div class='actions'><button type='submit'>Run Example 06</button><a href='/state.json'>state.json</a></div></form></body></html>")
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
    wfrog = load_json(repo_path(refs["wfrog_path"]))
    expected_snapshot = load_json(repo_path(refs["snapshot_path"]))
    widget_class_law = repo_path(refs["widget_class_law_path"]).read_text(encoding="utf-8")
    default_manifest = load_json(repo_path(refs["default_realization_manifest_path"]))

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
    default_targets = {target["class_id"] for target in default_manifest["targets"]}
    require({"frog.widgets.boolean_control", "frog.widgets.boolean_indicator"}.issubset(default_targets), "Default Boolean realization must target both Boolean classes")

    require(wfrog["format"] == "frog.wfrog", "unexpected .wfrog format")
    panel = wfrog["front_panels"][0]
    require(panel["layout"] == {"width": 420, "height": 150, "coordinate_space": "panel_pixels"}, "unexpected panel layout")
    widgets = {widget["instance_id"]: widget for widget in panel["widgets"]}
    require(set(widgets) == {"bool_input", "bool_result"}, "unexpected widget ids")
    require(widgets["bool_input"]["class_ref"] == "frog.widgets.boolean_control", "bool_input class mismatch")
    require(widgets["bool_result"]["class_ref"] == "frog.widgets.boolean_indicator", "bool_result class mismatch")
    require(widgets["bool_input"]["layout"] == {"x": 20, "y": 32, "width": 160, "height": 80}, "bool_input layout mismatch")
    require(widgets["bool_result"]["layout"] == {"x": 240, "y": 32, "width": 160, "height": 80}, "bool_result layout mismatch")

    input_value = acceptance["headless"]["input_value"]
    require(isinstance(input_value, bool), "acceptance input_value must be bool")
    observed = execute_boolean_roundtrip(contract, wfrog, input_value)
    if canonical_json_bytes(observed) != canonical_json_bytes(expected_snapshot):
        raise Example06CheckError("Example 06 runtime snapshot mismatch")

    rendered = render_front_panel(observed)
    require("Current runtime snapshot" not in rendered, "front panel must not show debug snapshot by default")
    require("<pre>" not in rendered, "front panel must not include raw snapshot preformatted output by default")
    require("/state.json" in rendered, "front panel should keep state.json route available")
    require("data-widget-id='bool_input'" in rendered, "rendered control widget missing")
    require("data-widget-id='bool_result'" in rendered, "rendered indicator widget missing")
    require("left:20px;top:32px;width:160px;height:80px" in rendered, "rendered control layout mismatch")
    require("left:240px;top:32px;width:160px;height:80px" in rendered, "rendered indicator layout mismatch")

    if print_json:
        json.dump(observed, sys.stdout, indent=2)
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
