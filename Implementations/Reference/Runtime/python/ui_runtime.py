from __future__ import annotations

import html
import json
import re
import sys
import threading
import urllib.parse
import webbrowser
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any, Dict, Optional

try:
    from .runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
    from .native_kernel import NativeBoolKernelBridge, NativeKernelBridge
    from ..contract_executor import execute_contract_case, load_json as load_contract_json
except ImportError:  # pragma: no cover
    sys.path.append(str(Path(__file__).resolve().parents[1]))
    from runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
    from native_kernel import NativeBoolKernelBridge, NativeKernelBridge
    from contract_executor import execute_contract_case, load_json as load_contract_json


def repo_root() -> Path:
    current = Path(__file__).resolve()
    for candidate in [current, *current.parents]:
        if (candidate / "Examples").is_dir() and (candidate / "Implementations").is_dir():
            return candidate
    raise RuntimeError("Unable to locate the repository root from the Python runtime.")


def default_example06_contract_path() -> Path:
    root = repo_root()
    return root / "Implementations" / "Reference" / "ContractEmitter" / "examples" / "06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json"


def default_example06_wfrog_path() -> Path:
    return repo_root() / "Examples" / "06_boolean_value_roundtrip" / "ui" / "boolean_panel.wfrog"


def wants_example06(value: str | None) -> bool:
    return value in {"06", "6", "example06", "06_boolean_value_roundtrip"}


def parse_bool_input(value: str | bool | None) -> bool:
    if isinstance(value, bool):
        return value
    text = "true" if value is None else str(value).strip().lower()
    if text in {"true", "1", "on"}:
        return True
    if text in {"false", "0", ""}:
        return False
    raise ValueError("Boolean input must be true or false.")


def contract_example_id(contract: dict[str, Any]) -> str | None:
    example_id = contract.get("example_id")
    if isinstance(example_id, str):
        return example_id
    source_ref = contract.get("source_ref")
    if isinstance(source_ref, dict) and isinstance(source_ref.get("example_id"), str):
        return source_ref["example_id"]
    return None


def is_example06_contract(path: str | Path | None) -> bool:
    if path is None:
        return False
    try:
        return contract_example_id(load_contract_json(Path(path))) == "06_boolean_value_roundtrip"
    except Exception:
        return False


def asset_url(asset_ref: object) -> str:
    if isinstance(asset_ref, str) and asset_ref.startswith("asset:"):
        return f"/asset/{asset_ref.split(':', 1)[1]}"
    return ""


def runtime_string(runtime: dict[str, Any], key: str, fallback: str = "") -> str:
    value = runtime.get(key, fallback)
    return str(value if value is not None else fallback)


def runtime_bool(runtime: dict[str, Any], key: str, fallback: bool = False) -> bool:
    value = runtime.get(key)
    return value if isinstance(value, bool) else fallback


def safe_css_color(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    if re.fullmatch(r"#[0-9A-Fa-f]{6}([0-9A-Fa-f]{2})?", text):
        return text
    return fallback


def layout_int(layout: dict[str, Any], key: str, fallback: int) -> int:
    try:
        return int(layout.get(key, fallback))
    except (TypeError, ValueError):
        return fallback


def css_px(value: int | float) -> str:
    return f"{int(round(float(value)))}px"


def css_percent(value: int | float) -> str:
    return f"{float(value):.6g}%"


def pct(value: float, total: float) -> float:
    if total <= 0:
        return 0.0
    return (value / total) * 100.0


def svg_attribute(svg: str, element_id: str, attribute: str) -> str | None:
    tag_match = re.search(rf"<[^>]*\bid=[\"']{re.escape(element_id)}[\"'][^>]*>", svg)
    if not tag_match:
        return None
    attr_match = re.search(rf"\b{re.escape(attribute)}=[\"']([^\"']*)[\"']", tag_match.group(0))
    return attr_match.group(1) if attr_match else None


def svg_attribute_float(svg: str, element_id: str, attribute: str, fallback: float) -> float:
    value = svg_attribute(svg, element_id, attribute)
    if value is None:
        return fallback
    try:
        return float(value)
    except ValueError:
        return fallback


def load_numeric_svg_geometry(asset_path: Path | None) -> dict[str, float]:
    geometry = {
        "view_width": 220.0,
        "view_height": 88.0,
        "label_x": 16.0,
        "label_y": 24.0,
        "value_x": 22.0,
        "value_y": 62.0,
        "value_box_x": 14.0,
        "value_box_y": 40.0,
        "value_box_width": 192.0,
        "value_box_height": 32.0,
    }
    if asset_path is None or not asset_path.exists():
        return geometry

    svg = asset_path.read_text(encoding="utf-8")
    viewbox = re.search(r"\bviewBox=[\"']([^\"']+)[\"']", svg)
    if viewbox:
        parts = viewbox.group(1).replace(",", " ").split()
        if len(parts) == 4:
            try:
                width = float(parts[2])
                height = float(parts[3])
                if width > 0 and height > 0:
                    geometry["view_width"] = width
                    geometry["view_height"] = height
            except ValueError:
                pass

    for anchor_id, x_key, y_key in (
        ("label_anchor", "label_x", "label_y"),
        ("value_anchor", "value_x", "value_y"),
    ):
        transform = svg_attribute(svg, anchor_id, "transform") or ""
        match = re.search(r"translate\(([^)]*)\)", transform)
        if match:
            parts = match.group(1).replace(",", " ").split()
            if len(parts) >= 2:
                try:
                    geometry[x_key] = float(parts[0])
                    geometry[y_key] = float(parts[1])
                except ValueError:
                    pass

    geometry["value_box_x"] = svg_attribute_float(svg, "value_box", "x", geometry["value_box_x"])
    geometry["value_box_y"] = svg_attribute_float(svg, "value_box", "y", geometry["value_box_y"])
    geometry["value_box_width"] = svg_attribute_float(svg, "value_box", "width", geometry["value_box_width"])
    geometry["value_box_height"] = svg_attribute_float(svg, "value_box", "height", geometry["value_box_height"])
    return geometry


def svg_anchor_style(x: float, y: float, geometry: dict[str, float]) -> str:
    return (
        f"left:{css_percent(pct(x, geometry['view_width']))};"
        f"top:{css_percent(pct(y, geometry['view_height']))};"
    )


def svg_box_style(x: float, y: float, width: float, height: float, geometry: dict[str, float]) -> str:
    return (
        f"left:{css_percent(pct(x, geometry['view_width']))};"
        f"top:{css_percent(pct(y, geometry['view_height']))};"
        f"width:{css_percent(pct(width, geometry['view_width']))};"
        f"height:{css_percent(pct(height, geometry['view_height']))};"
    )


def render_numeric_widget(entry: dict[str, Any], widget_state: Any) -> str:
    runtime = entry["runtime"]
    layout = entry["layout"]
    is_control = entry["role"] == "control"
    geometry = load_numeric_svg_geometry(widget_state.asset_path)
    x = layout_int(layout, "x", 0)
    y = layout_int(layout, "y", 0)
    width = layout_int(layout, "width", 160)
    height = layout_int(layout, "height", 48)
    asset_route = asset_url(runtime.get("asset_ref"))
    value = int(runtime.get("value", 0))
    label = runtime_string(runtime, "label", entry["widget_id"])
    value_color = safe_css_color(runtime.get("foreground_color"), "#1f2933")
    label_color = safe_css_color(widget_state.properties.get("label_color"), "#111827")
    visible = runtime_bool(runtime, "visible", True)
    enabled = runtime_bool(runtime, "enabled", True)

    style = (
        f"position:absolute;left:{css_px(x)};top:{css_px(y)};"
        f"width:{css_px(width)};height:{css_px(height)};"
    )
    if not visible:
        style += "display:none;"

    class_name = "numeric-control" if is_control else "numeric-indicator"
    skin = (
        f"<img class='numeric-skin' src='{html.escape(asset_route)}' alt='' aria-hidden='true' />"
        if asset_route
        else "<div class='numeric-skin missing-skin'></div>"
    )
    label_style = svg_anchor_style(geometry["label_x"], geometry["label_y"], geometry)
    value_style = svg_box_style(
        geometry["value_box_x"],
        geometry["value_box_y"],
        geometry["value_box_width"],
        geometry["value_box_height"],
        geometry,
    )

    if is_control:
        value_part = (
            f"<input id='{html.escape(entry['widget_id'])}_value' name='input_value' type='number' min='0' max='65535'"
            " class='numeric-value-overlay numeric-control-editor' data-svg-part='value_box' data-svg-anchor='value_anchor'"
            f" style='{value_style}color:{html.escape(value_color)};' value='{value}'"
            f"{' disabled' if not enabled else ''} />"
        )
    else:
        value_part = (
            "<output class='numeric-value-overlay numeric-indicator-value' data-svg-part='value_box' data-svg-anchor='value_anchor'"
            f" style='{value_style}color:{html.escape(value_color)};'>{value}</output>"
        )

    asset_attr = f" data-asset-route='{html.escape(asset_route)}'" if asset_route else ""
    return (
        f"<section class='frog-widget numeric-widget {class_name}'"
        f" data-widget-id='{html.escape(entry['widget_id'])}'"
        f" data-class-ref='{html.escape(entry['class_ref'])}'"
        f" data-role='{html.escape(entry['role'])}'"
        f" data-frog-visual-law='wfrog-realization-state-map'"
        f"{asset_attr}"
        f" style='{style}'>"
        f"{skin}"
        "<span class='numeric-label-overlay' data-svg-anchor='label_anchor'"
        f" style='{label_style}color:{html.escape(label_color)};'>{html.escape(label)}</span>"
        f"{value_part}</section>"
    )


def state_property(runtime: dict[str, Any], base: str, state: str, fallback: str) -> str:
    return runtime_string(runtime, f"{base}.{state}", fallback)


class BooleanRuntimeCore:
    def __init__(self, *, contract_path: str | Path | None = None, wfrog_path: str | Path | None = None) -> None:
        self.contract_path = Path(contract_path or default_example06_contract_path()).resolve()
        self.wfrog_path = Path(wfrog_path or default_example06_wfrog_path()).resolve()
        self.contract = load_contract_json(self.contract_path)
        self.package = load_contract_json(self.wfrog_path)
        self.panel = self.package["front_panels"][0]
        self.asset_map = {
            item["asset_id"]: (self.wfrog_path.parent / Path(item["path"])).resolve()
            for item in self.package.get("svg_assets", [])
        }
        widgets = {entry["instance_id"]: entry for entry in self.panel.get("widgets", [])}
        self.current_value = bool(widgets.get("bool_input", {}).get("props", {}).get("value", True))
        self.last_result = self.current_value

    def execute(self, control_value: bool | None = None) -> dict[str, Any]:
        if control_value is not None:
            self.current_value = bool(control_value)
        self.last_result = self.current_value
        return self._execution_artifact_from_values(self.current_value, self.last_result)

    def execute_with_native_kernel_bridge(
        self,
        bridge: NativeBoolKernelBridge,
        control_value: bool | None = None,
    ) -> dict[str, Any]:
        if bridge.manifest.source_lowered_unit != "Examples/06_boolean_value_roundtrip/main.lowering.json":
            raise RuntimeError("Unexpected native bool kernel source lowered unit.")
        if control_value is not None:
            self.current_value = bool(control_value)
        result = bridge.run(self.current_value)
        if not result.ok:
            raise RuntimeError(result.diagnostic or "native bool kernel execution failed.")
        self.last_result = result.result
        return self._execution_artifact_from_values(self.current_value, self.last_result)

    def _execution_artifact_from_values(self, input_value: bool, result_value: bool) -> dict[str, Any]:
        artifact = execute_contract_case(
            self.contract,
            {"input_value": input_value},
            {"wfrog": self.package},
        )
        artifact["execution_summary"]["input_value"] = input_value
        artifact["execution_summary"]["result"] = result_value
        artifact["outputs"]["public"]["result"] = result_value
        artifact["outputs"]["ui"]["bool_input"] = input_value
        artifact["outputs"]["ui"]["bool_result"] = result_value
        for widget in artifact["ui_runtime"]["widgets"]:
            if widget.get("widget_id") == "bool_input":
                widget["runtime"]["value"] = input_value
            if widget.get("widget_id") == "bool_result":
                widget["runtime"]["value"] = result_value
        return artifact

    def execution_artifact(self) -> dict[str, Any]:
        return self._execution_artifact_from_values(self.current_value, self.last_result)


def render_boolean_widget(widget: dict[str, Any]) -> str:
    layout = widget["layout"]
    runtime = widget["runtime"]
    is_control = widget["role"] == "control"
    value = bool(runtime["value"])
    visual_state = "true" if value else "false"
    hover_state = "hover_true" if value else "hover_false"
    pressed_state = "pressed_true" if value else "pressed_false"
    transition_state = "transition_true_to_false" if value else "transition_false_to_true"
    variant = runtime_string(runtime, "realization.variant", "circular" if widget["class_ref"].endswith("indicator") else "rectangular")
    next_value = "false" if value else "true"
    caption_align = runtime_string(runtime, "caption.align.horizontal", "left")
    caption_centered = caption_align == "center"
    state_text_visible = runtime_bool(runtime, "state_text.visible", True)
    frame_visible = runtime_bool(runtime, "style.frame.visible", True)

    state_fill = state_property(runtime, "style.inner.fill_color", visual_state, "#8bd86f" if value else "#ffffff")
    hover_fill = state_property(runtime, "style.inner.fill_color", hover_state, "#9be884" if value else "#eef6ff")
    pressed_fill = state_property(runtime, "style.inner.fill_color", pressed_state, "#6fc657" if value else "#dbeafe")
    state_border = state_property(runtime, "style.outer.border_color", visual_state, "#184a24" if value else "#111827")
    hover_border = state_property(runtime, "style.outer.border_color", hover_state, "#166534" if value else "#2563eb")
    pressed_border = state_property(runtime, "style.outer.border_color", pressed_state, "#14532d" if value else "#1d4ed8")
    state_inner_border = state_property(runtime, "style.inner.border_color", visual_state, state_border)
    hover_inner_border = state_property(runtime, "style.inner.border_color", hover_state, hover_border)
    pressed_inner_border = state_property(runtime, "style.inner.border_color", pressed_state, pressed_border)
    text_color = state_property(runtime, "state_text.style.text_color", visual_state, "#0b3d19" if value else "#111827")

    inner_left = runtime_string(runtime, "style.inner.left", "52px" if variant == "circular" else "18px")
    inner_top = runtime_string(runtime, "style.inner.top", "23px" if variant == "circular" else "31px")
    inner_width = runtime_string(runtime, "style.inner.width", "56px" if variant == "circular" else "124px")
    inner_height = runtime_string(runtime, "style.inner.height", "56px" if variant == "circular" else "34px")
    transition_ms = runtime_string(runtime, "style.transition.duration_ms", "120")
    transition_timing = runtime_string(runtime, "style.transition.timing", "ease-out")
    pressed_inset = runtime_string(runtime, "style.pressed.inset", "1px")

    attrs = (
        f" data-widget-id='{html.escape(widget['widget_id'])}'"
        f" data-class-ref='{html.escape(widget['class_ref'])}'"
        f" data-role='{html.escape(widget['role'])}'"
        f" data-asset-ref='{html.escape(str(runtime.get('asset_ref', '')))}'"
        f" data-asset-route='{html.escape(asset_url(runtime.get('asset_ref')))}'"
        f" data-current-value='{'true' if value else 'false'}'"
        f" data-realization-variant='{html.escape(variant)}'"
        " data-frog-visual-law='wfrog-realization-state-map'"
        f" data-frog-visual-state='{visual_state}'"
        f" data-frog-hover-state='{hover_state}'"
        f" data-frog-pressed-state='{pressed_state}'"
        f" data-frog-transition-state='{transition_state}'"
        f" data-frog-state-text-visible='{'true' if state_text_visible else 'false'}'"
        f" data-frog-frame-visible='{'true' if frame_visible else 'false'}'"
    )
    style = (
        f"position:absolute;left:{layout['x']}px;top:{layout['y']}px;"
        f"width:{layout['width']}px;height:{layout['height']}px;"
        f"--boolean-fill:{state_fill};--boolean-hover-fill:{hover_fill};--boolean-pressed-fill:{pressed_fill};"
        f"--boolean-border:{state_border};--boolean-hover-border:{hover_border};--boolean-pressed-border:{pressed_border};"
        f"--boolean-inner-border:{state_inner_border};--boolean-hover-inner-border:{hover_inner_border};--boolean-pressed-inner-border:{pressed_inner_border};"
        f"--boolean-inner-left:{inner_left};--boolean-inner-top:{inner_top};--boolean-inner-width:{inner_width};--boolean-inner-height:{inner_height};"
        f"--boolean-caption-left:{'50%' if caption_centered else '8px'};"
        f"--boolean-caption-transform:{'translateX(-50%)' if caption_centered else 'none'};"
        f"--boolean-caption-text-align:{'center' if caption_centered else 'left'};"
        f"--boolean-text:{text_color};--boolean-transition:{transition_ms}ms {transition_timing};--boolean-pressed-inset:{pressed_inset};"
    )
    skin = (
        f"<span class='boolean-state-face' data-frog-part='inner_face' aria-hidden='true'></span>"
        f"<img class='boolean-skin' src='{html.escape(asset_url(runtime.get('asset_ref')))}' alt='' aria-hidden='true'>"
    )
    overlays = f"<span class='boolean-caption-overlay' data-frog-part='caption'>{html.escape(runtime_string(runtime, 'caption.text', widget['widget_id']))}</span>"
    if state_text_visible:
        text = runtime_string(runtime, "state_text.true_text" if value else "state_text.false_text", "TRUE" if value else "FALSE")
        overlays += f"<span class='boolean-state-overlay' data-frog-part='state_text'>{html.escape(text)}</span>"

    if is_control:
        return (
            f"<button class='frog-widget boolean-widget boolean-control' type='submit' name='input_value' value='{next_value}'"
            f" data-toggle-target='{next_value}' aria-pressed='{'true' if value else 'false'}'{attrs} style='{style}'>{skin}{overlays}</button>"
        )
    return f"<section class='frog-widget boolean-widget boolean-indicator' aria-readonly='true'{attrs} style='{style}'>{skin}{overlays}</section>"


class BrowserUiRuntime:
    def __init__(
        self,
        *,
        contract_path: str | Path | None = None,
        wfrog_path: str | Path | None = None,
        host: str = "127.0.0.1",
        port: int = 0,
        open_browser: bool = True,
        native_kernel_bridge: NativeKernelBridge | None = None,
    ) -> None:
        self.runtime = Slice05RuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
        self.host = host
        self.port = port
        self.open_browser = open_browser
        self.native_kernel_bridge = native_kernel_bridge
        self.last_error: Optional[str] = None
        self._httpd: Optional[ThreadingHTTPServer] = None

    def build_server(self) -> ThreadingHTTPServer:
        runtime = self

        class Handler(BaseHTTPRequestHandler):
            def do_GET(self) -> None:  # noqa: N802
                runtime._handle_get(self)

            def do_POST(self) -> None:  # noqa: N802
                runtime._handle_post(self)

            def log_message(self, format: str, *args: Any) -> None:  # noqa: A003
                return

        self._httpd = ThreadingHTTPServer((self.host, self.port), Handler)
        return self._httpd

    def state_snapshot(self) -> Dict[str, Any]:
        return self.runtime.execution_artifact()

    def render_html(self) -> str:
        snapshot = self.state_snapshot()
        widgets = {entry["widget_id"]: entry for entry in snapshot["ui_runtime"]["widgets"]}
        ctrl_html = render_numeric_widget(widgets["ctrl_input"], self.runtime.widgets["ctrl_input"])
        ind_html = render_numeric_widget(widgets["ind_result"], self.runtime.widgets["ind_result"])
        panel_layout = snapshot["ui_runtime"]["panel"]["layout"]
        panel_width = layout_int(panel_layout, "width", 500)
        panel_height = layout_int(panel_layout, "height", 170)

        error_block = ""
        if self.last_error:
            error_block = (
                "<div class='diagnostic error'>"
                + html.escape(self.last_error)
                + "</div>"
            )
        uses_native_kernel = self.native_kernel_bridge is not None

        return f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>{html.escape(snapshot['ui_runtime']['panel']['title'])}</title>
<style>
body {{
  font-family: Segoe UI, Arial, sans-serif;
  margin: 24px;
  background: #f3f6f8;
  color: #1f2933;
}}
h1 {{ margin: 0 0 12px 0; font-size: 24px; }}
p.meta {{ margin: 0 0 20px 0; color: #52606d; }}
.runtime-facts {{
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: -8px 0 18px 0;
}}
.runtime-facts div {{
  display: flex;
  gap: 6px;
  align-items: baseline;
  padding: 6px 8px;
  border: 1px solid #d9e2ec;
  border-radius: 6px;
  background: #ffffff;
}}
.runtime-facts dt {{
  margin: 0;
  color: #52606d;
  font-size: 11px;
  font-weight: 700;
  text-transform: uppercase;
}}
.runtime-facts dd {{
  margin: 0;
  color: #1f2933;
  font-size: 12px;
  font-weight: 600;
}}
.front-panel {{
  position: relative;
  background: #ffffff;
  border-radius: 10px;
  box-shadow: 0 4px 14px rgba(15, 23, 42, 0.08);
  overflow: hidden;
}}
.frog-widget {{
  position: absolute;
  box-sizing: border-box;
}}
.numeric-widget {{
  font-family: Segoe UI, Arial, sans-serif;
}}
.numeric-skin {{
  position: absolute;
  inset: 0;
  width: 100%;
  height: 100%;
  object-fit: fill;
  display: block;
}}
.missing-skin {{
  background: #e5e7eb;
  border: 1px solid #9ca3af;
  border-radius: 6px;
}}
.numeric-label-overlay {{
  position: absolute;
  transform: translateY(-50%);
  font-size: 10px;
  font-weight: 700;
  line-height: 1;
  white-space: nowrap;
  pointer-events: none;
}}
.numeric-value-overlay {{
  position: absolute;
  box-sizing: border-box;
  font-family: Consolas, Segoe UI Mono, monospace;
  font-size: 11px;
  font-weight: 700;
  line-height: 1;
  border: 0;
  background: transparent;
}}
.numeric-control-editor {{
  padding: 0 4px;
  border-radius: 4px;
  outline: 1px solid rgba(15,23,42,0.18);
  background: rgba(255,255,255,0.72);
  appearance: textfield;
}}
.numeric-control-editor:focus {{
  outline: 2px solid #0f62fe;
  background: rgba(255,255,255,0.9);
}}
.numeric-indicator-value {{
  display: flex;
  align-items: center;
  padding: 0 4px;
  pointer-events: none;
}}
.actions {{
  margin-top: 16px;
  display: flex;
  gap: 12px;
  align-items: center;
}}
button {{
  padding: 8px 14px;
  border: 0;
  border-radius: 6px;
  cursor: pointer;
  background: #0f62fe;
  color: #ffffff;
  font-weight: 600;
}}
.diagnostic {{
  margin: 12px 0;
  padding: 10px 12px;
  border-radius: 6px;
}}
.diagnostic.error {{
  background: #fff1f2;
  color: #9f1239;
  border: 1px solid #fecdd3;
}}
summary {{
  cursor: pointer;
  margin-top: 16px;
  font-weight: 600;
}}
pre {{
  white-space: pre-wrap;
  word-break: break-word;
  background: #0b1020;
  color: #dbeafe;
  padding: 12px;
  border-radius: 8px;
  font-size: 12px;
}}
</style>
</head>
<body>
<h1>{html.escape(snapshot['ui_runtime']['panel']['title'])}</h1>
<p class="meta">Example 05 - .wfrog front panel + Python runtime</p>
<dl class="runtime-facts" aria-label="Runtime facts">
  <div><dt>Runtime</dt><dd>Python reference runtime</dd></div>
  <div><dt>Execution</dt><dd>{'native kernel bridge' if uses_native_kernel else 'contract executor'}</dd></div>
  <div><dt>Compiler backend</dt><dd>{'LLVM native kernel artifact' if uses_native_kernel else 'none in runtime path'}</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(snapshot['ui_runtime']['panel']['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="{'llvm' if uses_native_kernel else 'none'}" data-execution-path="{'native_kernel_bridge' if uses_native_kernel else 'contract_executor'}" style="width:{css_px(panel_width)};height:{css_px(panel_height)};">
    {ctrl_html}
    {ind_html}
  </div>
    <div class="actions">
      <button type="submit">Run Example 05</button>
      <a href="/state.json">state.json</a>
    </div>
  </form>
  <details>
    <summary>Current runtime snapshot</summary>
    <pre>{html.escape(json.dumps(snapshot, indent=2))}</pre>
  </details>
</body>
</html>
"""

    def _serve_bytes(self, handler: BaseHTTPRequestHandler, body: bytes, content_type: str, status: int = 200) -> None:
        handler.send_response(status)
        handler.send_header("Content-Type", content_type)
        handler.send_header("Content-Length", str(len(body)))
        handler.end_headers()
        handler.wfile.write(body)

    def _redirect(self, handler: BaseHTTPRequestHandler, target: str) -> None:
        handler.send_response(HTTPStatus.SEE_OTHER)
        handler.send_header("Location", target)
        handler.end_headers()

    def _handle_get(self, handler: BaseHTTPRequestHandler) -> None:
        parsed = urllib.parse.urlparse(handler.path)
        path = parsed.path
        if path == "/":
            payload = self.render_html().encode("utf-8")
            self._serve_bytes(handler, payload, "text/html; charset=utf-8")
            return
        if path == "/state.json":
            payload = json.dumps(self.state_snapshot(), indent=2).encode("utf-8")
            self._serve_bytes(handler, payload, "application/json; charset=utf-8")
            return
        if path.startswith("/asset/"):
            asset_id = path.split("/", 2)[2]
            asset_path = self.runtime.asset_map.get(asset_id)
            if asset_path is None or not asset_path.exists():
                self._serve_bytes(handler, b"missing asset", "text/plain; charset=utf-8", status=404)
                return
            self._serve_bytes(handler, asset_path.read_bytes(), "image/svg+xml")
            return
        self._serve_bytes(handler, b"not found", "text/plain; charset=utf-8", status=404)

    def _handle_post(self, handler: BaseHTTPRequestHandler) -> None:
        parsed = urllib.parse.urlparse(handler.path)
        if parsed.path != "/run":
            self._serve_bytes(handler, b"not found", "text/plain; charset=utf-8", status=404)
            return
        length = int(handler.headers.get("Content-Length", "0"))
        body = handler.rfile.read(length).decode("utf-8")
        form = urllib.parse.parse_qs(body, keep_blank_values=True)
        raw_value = form.get("input_value", ["0"])[0]
        try:
            if self.native_kernel_bridge is None:
                self.runtime.execute(control_value=int(raw_value))
            else:
                self.runtime.execute_with_native_kernel_bridge(
                    self.native_kernel_bridge,
                    control_value=int(raw_value),
                )
            self.last_error = None
        except Exception as exc:  # pragma: no cover - exact error rendering is covered through state.json/headless tests
            self.last_error = str(exc)
        self._redirect(handler, "/")

    def serve(self) -> None:
        httpd = self.build_server()
        address = f"http://{httpd.server_address[0]}:{httpd.server_address[1]}/"
        if self.open_browser:
            webbrowser.open(address)
        print(address)
        try:
            httpd.serve_forever()
        finally:
            httpd.server_close()

    def serve_in_thread(self) -> tuple[ThreadingHTTPServer, threading.Thread]:
        httpd = self.build_server()
        thread = threading.Thread(target=httpd.serve_forever, daemon=True)
        thread.start()
        return httpd, thread


class BooleanBrowserUiRuntime:
    def __init__(
        self,
        *,
        contract_path: str | Path | None = None,
        wfrog_path: str | Path | None = None,
        host: str = "127.0.0.1",
        port: int = 0,
        open_browser: bool = True,
        native_kernel_bridge: NativeBoolKernelBridge | None = None,
    ) -> None:
        self.runtime = BooleanRuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
        self.host = host
        self.port = port
        self.open_browser = open_browser
        self.native_kernel_bridge = native_kernel_bridge
        self.last_error: Optional[str] = None
        self._httpd: Optional[ThreadingHTTPServer] = None

    def build_server(self) -> ThreadingHTTPServer:
        runtime = self

        class Handler(BaseHTTPRequestHandler):
            def do_GET(self) -> None:  # noqa: N802
                runtime._handle_get(self)

            def do_POST(self) -> None:  # noqa: N802
                runtime._handle_post(self)

            def log_message(self, format: str, *args: Any) -> None:  # noqa: A003
                return

        self._httpd = ThreadingHTTPServer((self.host, self.port), Handler)
        return self._httpd

    def state_snapshot(self) -> dict[str, Any]:
        return self.runtime.execution_artifact()

    def render_html(self) -> str:
        snapshot = self.state_snapshot()
        panel = snapshot["ui_runtime"]["panel"]
        widgets = snapshot["ui_runtime"]["widgets"]
        panel_layout = panel["layout"]
        error_block = ""
        if self.last_error:
            error_block = "<div class='diagnostic error'>" + html.escape(self.last_error) + "</div>"
        uses_native_kernel = self.native_kernel_bridge is not None

        rendered_widgets = "".join(render_boolean_widget(widget) for widget in widgets)
        return f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>{html.escape(panel['title'])}</title>
<style>
body{{font-family:Segoe UI,Arial,sans-serif;margin:24px;background:#f3f6f8;color:#1f2933;}}
h1{{font-size:24px;margin:0 0 12px 0;}}
p.meta{{margin:0 0 20px 0;color:#52606d;}}
.runtime-facts{{display:flex;flex-wrap:wrap;gap:8px;margin:-8px 0 18px 0;}}
.runtime-facts div{{display:flex;gap:6px;align-items:baseline;padding:6px 8px;border:1px solid #d9e2ec;border-radius:6px;background:#ffffff;}}
.runtime-facts dt{{margin:0;color:#52606d;font-size:11px;font-weight:700;text-transform:uppercase;}}
.runtime-facts dd{{margin:0;color:#1f2933;font-size:12px;font-weight:600;}}
.front-panel{{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:hidden;}}
.frog-widget{{position:absolute;box-sizing:border-box;}}
.boolean-widget{{border:0;padding:0;background:transparent;font:inherit;color:inherit;overflow:visible;}}
.boolean-control{{cursor:pointer;}}
.boolean-indicator{{pointer-events:none;}}
.boolean-skin{{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;pointer-events:none;z-index:2;}}
.boolean-caption-overlay{{position:absolute;left:var(--boolean-caption-left);top:6px;transform:var(--boolean-caption-transform);text-align:var(--boolean-caption-text-align);font-size:14px;font-weight:600;line-height:1;color:#1f2933;white-space:nowrap;pointer-events:none;z-index:3;}}
.boolean-state-face{{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:2px solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}}
.boolean-widget[data-realization-variant='circular'] .boolean-state-face{{border-radius:50%;}}
.boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{{box-shadow:none;}}
.boolean-control:hover .boolean-state-face{{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-inner-border);box-shadow:inset 0 1px 0 rgba(255,255,255,.72),0 2px 5px rgba(15,23,42,.18);}}
.boolean-control[data-frog-frame-visible='false']:hover .boolean-state-face{{box-shadow:none;}}
.boolean-control:active .boolean-state-face{{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-inner-border);box-shadow:inset 0 2px 4px rgba(15,23,42,.22);transform:translateY(var(--boolean-pressed-inset));}}
.boolean-control[data-frog-frame-visible='false']:active .boolean-state-face{{box-shadow:none;}}
.boolean-control:focus-visible .boolean-state-face{{outline:2px solid #2563eb;outline-offset:2px;}}
.boolean-state-overlay{{position:absolute;left:0;right:0;top:49px;transform:translateY(-50%);text-align:center;font-size:18px;font-weight:700;line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;}}
.actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}
.state-link{{font-size:16px;}}
.diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}
.diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}
</style>
</head>
<body>
<h1>{html.escape(panel['title'])}</h1>
<p class="meta">Example 06 - .wfrog front panel + Default Boolean realization assets + Python runtime</p>
<dl class="runtime-facts" aria-label="Runtime facts">
  <div><dt>Runtime</dt><dd>Python reference runtime</dd></div>
  <div><dt>Execution</dt><dd>{'native kernel bridge' if uses_native_kernel else 'boolean contract executor'}</dd></div>
  <div><dt>Compiler backend</dt><dd>{'LLVM native bool kernel artifact' if uses_native_kernel else 'none for Example 06'}</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(panel['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="{'llvm' if uses_native_kernel else 'none'}" data-execution-path="{'native_kernel_bridge' if uses_native_kernel else 'python_boolean_contract_executor'}" style="width:{panel_layout['width']}px;height:{panel_layout['height']}px;">
    {rendered_widgets}
  </div>
  <div class="actions"><a class="state-link" href="/state.json">state.json</a></div>
</form>
</body>
</html>
"""

    def _serve_bytes(self, handler: BaseHTTPRequestHandler, body: bytes, content_type: str, status: int = 200) -> None:
        handler.send_response(status)
        handler.send_header("Content-Type", content_type)
        handler.send_header("Content-Length", str(len(body)))
        handler.end_headers()
        handler.wfile.write(body)

    def _redirect(self, handler: BaseHTTPRequestHandler, target: str) -> None:
        handler.send_response(HTTPStatus.SEE_OTHER)
        handler.send_header("Location", target)
        handler.end_headers()

    def _handle_get(self, handler: BaseHTTPRequestHandler) -> None:
        parsed = urllib.parse.urlparse(handler.path)
        path = parsed.path
        if path == "/":
            self._serve_bytes(handler, self.render_html().encode("utf-8"), "text/html; charset=utf-8")
            return
        if path == "/state.json":
            self._serve_bytes(handler, json.dumps(self.state_snapshot(), indent=2).encode("utf-8"), "application/json; charset=utf-8")
            return
        if path.startswith("/asset/"):
            asset_id = path.split("/", 2)[2]
            asset_path = self.runtime.asset_map.get(asset_id)
            if asset_path is None or not asset_path.exists():
                self._serve_bytes(handler, b"missing asset", "text/plain; charset=utf-8", status=404)
                return
            self._serve_bytes(handler, asset_path.read_bytes(), "image/svg+xml")
            return
        self._serve_bytes(handler, b"not found", "text/plain; charset=utf-8", status=404)

    def _handle_post(self, handler: BaseHTTPRequestHandler) -> None:
        parsed = urllib.parse.urlparse(handler.path)
        if parsed.path != "/run":
            self._serve_bytes(handler, b"not found", "text/plain; charset=utf-8", status=404)
            return
        length = int(handler.headers.get("Content-Length", "0"))
        body = handler.rfile.read(length).decode("utf-8")
        form = urllib.parse.parse_qs(body, keep_blank_values=True)
        raw_value = form.get("input_value", ["false"])[0]
        try:
            if self.native_kernel_bridge is None:
                self.runtime.execute(control_value=parse_bool_input(raw_value))
            else:
                self.runtime.execute_with_native_kernel_bridge(
                    self.native_kernel_bridge,
                    control_value=parse_bool_input(raw_value),
                )
            self.last_error = None
        except Exception as exc:  # pragma: no cover
            self.last_error = str(exc)
        self._redirect(handler, "/")

    def serve(self) -> None:
        httpd = self.build_server()
        address = f"http://{httpd.server_address[0]}:{httpd.server_address[1]}/"
        if self.open_browser:
            webbrowser.open(address)
        print(address)
        try:
            httpd.serve_forever()
        finally:
            httpd.server_close()

    def serve_in_thread(self) -> tuple[ThreadingHTTPServer, threading.Thread]:
        httpd = self.build_server()
        thread = threading.Thread(target=httpd.serve_forever, daemon=True)
        thread.start()
        return httpd, thread


def build_runtime(
    *,
    example: str | None = None,
    contract_path: str | Path | None = None,
    wfrog_path: str | Path | None = None,
    host: str = "127.0.0.1",
    port: int = 0,
    open_browser: bool = True,
    native_kernel_bridge: NativeKernelBridge | NativeBoolKernelBridge | None = None,
) -> BrowserUiRuntime | BooleanBrowserUiRuntime:
    if wants_example06(example) or is_example06_contract(contract_path):
        return BooleanBrowserUiRuntime(
            contract_path=contract_path or default_example06_contract_path(),
            wfrog_path=wfrog_path or default_example06_wfrog_path(),
            host=host,
            port=port,
            open_browser=open_browser,
            native_kernel_bridge=native_kernel_bridge if isinstance(native_kernel_bridge, NativeBoolKernelBridge) else None,
        )
    return BrowserUiRuntime(
        contract_path=contract_path or default_contract_path(),
        wfrog_path=wfrog_path or default_wfrog_path(),
        host=host,
        port=port,
        open_browser=open_browser,
        native_kernel_bridge=native_kernel_bridge if isinstance(native_kernel_bridge, NativeKernelBridge) else None,
    )
