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
    from .runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path, load_source_front_panel_from_contract
    from .native_kernel import NativeBoolKernelBridge, NativeEnumKernelBridge, NativeKernelBridge, NativeStringKernelBridge
    from ..contract_executor import execute_contract_case, load_json as load_contract_json
except ImportError:  # pragma: no cover
    sys.path.append(str(Path(__file__).resolve().parents[1]))
    from runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path, load_source_front_panel_from_contract
    from native_kernel import NativeBoolKernelBridge, NativeEnumKernelBridge, NativeKernelBridge, NativeStringKernelBridge
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


def default_example07_contract_path() -> Path:
    root = repo_root()
    return root / "Implementations" / "Reference" / "ContractEmitter" / "examples" / "07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json"


def default_example07_wfrog_path() -> Path:
    return repo_root() / "Examples" / "07_string_value_roundtrip" / "ui" / "string_panel.wfrog"


def default_example08_contract_path() -> Path:
    root = repo_root()
    return root / "Implementations" / "Reference" / "ContractEmitter" / "examples" / "08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json"


def default_example08_wfrog_path() -> Path:
    return repo_root() / "Examples" / "08_enum_value_roundtrip" / "ui" / "enum_panel.wfrog"


def default_example09_contract_path() -> Path:
    root = repo_root()
    return root / "Implementations" / "Reference" / "ContractEmitter" / "examples" / "09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json"


def default_example09_wfrog_path() -> Path:
    return repo_root() / "Examples" / "09_path_value_roundtrip" / "ui" / "path_panel.wfrog"


def default_example10_contract_path() -> Path:
    root = repo_root()
    return root / "Implementations" / "Reference" / "ContractEmitter" / "examples" / "10_button_press_to_boolean.reference_host_runtime_ui_binding.contract.json"


def default_example10_wfrog_path() -> Path:
    return repo_root() / "Examples" / "10_button_press_to_boolean" / "ui" / "button_panel.wfrog"


def wants_example06(value: str | None) -> bool:
    return value in {"06", "6", "example06", "06_boolean_value_roundtrip"}


def wants_example07(value: str | None) -> bool:
    return value in {"07", "7", "example07", "07_string_value_roundtrip"}


def wants_example08(value: str | None) -> bool:
    return value in {"08", "8", "example08", "08_enum_value_roundtrip"}


def wants_example09(value: str | None) -> bool:
    return value in {"09", "9", "example09", "09_path_value_roundtrip"}


def wants_example10(value: str | None) -> bool:
    return value in {"10", "example10", "10_button_press_to_boolean"}


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


def is_example07_contract(path: str | Path | None) -> bool:
    if path is None:
        return False
    try:
        return contract_example_id(load_contract_json(Path(path))) == "07_string_value_roundtrip"
    except Exception:
        return False


def is_example08_contract(path: str | Path | None) -> bool:
    if path is None:
        return False
    try:
        return contract_example_id(load_contract_json(Path(path))) == "08_enum_value_roundtrip"
    except Exception:
        return False


def is_example09_contract(path: str | Path | None) -> bool:
    if path is None:
        return False
    try:
        return contract_example_id(load_contract_json(Path(path))) == "09_path_value_roundtrip"
    except Exception:
        return False


def is_example10_contract(path: str | Path | None) -> bool:
    if path is None:
        return False
    try:
        return contract_example_id(load_contract_json(Path(path))) == "10_button_press_to_boolean"
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


def runtime_number(runtime: dict[str, Any], key: str, fallback: float) -> float:
    value = runtime.get(key)
    if isinstance(value, (int, float)):
        return float(value)
    return fallback


def safe_css_color(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    if text == "transparent":
        return text
    if re.fullmatch(r"#[0-9A-Fa-f]{6}([0-9A-Fa-f]{2})?", text):
        return text
    return fallback


def safe_css_length(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    if re.fullmatch(r"[0-9]+(\.[0-9]+)?px", text):
        return text
    return fallback


def safe_css_signed_length(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    if re.fullmatch(r"-?[0-9]+(\.[0-9]+)?px", text):
        return text
    return fallback


def safe_css_font_weight(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    if text in {"normal", "bold", "lighter", "bolder"}:
        return text
    if re.fullmatch(r"[1-9]00", text):
        weight = int(text)
        if 100 <= weight <= 900:
            return text
    return fallback


def safe_css_font_style(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    return text if text in {"normal", "italic", "oblique"} else fallback


def safe_css_font_family(value: object, fallback: str) -> str:
    text = str(value if value is not None else "")
    if re.fullmatch(r"[A-Za-z0-9 ,._-]+", text):
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


def svg_child_rect_attribute(svg: str, group_id: str, attribute: str) -> str | None:
    group_match = re.search(rf"<g[^>]*\bid=[\"']{re.escape(group_id)}[\"'][^>]*>", svg)
    if not group_match:
        return None
    group_end = svg.find("</g>", group_match.end())
    if group_end < 0:
        return None
    rect_match = re.search(r"<rect[^>]*>", svg[group_match.end():group_end])
    if not rect_match:
        return None
    attr_match = re.search(rf"\b{re.escape(attribute)}=[\"']([^\"']*)[\"']", rect_match.group(0))
    return attr_match.group(1) if attr_match else None


def svg_child_rect_attribute_float(svg: str, group_id: str, attribute: str, fallback: float) -> float:
    value = svg_child_rect_attribute(svg, group_id, attribute)
    if value is None:
        return fallback
    try:
        return float(value)
    except ValueError:
        return fallback


def load_numeric_svg_geometry(asset_path: Path | None) -> dict[str, float]:
    geometry = {
        "view_width": 380.0,
        "view_height": 150.0,
        "caption_x": 16.0,
        "caption_y": 46.0,
        "value_face_x": 22.0,
        "value_face_y": 82.0,
        "value_face_width": 214.0,
        "value_face_height": 28.0,
        "increment_up_x": 246.0,
        "increment_up_y": 82.0,
        "increment_up_width": 30.0,
        "increment_up_height": 13.0,
        "increment_down_x": 246.0,
        "increment_down_y": 97.0,
        "increment_down_width": 30.0,
        "increment_down_height": 13.0,
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

    geometry["caption_x"] = svg_attribute_float(svg, "caption_text", "x", geometry["caption_x"])
    geometry["caption_y"] = svg_attribute_float(svg, "caption_text", "y", geometry["caption_y"])
    geometry["value_face_x"] = svg_attribute_float(svg, "value_face", "x", geometry["value_face_x"])
    geometry["value_face_y"] = svg_attribute_float(svg, "value_face", "y", geometry["value_face_y"])
    geometry["value_face_width"] = svg_attribute_float(svg, "value_face", "width", geometry["value_face_width"])
    geometry["value_face_height"] = svg_attribute_float(svg, "value_face", "height", geometry["value_face_height"])
    geometry["increment_up_x"] = svg_child_rect_attribute_float(svg, "increment_up", "x", geometry["increment_up_x"])
    geometry["increment_up_y"] = svg_child_rect_attribute_float(svg, "increment_up", "y", geometry["increment_up_y"])
    geometry["increment_up_width"] = svg_child_rect_attribute_float(svg, "increment_up", "width", geometry["increment_up_width"])
    geometry["increment_up_height"] = svg_child_rect_attribute_float(svg, "increment_up", "height", geometry["increment_up_height"])
    geometry["increment_down_x"] = svg_child_rect_attribute_float(svg, "increment_down", "x", geometry["increment_down_x"])
    geometry["increment_down_y"] = svg_child_rect_attribute_float(svg, "increment_down", "y", geometry["increment_down_y"])
    geometry["increment_down_width"] = svg_child_rect_attribute_float(svg, "increment_down", "width", geometry["increment_down_width"])
    geometry["increment_down_height"] = svg_child_rect_attribute_float(svg, "increment_down", "height", geometry["increment_down_height"])
    return geometry


def svg_anchor_style(x: float, y: float, geometry: dict[str, float]) -> str:
    return (
        f"left:{css_percent(pct(x, geometry['view_width']))};"
        f"top:{css_percent(pct(y, geometry['view_height']))};"
    )


def caption_transform_for_align(align: str) -> str:
    if align == "center":
        return "translate(-50%,-50%)"
    if align in {"right", "end"}:
        return "translate(-100%,-50%)"
    return "translateY(-50%)"


def caption_text_align(align: str) -> str:
    if align == "center":
        return "center"
    if align in {"right", "end"}:
        return "right"
    return "left"


def caption_anchor_style(runtime: dict[str, Any], geometry: dict[str, float]) -> str:
    x = runtime_number(runtime, "caption.anchor.x", geometry["caption_x"])
    y = runtime_number(runtime, "caption.anchor.y", geometry["caption_y"])
    align = runtime_string(runtime, "caption.align.horizontal", "left")
    style = svg_anchor_style(x, y, geometry)
    style += f"transform:{caption_transform_for_align(align)};"
    style += f"text-align:{caption_text_align(align)};"
    if not runtime_bool(runtime, "caption.visible", True):
        style += "display:none;"
    return style


def svg_box_style(x: float, y: float, width: float, height: float, geometry: dict[str, float]) -> str:
    return (
        f"left:{css_percent(pct(x, geometry['view_width']))};"
        f"top:{css_percent(pct(y, geometry['view_height']))};"
        f"width:{css_percent(pct(width, geometry['view_width']))};"
        f"height:{css_percent(pct(height, geometry['view_height']))};"
    )


def runtime_int(runtime: dict[str, Any], key: str, fallback: int) -> int:
    try:
        value = int(runtime.get(key, fallback))
    except (TypeError, ValueError):
        value = fallback
    return value if value >= 0 else fallback


def render_numeric_skin(widget_state: Any, *, is_control: bool, value_face_color: str) -> str:
    asset_path = getattr(widget_state, "asset_path", None)
    if asset_path is None or not asset_path.exists():
        return "<div class='numeric-skin missing-skin'></div>"
    properties = widget_state.properties
    frame_fill = safe_css_color(properties.get("style.frame.fill_color"), "#ffffff")
    frame_stroke = safe_css_color(properties.get("style.frame.border_color"), "#000000")
    frame_stroke_width = safe_css_length(properties.get("style.frame.border_width"), "2px")
    value_face_fill = safe_css_color(properties.get("style.value_face.fill_color"), value_face_color)
    value_face_stroke = safe_css_color(properties.get("style.value_face.border_color"), "transparent")
    value_face_stroke_width = safe_css_length(properties.get("style.value_face.border_width"), "0px")
    step_fill = safe_css_color(properties.get("style.increment_button.fill_color.normal"), value_face_color)
    step_symbol = safe_css_color(properties.get("style.increment_button.symbol_color.normal"), "#ffffff")
    style = (
        "--frog-numeric-caption-display:none;"
        "--frog-numeric-text-display:none;"
        f"--frog-numeric-frame-fill:{html.escape(frame_fill)};"
        f"--frog-numeric-frame-stroke:{html.escape(frame_stroke)};"
        f"--frog-numeric-frame-stroke-width:{html.escape(frame_stroke_width)};"
        f"--frog-numeric-unit-display:{'inline' if bool(properties.get('unit_label.visible', False)) else 'none'};"
        f"--frog-numeric-radix-display:{'inline' if bool(properties.get('display.radix_visible', False)) else 'none'};"
        f"--frog-numeric-spinner-display:{'inline' if is_control and bool(properties.get('display.increment_buttons_visible', True)) else 'none'};"
        f"--frog-numeric-value-face-fill:{html.escape(value_face_fill)};"
        f"--frog-numeric-value-face-stroke:{html.escape(value_face_stroke)};"
        f"--frog-numeric-value-face-stroke-width:{html.escape(value_face_stroke_width)};"
        f"--frog-numeric-spinner-fill:{html.escape(step_fill)};"
        f"--frog-numeric-spinner-stroke:{html.escape(step_symbol)};"
    )
    return f"<div class='numeric-skin' aria-hidden='true' style='{style}'>{asset_path.read_text(encoding='utf-8')}</div>"


def numeric_step_button_state_style(widget_state: Any) -> str:
    properties = widget_state.properties
    normal_fill = safe_css_color(properties.get("style.increment_button.fill_color.normal"), "#5B9BD5")
    pressed_fill = safe_css_color(properties.get("style.increment_button.fill_color.pressed"), "#2B4F7B")
    normal_border = safe_css_color(properties.get("style.increment_button.border_color.normal"), "transparent")
    pressed_border = safe_css_color(properties.get("style.increment_button.border_color.pressed"), normal_border)
    normal_symbol = safe_css_color(properties.get("style.increment_button.symbol_color.normal"), "#ffffff")
    pressed_symbol = safe_css_color(properties.get("style.increment_button.symbol_color.pressed"), normal_symbol)
    return (
        f"--frog-numeric-step-fill:{html.escape(normal_fill)};"
        f"--frog-numeric-step-fill-pressed:{html.escape(pressed_fill)};"
        f"--frog-numeric-step-border:{html.escape(normal_border)};"
        f"--frog-numeric-step-border-pressed:{html.escape(pressed_border)};"
        f"--frog-numeric-step-symbol:{html.escape(normal_symbol)};"
        f"--frog-numeric-step-symbol-pressed:{html.escape(pressed_symbol)};"
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
    label = str(widget_state.properties.get("caption.text", widget_state.properties.get("label", entry["widget_id"])))
    value_face_color = safe_css_color(widget_state.properties.get("foreground_color"), "#ffffff")
    label_color = safe_css_color(
        widget_state.properties.get("style.caption.text_color", widget_state.properties.get("label_color")),
        "#111827",
    )
    label_size = safe_css_length(widget_state.properties.get("style.caption.font_size"), "12px")
    label_weight = safe_css_font_weight(widget_state.properties.get("style.caption.font_weight"), "400")
    label_family = safe_css_font_family(
        widget_state.properties.get("style.caption.font_family"),
        "system-ui, Segoe UI, Arial, sans-serif",
    )
    text_color = safe_css_color(widget_state.properties.get("style.text_value.color"), "#111827")
    text_size = safe_css_length(widget_state.properties.get("style.text_value.font_size"), "11px")
    text_weight = safe_css_font_weight(widget_state.properties.get("style.text_value.font_weight"), "700")
    text_family = safe_css_font_family(
        widget_state.properties.get("style.text_value.font_family"),
        "Consolas, Segoe UI Mono, monospace",
    )
    visible = runtime_bool(runtime, "visible", True)
    enabled = runtime_bool(runtime, "enabled", True)
    minimum = runtime_int(widget_state.properties, "data_entry.minimum", 0)
    maximum = runtime_int(widget_state.properties, "data_entry.maximum", 65535)
    step = max(1, runtime_int(widget_state.properties, "data_entry.increment_step", 1))

    style = (
        f"position:absolute;left:{css_px(x)};top:{css_px(y)};"
        f"width:{css_px(width)};height:{css_px(height)};"
        f"--frog-numeric-caption-color:{html.escape(label_color)};"
        f"--frog-numeric-caption-font-size:{html.escape(label_size)};"
        f"--frog-numeric-caption-font-weight:{html.escape(label_weight)};"
        f"--frog-numeric-caption-font-family:{html.escape(label_family)};"
        f"--frog-numeric-text-color:{html.escape(text_color)};"
        f"--frog-numeric-text-font-size:{html.escape(text_size)};"
        f"--frog-numeric-text-font-weight:{html.escape(text_weight)};"
        f"--frog-numeric-text-font-family:{html.escape(text_family)};"
    )
    if not visible:
        style += "display:none;"

    class_name = "numeric-control" if is_control else "numeric-indicator"
    skin = render_numeric_skin(widget_state, is_control=is_control, value_face_color=value_face_color)
    label_style = caption_anchor_style(widget_state.properties, geometry)
    value_style = svg_box_style(
        geometry["value_face_x"],
        geometry["value_face_y"],
        geometry["value_face_width"],
        geometry["value_face_height"],
        geometry,
    )

    if is_control:
        value_part = (
            f"<input id='{html.escape(entry['widget_id'])}_value' name='input_value' type='number' min='{minimum}' max='{maximum}' step='{step}'"
            " class='numeric-value-overlay numeric-control-editor' data-frog-part='text_value' data-svg-anchor='text_value.center'"
            f" style='{value_style}' value='{value}'"
            f"{' disabled' if not enabled else ''} />"
        )
        if bool(widget_state.properties.get("display.increment_buttons_visible", True)):
            step_state_style = numeric_step_button_state_style(widget_state)
            value_part += (
                "<button type='button' class='numeric-step-overlay numeric-increment'"
                f" data-target='{html.escape(entry['widget_id'])}_value' data-step='{step}'"
                f" data-frog-part='increment_up' data-frog-method='increment' data-frog-button-state-law='normal-pressed' aria-label='Increment {html.escape(label)}'"
                f" style='{svg_box_style(geometry['increment_up_x'], geometry['increment_up_y'], geometry['increment_up_width'], geometry['increment_up_height'], geometry)}{step_state_style}'></button>"
                "<button type='button' class='numeric-step-overlay numeric-decrement'"
                f" data-target='{html.escape(entry['widget_id'])}_value' data-step='-{step}'"
                f" data-frog-part='increment_down' data-frog-method='decrement' data-frog-button-state-law='normal-pressed' aria-label='Decrement {html.escape(label)}'"
                f" style='{svg_box_style(geometry['increment_down_x'], geometry['increment_down_y'], geometry['increment_down_width'], geometry['increment_down_height'], geometry)}{step_state_style}'></button>"
            )
    else:
        value_part = (
            "<output class='numeric-value-overlay numeric-indicator-value' data-frog-part='text_value' data-svg-anchor='text_value.center'"
            f" style='{value_style}'>{value}</output>"
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
        "<span class='numeric-label-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor'"
        f" style='{label_style}'>{html.escape(label)}</span>"
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
        self.panel = load_source_front_panel_from_contract(self.contract, self.contract_path)
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
            {"wfrog": self.package, "front_panel": self.panel},
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


class ButtonRuntimeCore:
    def __init__(self, *, contract_path: str | Path | None = None, wfrog_path: str | Path | None = None) -> None:
        self.contract_path = Path(contract_path or default_example10_contract_path()).resolve()
        self.wfrog_path = Path(wfrog_path or default_example10_wfrog_path()).resolve()
        self.contract = load_contract_json(self.contract_path)
        self.package = load_contract_json(self.wfrog_path)
        self.panel = load_source_front_panel_from_contract(self.contract, self.contract_path)
        self.unit = self._load_and_validate()
        self.asset_map = {
            item["asset_id"]: (self.wfrog_path.parent / Path(item["path"])).resolve()
            for item in self.package.get("svg_assets", [])
        }
        self.widgets = self._build_widgets()
        self.last_trigger_pressed = False
        self.last_result = False
        self.execute(False)

    def _load_and_validate(self) -> dict[str, Any]:
        if self.contract.get("backend_family") != "reference_host_runtime_ui_binding":
            raise RuntimeError("Unexpected backend family.")
        if self.contract.get("example_id") != "10_button_press_to_boolean":
            raise RuntimeError("Slice 10 expects Example 10.")

        runtime_family = self.contract.get("assumptions", {}).get("runtime_family", {})
        if runtime_family.get("name") != "reference_host_runtime_ui_binding":
            raise RuntimeError("Unexpected runtime-family assumption name.")
        if runtime_family.get("ui_binding", {}).get("widget_value_binding") is not True:
            raise RuntimeError("Contract must require widget_value_binding.")

        units = self.contract.get("units", [])
        if len(units) != 1:
            raise RuntimeError("Expected exactly one contract unit.")
        unit = units[0]
        if unit.get("unit_id") != "main":
            raise RuntimeError("Expected unit_id main.")
        if unit.get("kind") != "button_press_to_boolean_ui_unit":
            raise RuntimeError("Unexpected runtime unit kind.")

        public_io = unit.get("public_io", {})
        inputs = public_io.get("inputs", [])
        outputs = public_io.get("outputs", [])
        if len(inputs) != 1 or inputs[0].get("id") != "trigger_pressed" or inputs[0].get("type") != "bool":
            raise RuntimeError("Expected bool public input trigger_pressed.")
        if len(outputs) != 1 or outputs[0].get("id") != "pressed" or outputs[0].get("type") != "bool":
            raise RuntimeError("Expected bool public output pressed.")

        execution_kernel = unit.get("execution_kernel", {})
        if execution_kernel.get("operation") != "copy" or execution_kernel.get("src") != "trigger_pressed" or execution_kernel.get("dst") != "pressed":
            raise RuntimeError("Slice 10 expects trigger_pressed -> pressed copy execution.")
        if unit.get("effects", []) != []:
            raise RuntimeError("Slice 10 does not use property writes.")

        if self.panel.get("host_binding_ref") != "reference_host_default":
            raise RuntimeError("Expected host_binding_ref reference_host_default.")
        host_bindings = {entry.get("binding_id"): entry for entry in self.package.get("host_bindings", [])}
        required = set(host_bindings.get("reference_host_default", {}).get("required_capabilities", []))
        for capability in {
            "window",
            "basic_widget_rendering",
            "widget_value_binding",
            "button_press_binding",
            "svg_part_overlay_alignment",
        }:
            if capability not in required:
                raise RuntimeError(f"Missing host capability {capability}.")

        panel_widgets = {entry["instance_id"]: entry for entry in self.panel.get("widgets", [])}
        if "trigger_button" not in panel_widgets or "pressed_indicator" not in panel_widgets:
            raise RuntimeError("Slice 10 requires trigger_button and pressed_indicator.")
        for binding in unit.get("ui_bindings", {}).get("widgets", []):
            widget_id = binding.get("widget_id")
            panel_widget = panel_widgets.get(widget_id)
            if panel_widget is None:
                raise RuntimeError(f"Missing panel widget {widget_id}.")
            if panel_widget.get("class_ref") != binding.get("widget_class"):
                raise RuntimeError(f"Class mismatch for widget {widget_id}.")
            if binding.get("value_type") != "bool":
                raise RuntimeError("Slice 10 supports only bool widget values.")
            if binding.get("widget_class") not in {"frog.widgets.button", "frog.widgets.boolean_indicator"}:
                raise RuntimeError(f"Unsupported widget class {binding.get('widget_class')}.")
        return unit

    def _build_widgets(self) -> dict[str, dict[str, Any]]:
        bindings_by_widget = {
            binding["widget_id"]: binding
            for binding in self.unit.get("ui_bindings", {}).get("widgets", [])
        }
        result: dict[str, dict[str, Any]] = {}
        for panel_widget in self.panel.get("widgets", []):
            class_ref = panel_widget.get("class_ref")
            if class_ref not in {"frog.widgets.button", "frog.widgets.boolean_indicator"}:
                continue
            widget_id = panel_widget["instance_id"]
            binding = bindings_by_widget.get(widget_id)
            if binding is None:
                raise RuntimeError(f"Slice 10 widget {widget_id} must have a contract binding.")

            asset_ref = str(panel_widget.get("visual", {}).get("asset_ref", ""))
            if not asset_ref.startswith("asset:"):
                raise RuntimeError(f"Slice 10 widget {widget_id} must reference a .wfrog SVG asset.")
            asset_id = asset_ref.split(":", 1)[1]
            asset_path = self.asset_map.get(asset_id)
            if asset_path is None or not asset_path.exists():
                raise RuntimeError(f"Slice 10 widget {widget_id} asset path must exist.")

            props = dict(panel_widget.get("props", {}))
            is_button = class_ref == "frog.widgets.button"
            props.setdefault("value", False)
            props.setdefault("pressed", props.get("value", False))
            props.setdefault("label.text", widget_id)
            props.setdefault("caption.text", widget_id)
            props.setdefault("interaction.enabled", is_button)
            props.setdefault("interaction.read_only", not is_button)
            props.setdefault("realization.variant", "rectangular" if is_button else "circular")
            binding_data = binding.get("binding", {})
            if "public_input_id" in binding_data:
                props["binding.public_input_id"] = binding_data["public_input_id"]
            if "public_output_id" in binding_data:
                props["binding.public_output_id"] = binding_data["public_output_id"]

            result[widget_id] = {
                "widget_id": widget_id,
                "class_ref": class_ref,
                "role": binding.get("role", "control" if is_button else "indicator"),
                "layout": dict(panel_widget.get("layout", {})),
                "properties": props,
                "asset_id": asset_id,
                "asset_path": asset_path,
            }
        return result

    def set_control_pressed(self, value: bool) -> None:
        button = self.widgets["trigger_button"]
        self.last_trigger_pressed = bool(value)
        button["properties"]["pressed"] = bool(value)
        button["properties"]["value"] = bool(value)

    def control_pressed(self) -> bool:
        props = self.widgets["trigger_button"]["properties"]
        return bool(props.get("pressed", props.get("value", False)))

    def execute(self, pressed_override: bool | None = None) -> dict[str, Any]:
        if pressed_override is not None:
            self.set_control_pressed(bool(pressed_override))
        self.last_trigger_pressed = self.control_pressed()
        self.last_result = self.last_trigger_pressed
        self.widgets["pressed_indicator"]["properties"]["value"] = self.last_result
        self.widgets["trigger_button"]["properties"]["pressed"] = False
        self.widgets["trigger_button"]["properties"]["value"] = False
        return self.execution_artifact()

    def execute_with_native_kernel_bridge(
        self,
        bridge: NativeBoolKernelBridge,
        pressed_override: bool | None = None,
    ) -> dict[str, Any]:
        if bridge.manifest.source_lowered_unit != "Examples/10_button_press_to_boolean/main.lowering.json":
            raise RuntimeError("Unexpected native Button kernel source lowered unit.")
        if pressed_override is not None:
            self.set_control_pressed(bool(pressed_override))
        self.last_trigger_pressed = self.control_pressed()
        result = bridge.run(self.last_trigger_pressed)
        if not result.ok:
            raise RuntimeError(result.diagnostic or "native Button bool kernel execution failed.")
        self.last_result = result.result
        self.widgets["pressed_indicator"]["properties"]["value"] = self.last_result
        self.widgets["trigger_button"]["properties"]["pressed"] = False
        self.widgets["trigger_button"]["properties"]["value"] = False
        return self.execution_artifact()

    def _runtime_for(self, widget: dict[str, Any]) -> dict[str, Any]:
        props = widget["properties"]
        runtime: dict[str, Any] = {
            "value": bool(props.get("value", False)),
            "label.text": props.get("label.text", ""),
            "caption.text": props.get("caption.text", widget["widget_id"]),
            "asset_ref": f"asset:{widget['asset_id']}",
            "realization.variant": props.get("realization.variant", ""),
        }
        if widget["widget_id"] == "trigger_button":
            runtime["event.pressed"] = self.last_trigger_pressed
        for key in [
            "caption.visible",
            "caption.anchor.x",
            "caption.anchor.y",
            "caption.align.horizontal",
            "caption.style.text_color",
            "caption.style.font_family",
            "caption.style.font_size",
            "caption.style.font_weight",
            "label.visible",
            "state_text.false_text",
            "state_text.true_text",
            "state_text.visible",
            "state_text.anchor.x",
            "state_text.anchor.y",
            "state_text.style.text_color.false",
            "state_text.style.text_color.true",
            "state_text.style.font_size",
            "state_text.style.font_weight",
            "behavior.mechanical_action",
            "behavior.latch_reset_policy",
            "style.frame.fill_color",
            "style.frame.border_color",
            "style.frame.border_width",
            "style.frame.visible",
            "style.face.fill_color.false",
            "style.face.fill_color.true",
            "style.face.fill_color.hover_false",
            "style.face.fill_color.hover_true",
            "style.face.fill_color.pressed_false",
            "style.face.fill_color.pressed_true",
            "style.face.border_color.false",
            "style.face.border_color.true",
            "style.face.border_width",
            "style.state_face.fill_color.false",
            "style.state_face.fill_color.true",
            "style.state_face.fill_color.hover_false",
            "style.state_face.fill_color.hover_true",
            "style.state_face.fill_color.pressed_false",
            "style.state_face.fill_color.pressed_true",
            "style.state_face.border_color.false",
            "style.state_face.border_color.true",
            "style.state_face.border_color.hover_false",
            "style.state_face.border_color.hover_true",
            "style.state_face.border_color.pressed_false",
            "style.state_face.border_color.pressed_true",
            "style.state_face.border_width",
            "style.inner.left",
            "style.inner.top",
            "style.inner.width",
            "style.inner.height",
            "style.inner.fill_color.false",
            "style.inner.fill_color.true",
            "style.inner.border_color.false",
            "style.inner.border_color.true",
            "style.inner.border_width",
            "style.focus_ring.visible",
            "style.focus_ring.color",
            "style.focus_ring.width",
            "style.pressed.inset",
            "style.transition.duration_ms",
            "style.transition.timing",
            "binding.public_input_id",
            "binding.public_output_id",
            "interaction.enabled",
            "interaction.read_only",
        ]:
            if key in props:
                runtime[key] = props[key]
        return runtime

    def execution_artifact(self) -> dict[str, Any]:
        widgets = []
        ui_outputs: dict[str, bool] = {}
        for panel_widget in self.panel.get("widgets", []):
            widget = self.widgets.get(panel_widget["instance_id"])
            if widget is None:
                continue
            value = bool(widget["properties"].get("value", False))
            ui_outputs[widget["widget_id"]] = value
            widgets.append(
                {
                    "widget_id": widget["widget_id"],
                    "class_ref": widget["class_ref"],
                    "role": widget["role"],
                    "layout": widget["layout"],
                    "runtime": self._runtime_for(widget),
                }
            )

        return {
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {
                "unit_ids": [self.unit["unit_id"]],
                "backend_family": self.contract["backend_family"],
                "source_ref": self.contract["source_ref"],
            },
            "execution_summary": {
                "mode": "button_press_to_boolean",
                "executed_unit": self.unit["unit_id"],
                "operation": "copy",
                "trigger_pressed": self.last_trigger_pressed,
                "pressed": self.last_result,
            },
            "outputs": {
                "public": {"pressed": self.last_result},
                "ui": ui_outputs,
            },
            "ui_runtime": {
                "panel": {
                    "panel_id": self.panel["panel_id"],
                    "title": self.panel["title"],
                    "class_ref": self.panel["class_ref"],
                    "layout": self.panel["layout"],
                },
                "widgets": widgets,
            },
            "diagnostics": [],
        }


class StringRuntimeCore:
    def __init__(self, *, contract_path: str | Path | None = None, wfrog_path: str | Path | None = None) -> None:
        self.contract_path = Path(contract_path or default_example07_contract_path()).resolve()
        self.wfrog_path = Path(wfrog_path or default_example07_wfrog_path()).resolve()
        self.contract = load_contract_json(self.contract_path)
        self.package = load_contract_json(self.wfrog_path)
        self.panel = load_source_front_panel_from_contract(self.contract, self.contract_path)
        self.asset_map = {
            item["asset_id"]: (self.wfrog_path.parent / Path(item["path"])).resolve()
            for item in self.package.get("svg_assets", [])
        }
        self.widgets = {entry["instance_id"]: entry for entry in self.panel.get("widgets", [])}
        self.current_text = str(self.widgets.get("str_input", {}).get("props", {}).get("value", "hello world"))
        self.last_result = self.current_text

    def execute(self, control_value: str | None = None) -> dict[str, Any]:
        if control_value is not None:
            self.current_text = str(control_value)
        if len(self.current_text.encode("utf-8")) > 256:
            raise RuntimeError("input_text must remain within 256 UTF-8 bytes.")
        self.last_result = self.current_text
        return self.execution_artifact()

    def execute_with_native_kernel_bridge(
        self,
        bridge: NativeStringKernelBridge,
        control_value: str | None = None,
    ) -> dict[str, Any]:
        if bridge.manifest.source_lowered_unit != "Examples/07_string_value_roundtrip/main.lowering.json":
            raise RuntimeError("Unexpected native string kernel source lowered unit.")
        if control_value is not None:
            self.current_text = str(control_value)
        result = bridge.run(self.current_text)
        if not result.ok:
            raise RuntimeError(result.diagnostic or "native string kernel execution failed.")
        self.last_result = result.result
        return self.execution_artifact()

    def _runtime_for(self, widget_id: str, value: str) -> dict[str, Any]:
        widget = self.widgets[widget_id]
        props = dict(widget.get("props", {}))
        visual = dict(widget.get("visual", {}))
        runtime = {
            "value": value,
            "label.text": props.get("label.text", ""),
            "caption.text": props.get("caption.text", widget_id),
            "asset_ref": visual.get("asset_ref"),
            "realization.variant": props.get("realization.variant", "rectangular"),
        }
        for key in [
            "caption.visible",
            "caption.anchor.x",
            "caption.anchor.y",
            "caption.align.horizontal",
            "caption.style.text_color",
            "caption.style.font_family",
            "caption.style.font_size",
            "caption.style.font_weight",
            "style.frame.fill_color",
            "style.frame.border_color",
            "style.frame.border_width",
            "style.text_region.fill_color",
            "style.text_region.fill_color.hover",
            "style.text_region.border_color",
            "style.text_region.border_color.hover",
            "style.text_region.border_width",
            "style.text_region.border_width.hover",
            "style.text.color",
            "style.text.font_size",
            "style.text.font_weight",
            "placeholder.text",
            "placeholder.visible",
            "interaction.enabled",
            "interaction.read_only",
        ]:
            if key in props:
                runtime[key] = props[key]
        return runtime

    def execution_artifact(self) -> dict[str, Any]:
        return {
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {
                "unit_ids": ["main"],
                "backend_family": self.contract["backend_family"],
                "source_ref": dict(self.contract["source_ref"]),
            },
            "execution_summary": {
                "mode": "string_value_roundtrip",
                "executed_unit": "main",
                "operation": "copy",
                "input_text": self.current_text,
                "result_text": self.last_result,
            },
            "outputs": {
                "public": {"result_text": self.last_result},
                "ui": {"str_input": self.current_text, "str_result": self.last_result},
            },
            "ui_runtime": {
                "panel": {
                    "panel_id": self.panel["panel_id"],
                    "title": self.panel["title"],
                    "class_ref": self.panel["class_ref"],
                    "layout": dict(self.panel["layout"]),
                },
                "widgets": [
                    {
                        "widget_id": "str_input",
                        "class_ref": self.widgets["str_input"]["class_ref"],
                        "role": "control",
                        "layout": dict(self.widgets["str_input"]["layout"]),
                        "runtime": self._runtime_for("str_input", self.current_text),
                    },
                    {
                        "widget_id": "str_result",
                        "class_ref": self.widgets["str_result"]["class_ref"],
                        "role": "indicator",
                        "layout": dict(self.widgets["str_result"]["layout"]),
                        "runtime": self._runtime_for("str_result", self.last_result),
                    },
                ],
            },
            "diagnostics": [],
        }


def enum_items_from_props(props: dict[str, Any], widget_id: str) -> list[dict[str, Any]]:
    items = props.get("items")
    if not isinstance(items, list) or not items:
        raise RuntimeError(f"Enum widget {widget_id} must define items in front-panel instance properties.")
    result: list[dict[str, Any]] = []
    seen_ids: set[str] = set()
    seen_numbers: set[int] = set()
    for item in items:
        if not isinstance(item, dict):
            raise RuntimeError("Enum item must be an object.")
        item_id = str(item.get("id", ""))
        text = str(item.get("text", ""))
        numeric_value = item.get("numeric_value")
        if not item_id or not text or not isinstance(numeric_value, int):
            raise RuntimeError("Enum item must publish id, text, and numeric_value.")
        if item_id in seen_ids:
            raise RuntimeError(f"Duplicate enum item id: {item_id}")
        if numeric_value in seen_numbers:
            raise RuntimeError("Duplicate enum item numeric_value.")
        if numeric_value < 0 or numeric_value > 65535:
            raise RuntimeError("Enum item numeric_value must remain in the u16 domain.")
        seen_ids.add(item_id)
        seen_numbers.add(numeric_value)
        result.append({
            "id": item_id,
            "text": text,
            "numeric_value": numeric_value,
            "enabled": bool(item.get("enabled", True)),
        })
    return result


def enum_item_by_id(items: list[dict[str, Any]], item_id: str, label: str) -> dict[str, Any]:
    for item in items:
        if item["id"] == item_id:
            return item
    raise RuntimeError(f"{label} must resolve to a declared enum item.")


def enum_item_by_numeric_value(items: list[dict[str, Any]], numeric_value: int, label: str) -> dict[str, Any]:
    for item in items:
        if item["numeric_value"] == numeric_value:
            return item
    raise RuntimeError(f"{label} must resolve to a declared enum item.")


class EnumRuntimeCore:
    def __init__(self, *, contract_path: str | Path | None = None, wfrog_path: str | Path | None = None) -> None:
        self.contract_path = Path(contract_path or default_example08_contract_path()).resolve()
        self.wfrog_path = Path(wfrog_path or default_example08_wfrog_path()).resolve()
        self.contract = load_contract_json(self.contract_path)
        self.package = load_contract_json(self.wfrog_path)
        self.panel = load_source_front_panel_from_contract(self.contract, self.contract_path)
        self.asset_map = {
            item["asset_id"]: (self.wfrog_path.parent / Path(item["path"])).resolve()
            for item in self.package.get("svg_assets", [])
        }
        self.widgets = {entry["instance_id"]: entry for entry in self.panel.get("widgets", [])}
        if self.contract.get("source_ref", {}).get("example_id") != "08_enum_value_roundtrip":
            raise RuntimeError("EnumRuntimeCore expects Example 08.")
        if self.contract.get("units", [{}])[0].get("kind") != "enum_value_roundtrip_ui_unit":
            raise RuntimeError("EnumRuntimeCore expects enum_value_roundtrip_ui_unit.")
        input_items = enum_items_from_props(self.widgets["mode_input"].get("props", {}), "mode_input")
        result_items = enum_items_from_props(self.widgets["mode_result"].get("props", {}), "mode_result")
        if [(item["id"], item["text"], item["numeric_value"]) for item in input_items] != [
            (item["id"], item["text"], item["numeric_value"]) for item in result_items
        ]:
            raise RuntimeError("Enum input and result widgets must publish the same vocabulary.")
        self.current_mode = str(self.widgets["mode_input"].get("props", {}).get("value", input_items[0]["id"]))
        enum_item_by_id(input_items, self.current_mode, "mode_value")
        self.last_result = self.current_mode

    def execute(self, control_value: str | None = None) -> dict[str, Any]:
        if control_value is not None:
            self.current_mode = str(control_value)
        item = enum_item_by_id(enum_items_from_props(self.widgets["mode_input"].get("props", {}), "mode_input"), self.current_mode, "mode_value")
        if not item["enabled"]:
            raise RuntimeError("mode_value must resolve to an enabled enum item.")
        self.last_result = item["id"]
        return self.execution_artifact()

    def execute_with_native_kernel_bridge(
        self,
        bridge: NativeEnumKernelBridge,
        control_value: str | None = None,
    ) -> dict[str, Any]:
        if bridge.manifest.source_lowered_unit != "Examples/08_enum_value_roundtrip/main.lowering.json":
            raise RuntimeError("Unexpected native enum kernel source lowered unit.")
        if control_value is not None:
            self.current_mode = str(control_value)
        input_items = enum_items_from_props(self.widgets["mode_input"].get("props", {}), "mode_input")
        input_item = enum_item_by_id(input_items, self.current_mode, "mode_value")
        if not input_item["enabled"]:
            raise RuntimeError("mode_value must resolve to an enabled enum item.")
        result = bridge.run(int(input_item["numeric_value"]))
        if not result.ok:
            raise RuntimeError(result.diagnostic or "native enum kernel execution failed.")
        output_items = enum_items_from_props(self.widgets["mode_result"].get("props", {}), "mode_result")
        output_item = enum_item_by_numeric_value(output_items, result.result_numeric_value, "result_mode")
        self.last_result = output_item["id"]
        return self.execution_artifact()

    def _runtime_for(self, widget_id: str, value: str) -> dict[str, Any]:
        widget = self.widgets[widget_id]
        props = dict(widget.get("props", {}))
        visual = dict(widget.get("visual", {}))
        selected = enum_item_by_id(enum_items_from_props(props, widget_id), value, f"{widget_id}.value")
        runtime: dict[str, Any] = {
            "value": selected["id"],
            "selected.text": selected["text"],
            "selected.numeric_value": selected["numeric_value"],
            "label.text": props.get("label.text", ""),
            "caption.text": props.get("caption.text", widget_id),
            "items": enum_items_from_props(props, widget_id),
            "asset_ref": visual.get("asset_ref"),
            "realization.variant": props.get("realization.variant", "rectangular_ring"),
        }
        for key in [
            "enum.domain_id",
            "caption.visible",
            "caption.anchor.x",
            "caption.anchor.y",
            "caption.align.horizontal",
            "caption.style.text_color",
            "caption.style.font_family",
            "caption.style.font_size",
            "caption.style.font_weight",
            "display.digital_display_visible",
            "display.increment_buttons_visible",
            "display.selector_visible",
            "display.text_overflow_visible",
            "style.scale.reference_width",
            "style.scale.reference_height",
            "style.frame.fill_color",
            "style.frame.border_color",
            "style.frame.border_width",
            "style.value_face.fill_color",
            "style.value_face.fill_color.hover",
            "style.value_face.border_color",
            "style.value_face.border_color.hover",
            "style.value_face.border_width",
            "style.value_display.color",
            "style.value_display.font_size",
            "style.value_display.font_size_mode",
            "style.value_display.font_weight",
            "style.value_display.vertical_offset",
            "style.value_display.vertical_offset_mode",
            "style.value_display.padding_inline",
            "style.value_display.padding_inline_mode",
            "style.selector_face.fill_color",
            "style.selector_face.fill_color.hover",
            "style.selector_face.border_color",
            "style.selector_face.border_color.hover",
            "style.selector_face.border_width",
            "style.selector_face.border_width_mode",
            "style.selector_face.border_radius",
            "style.selector_face.border_radius_mode",
            "style.selector_face.symbol_color",
            "style.selector_face.symbol_color.hover",
            "style.selector_face.symbol_width",
            "style.selector_face.symbol_height",
            "style.selector_face.symbol_size_mode",
            "style.dropdown.fill_color",
            "style.dropdown.border_color",
            "style.dropdown.border_width",
            "style.dropdown.border_width_mode",
            "style.dropdown.option.fill_color",
            "style.dropdown.option.text_color",
            "style.dropdown.option.hover_fill_color",
            "style.dropdown.option.hover_text_color",
            "style.dropdown.option.selected_fill_color",
            "style.dropdown.option.selected_text_color",
            "style.dropdown.option.font_family",
            "style.dropdown.option.font_size",
            "style.dropdown.option.font_size_mode",
            "style.dropdown.option.font_weight",
            "style.dropdown.option.font_style",
            "style.dropdown.option.padding_inline",
            "style.dropdown.option.padding_inline_mode",
            "style.dropdown.option.height",
            "style.dropdown.option.height_mode",
            "interaction.enabled",
            "interaction.read_only",
            "visible",
        ]:
            if key in props:
                runtime[key] = props[key]
        return runtime

    def execution_artifact(self) -> dict[str, Any]:
        input_item = enum_item_by_id(enum_items_from_props(self.widgets["mode_input"].get("props", {}), "mode_input"), self.current_mode, "mode_value")
        output_item = enum_item_by_id(enum_items_from_props(self.widgets["mode_result"].get("props", {}), "mode_result"), self.last_result, "result_mode")
        return {
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {
                "unit_ids": ["main"],
                "backend_family": self.contract["backend_family"],
                "source_ref": dict(self.contract["source_ref"]),
            },
            "execution_summary": {
                "mode": "enum_value_roundtrip",
                "executed_unit": "main",
                "operation": "copy",
                "input_mode": input_item["id"],
                "input_text": input_item["text"],
                "input_numeric_value": input_item["numeric_value"],
                "result_mode": output_item["id"],
                "result_text": output_item["text"],
                "result_numeric_value": output_item["numeric_value"],
            },
            "outputs": {
                "public": {"result_mode": output_item["id"]},
                "ui": {"mode_input": input_item["id"], "mode_result": output_item["id"]},
            },
            "ui_runtime": {
                "panel": {
                    "panel_id": self.panel["panel_id"],
                    "title": self.panel["title"],
                    "class_ref": self.panel["class_ref"],
                    "layout": dict(self.panel["layout"]),
                },
                "widgets": [
                    {
                        "widget_id": "mode_input",
                        "class_ref": self.widgets["mode_input"]["class_ref"],
                        "role": "control",
                        "layout": dict(self.widgets["mode_input"]["layout"]),
                        "runtime": self._runtime_for("mode_input", input_item["id"]),
                    },
                    {
                        "widget_id": "mode_result",
                        "class_ref": self.widgets["mode_result"]["class_ref"],
                        "role": "indicator",
                        "layout": dict(self.widgets["mode_result"]["layout"]),
                        "runtime": self._runtime_for("mode_result", output_item["id"]),
                    },
                ],
            },
            "diagnostics": [],
        }


def load_string_svg_geometry(asset_path: Path | None) -> dict[str, float]:
    geometry = {
        "view_width": 420.0,
        "view_height": 190.0,
        "caption_x": 16.0,
        "caption_y": 46.0,
        "text_region_x": 28.0,
        "text_region_y": 88.0,
        "text_region_width": 364.0,
        "text_region_height": 56.0,
    }
    if asset_path is None or not asset_path.exists():
        return geometry
    svg = asset_path.read_text(encoding="utf-8")
    viewbox = re.search(r"\bviewBox=[\"']([^\"']+)[\"']", svg)
    if viewbox:
        parts = viewbox.group(1).replace(",", " ").split()
        if len(parts) == 4:
            try:
                geometry["view_width"] = float(parts[2])
                geometry["view_height"] = float(parts[3])
            except ValueError:
                pass
    geometry["caption_x"] = svg_attribute_float(svg, "caption_text", "x", geometry["caption_x"])
    geometry["caption_y"] = svg_attribute_float(svg, "caption_text", "y", geometry["caption_y"])
    geometry["text_region_x"] = svg_attribute_float(svg, "text_region", "x", geometry["text_region_x"])
    geometry["text_region_y"] = svg_attribute_float(svg, "text_region", "y", geometry["text_region_y"])
    geometry["text_region_width"] = svg_attribute_float(svg, "text_region", "width", geometry["text_region_width"])
    geometry["text_region_height"] = svg_attribute_float(svg, "text_region", "height", geometry["text_region_height"])
    return geometry


def render_string_skin(asset_path: Path | None, runtime: dict[str, Any]) -> str:
    if asset_path is None or not asset_path.exists():
        return "<div class='string-skin missing-skin'></div>"
    style = (
        "--frog-string-label-display:none;"
        "--frog-string-caption-display:none;"
        "--frog-string-placeholder-display:none;"
        f"--frog-string-frame-fill:{html.escape(safe_css_color(runtime.get('style.frame.fill_color'), 'transparent'))};"
        f"--frog-string-frame-stroke:{html.escape(safe_css_color(runtime.get('style.frame.border_color'), 'transparent'))};"
        f"--frog-string-frame-stroke-width:{html.escape(safe_css_length(runtime.get('style.frame.border_width'), '0px'))};"
        f"--frog-string-text-region-fill:{html.escape(safe_css_color(runtime.get('style.text_region.fill_color'), '#ffffff'))};"
        f"--frog-string-text-region-stroke:{html.escape(safe_css_color(runtime.get('style.text_region.border_color'), '#64748b'))};"
        f"--frog-string-text-region-stroke-width:{html.escape(safe_css_length(runtime.get('style.text_region.border_width'), '2px'))};"
        f"--frog-string-text-region-fill-hover:{html.escape(safe_css_color(runtime.get('style.text_region.fill_color.hover'), safe_css_color(runtime.get('style.text_region.fill_color'), '#ffffff')))};"
        f"--frog-string-text-region-stroke-hover:{html.escape(safe_css_color(runtime.get('style.text_region.border_color.hover'), safe_css_color(runtime.get('style.text_region.border_color'), '#64748b')))};"
        f"--frog-string-text-region-stroke-width-hover:{html.escape(safe_css_length(runtime.get('style.text_region.border_width.hover'), safe_css_length(runtime.get('style.text_region.border_width'), '2px')))};"
        f"--frog-string-text-fill:{html.escape(safe_css_color(runtime.get('style.text.color'), '#111827'))};"
        f"--frog-string-text-font-size:{html.escape(safe_css_length(runtime.get('style.text.font_size'), '16px'))};"
        f"--frog-string-text-font-weight:{html.escape(safe_css_font_weight(runtime.get('style.text.font_weight'), '400'))};"
    )
    return f"<div class='string-skin' aria-hidden='true' style='{style}'>{asset_path.read_text(encoding='utf-8')}</div>"


def render_string_widget(widget: dict[str, Any], asset_path: Path | None) -> str:
    layout = widget["layout"]
    runtime = widget["runtime"]
    is_control = widget["role"] == "control"
    geometry = load_string_svg_geometry(asset_path)
    label_style = caption_anchor_style(runtime, geometry)
    value_style = svg_box_style(
        geometry["text_region_x"],
        geometry["text_region_y"],
        geometry["text_region_width"],
        geometry["text_region_height"],
        geometry,
    )
    style = (
        f"position:absolute;left:{layout['x']}px;top:{layout['y']}px;"
        f"width:{layout['width']}px;height:{layout['height']}px;"
        f"--frog-string-caption-color:{html.escape(safe_css_color(runtime.get('caption.style.text_color'), '#111827'))};"
        f"--frog-string-caption-font-size:{html.escape(safe_css_length(runtime.get('caption.style.font_size'), '14px'))};"
        f"--frog-string-caption-font-weight:{html.escape(safe_css_font_weight(runtime.get('caption.style.font_weight'), '600'))};"
        f"--frog-string-caption-font-family:{html.escape(safe_css_font_family(runtime.get('caption.style.font_family'), 'system-ui, Segoe UI, Arial, sans-serif'))};"
    )
    attrs = (
        f" data-widget-id='{html.escape(widget['widget_id'])}'"
        f" data-class-ref='{html.escape(widget['class_ref'])}'"
        f" data-role='{html.escape(widget['role'])}'"
        " data-frog-visual-law='wfrog-realization-state-map'"
        f" data-asset-route='{html.escape(asset_url(runtime.get('asset_ref')))}'"
    )
    caption = html.escape(runtime_string(runtime, "caption.text", widget["widget_id"]))
    text_value = html.escape(runtime_string(runtime, "value", ""))
    text_color = html.escape(safe_css_color(runtime.get("style.text.color"), "#111827"))
    text_size = html.escape(safe_css_length(runtime.get("style.text.font_size"), "16px"))
    text_weight = html.escape(safe_css_font_weight(runtime.get("style.text.font_weight"), "400"))
    skin = render_string_skin(asset_path, runtime)
    caption_overlay = (
        "<span class='string-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor'"
        f" style='{label_style}'>{caption}</span>"
    )
    if is_control:
        value_overlay = (
            f"<input id='{html.escape(widget['widget_id'])}_value' name='input_text' type='text'"
            " class='string-value-overlay string-control-editor' data-frog-part='text_value' data-svg-anchor='text_region.left_center'"
            f" style='{value_style}color:{text_color};font-size:{text_size};font-weight:{text_weight};'"
            f" value='{text_value}'>"
        )
    else:
        value_overlay = (
            "<output class='string-value-overlay string-indicator-value' data-frog-part='text_value' data-svg-anchor='text_region.left_center'"
            f" style='{value_style}color:{text_color};font-size:{text_size};font-weight:{text_weight};'>{text_value}</output>"
        )
    return f"<section class='frog-widget string-widget {'string-control' if is_control else 'string-indicator'}'{attrs} style='{style}'>{skin}{caption_overlay}{value_overlay}</section>"


def load_enum_svg_geometry(asset_path: Path | None) -> dict[str, float]:
    geometry = {
        "view_width": 380.0,
        "view_height": 150.0,
        "caption_x": 16.0,
        "caption_y": 46.0,
        "value_face_x": 22.0,
        "value_face_y": 82.0,
        "value_face_width": 214.0,
        "value_face_height": 28.0,
        "selector_face_x": 246.0,
        "selector_face_y": 82.0,
        "selector_face_width": 24.0,
        "selector_face_height": 28.0,
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
    geometry["caption_x"] = svg_attribute_float(svg, "caption_text", "x", geometry["caption_x"])
    geometry["caption_y"] = svg_attribute_float(svg, "caption_text", "y", geometry["caption_y"])
    geometry["value_face_x"] = svg_attribute_float(svg, "value_face", "x", geometry["value_face_x"])
    geometry["value_face_y"] = svg_attribute_float(svg, "value_face", "y", geometry["value_face_y"])
    geometry["value_face_width"] = svg_attribute_float(svg, "value_face", "width", geometry["value_face_width"])
    geometry["value_face_height"] = svg_attribute_float(svg, "value_face", "height", geometry["value_face_height"])
    geometry["selector_face_x"] = svg_attribute_float(svg, "selector_face", "x", geometry["selector_face_x"])
    geometry["selector_face_y"] = svg_attribute_float(svg, "selector_face", "y", geometry["selector_face_y"])
    geometry["selector_face_width"] = svg_attribute_float(svg, "selector_face", "width", geometry["selector_face_width"])
    geometry["selector_face_height"] = svg_attribute_float(svg, "selector_face", "height", geometry["selector_face_height"])
    return geometry


def svg_dropdown_style(x: float, y: float, width: float, height: float, geometry: dict[str, float]) -> str:
    return (
        f"left:{css_percent(pct(x, geometry['view_width']))};"
        f"top:{css_percent(pct(y + height, geometry['view_height']))};"
        f"width:{css_percent(pct(width, geometry['view_width']))};"
    )


def render_enum_skin(asset_path: Path | None, runtime: dict[str, Any]) -> str:
    if asset_path is None or not asset_path.exists():
        return "<div class='enum-skin missing-skin'></div>"
    frame_stroke = safe_css_color(runtime.get("style.frame.border_color"), "transparent")
    frame_stroke_width = safe_css_length(runtime.get("style.frame.border_width"), "0px")
    frame_visible = frame_stroke != "transparent" and frame_stroke_width not in {"0", "0px"}
    selector_visible = runtime_bool(runtime, "display.selector_visible", True)
    style = (
        "--frog-enum-label-display:none;"
        "--frog-enum-caption-display:none;"
        "--frog-enum-value-display:none;"
        f"--frog-enum-frame-display:{'inline' if frame_visible else 'none'};"
        f"--frog-enum-frame-fill:{html.escape(safe_css_color(runtime.get('style.frame.fill_color'), 'transparent'))};"
        f"--frog-enum-frame-stroke:{html.escape(frame_stroke)};"
        f"--frog-enum-frame-stroke-width:{html.escape(frame_stroke_width)};"
        f"--frog-enum-value-face-fill:{html.escape(safe_css_color(runtime.get('style.value_face.fill_color'), '#ffffff'))};"
        f"--frog-enum-value-face-stroke:{html.escape(safe_css_color(runtime.get('style.value_face.border_color'), '#64748b'))};"
        f"--frog-enum-value-face-stroke-width:{html.escape(safe_css_length(runtime.get('style.value_face.border_width'), '2px'))};"
        f"--frog-enum-selector-display:{'inline' if selector_visible else 'none'};"
        f"--frog-enum-selector-fill:{html.escape(safe_css_color(runtime.get('style.selector_face.fill_color'), '#f1f5f9'))};"
        f"--frog-enum-selector-stroke:{html.escape(safe_css_color(runtime.get('style.selector_face.border_color'), '#64748b'))};"
        f"--frog-enum-selector-stroke-width:{html.escape(safe_css_length(runtime.get('style.selector_face.border_width'), '1px'))};"
        f"--frog-enum-selector-symbol:{html.escape(safe_css_color(runtime.get('style.selector_face.symbol_color'), '#111827'))};"
        f"--frog-enum-increment-display:{'inline' if runtime_bool(runtime, 'display.increment_buttons_visible', False) else 'none'};"
        f"--frog-enum-digital-display:{'inline' if runtime_bool(runtime, 'display.digital_display_visible', False) else 'none'};"
        f"--frog-enum-overflow-display:{'inline' if runtime_bool(runtime, 'display.text_overflow_visible', False) else 'none'};"
    )
    return f"<div class='enum-skin' aria-hidden='true' style='{style}'>{asset_path.read_text(encoding='utf-8')}</div>"


def render_enum_widget(widget: dict[str, Any], asset_path: Path | None) -> str:
    layout = widget["layout"]
    runtime = widget["runtime"]
    is_control = widget["role"] == "control"
    geometry = load_enum_svg_geometry(asset_path)
    items = runtime.get("items", [])
    selected_id = runtime_string(runtime, "value", "")
    selected = enum_item_by_id(items, selected_id, f"{widget['widget_id']}.value")
    selector_visible = runtime_bool(runtime, "display.selector_visible", is_control)

    value_style = svg_box_style(
        geometry["value_face_x"],
        geometry["value_face_y"],
        geometry["value_face_width"],
        geometry["value_face_height"],
        geometry,
    )
    selector_style = svg_box_style(
        geometry["selector_face_x"],
        geometry["selector_face_y"],
        geometry["selector_face_width"],
        geometry["selector_face_height"],
        geometry,
    )
    dropdown_style = svg_dropdown_style(
        geometry["value_face_x"],
        geometry["value_face_y"],
        geometry["value_face_width"],
        geometry["value_face_height"],
        geometry,
    )
    text_color = html.escape(safe_css_color(runtime.get("style.value_display.color"), "#111827"))
    text_size = html.escape(safe_css_length(runtime.get("style.value_display.font_size"), "16px"))
    text_weight = html.escape(safe_css_font_weight(runtime.get("style.value_display.font_weight"), "400"))
    text_offset = html.escape(safe_css_length(runtime.get("style.value_display.vertical_offset"), "0px"))
    text_padding = html.escape(safe_css_length(runtime.get("style.value_display.padding_inline"), "8px"))
    selector_fill = safe_css_color(runtime.get("style.selector_face.fill_color"), "#f1f5f9")
    selector_stroke = safe_css_color(runtime.get("style.selector_face.border_color"), "#64748b")
    selector_symbol = safe_css_color(runtime.get("style.selector_face.symbol_color"), "#111827")
    dropdown_option_font_size = safe_css_length(runtime.get("style.dropdown.option.font_size"), text_size)
    attrs = (
        f" data-widget-id='{html.escape(widget['widget_id'])}'"
        f" data-class-ref='{html.escape(widget['class_ref'])}'"
        f" data-role='{html.escape(widget['role'])}'"
        " data-frog-visual-law='wfrog-realization-state-map'"
        f" data-frog-selector-visible='{'true' if selector_visible else 'false'}'"
        f" data-asset-route='{html.escape(asset_url(runtime.get('asset_ref')))}'"
    )
    style = (
        f"position:absolute;left:{layout['x']}px;top:{layout['y']}px;width:{layout['width']}px;height:{layout['height']}px;"
        f"--frog-enum-caption-color:{html.escape(safe_css_color(runtime.get('caption.style.text_color'), '#111827'))};"
        f"--frog-enum-caption-font-size:{html.escape(safe_css_length(runtime.get('caption.style.font_size'), '14px'))};"
        f"--frog-enum-caption-font-weight:{html.escape(safe_css_font_weight(runtime.get('caption.style.font_weight'), '600'))};"
        f"--frog-enum-caption-font-family:{html.escape(safe_css_font_family(runtime.get('caption.style.font_family'), 'system-ui, Segoe UI, Arial, sans-serif'))};"
        f"--frog-enum-selector-fill:{html.escape(selector_fill)};"
        f"--frog-enum-selector-stroke:{html.escape(selector_stroke)};"
        f"--frog-enum-selector-stroke-width:{html.escape(safe_css_length(runtime.get('style.selector_face.border_width'), '1px'))};"
        f"--frog-enum-selector-radius:{html.escape(safe_css_length(runtime.get('style.selector_face.border_radius'), '1px'))};"
        f"--frog-enum-selector-symbol:{html.escape(selector_symbol)};"
        f"--frog-enum-selector-symbol-width:{html.escape(safe_css_length(runtime.get('style.selector_face.symbol_width'), '10px'))};"
        f"--frog-enum-selector-symbol-height:{html.escape(safe_css_length(runtime.get('style.selector_face.symbol_height'), '7px'))};"
        f"--frog-enum-selector-hover-fill:{html.escape(safe_css_color(runtime.get('style.selector_face.fill_color.hover'), selector_fill))};"
        f"--frog-enum-selector-hover-stroke:{html.escape(safe_css_color(runtime.get('style.selector_face.border_color.hover'), selector_stroke))};"
        f"--frog-enum-selector-hover-symbol:{html.escape(safe_css_color(runtime.get('style.selector_face.symbol_color.hover'), selector_symbol))};"
        f"--frog-enum-value-hover-fill:{html.escape(safe_css_color(runtime.get('style.value_face.fill_color.hover'), 'transparent'))};"
        f"--frog-enum-text-padding-inline:{text_padding};"
        f"--frog-enum-dropdown-fill:{html.escape(safe_css_color(runtime.get('style.dropdown.fill_color'), '#ffffff'))};"
        f"--frog-enum-dropdown-border:{html.escape(safe_css_color(runtime.get('style.dropdown.border_color'), '#64748b'))};"
        f"--frog-enum-dropdown-border-width:{html.escape(safe_css_length(runtime.get('style.dropdown.border_width'), '1px'))};"
        f"--frog-enum-dropdown-option-fill:{html.escape(safe_css_color(runtime.get('style.dropdown.option.fill_color'), '#ffffff'))};"
        f"--frog-enum-dropdown-option-text:{html.escape(safe_css_color(runtime.get('style.dropdown.option.text_color'), '#111827'))};"
        f"--frog-enum-dropdown-option-hover-fill:{html.escape(safe_css_color(runtime.get('style.dropdown.option.hover_fill_color'), '#2563eb'))};"
        f"--frog-enum-dropdown-option-hover-text:{html.escape(safe_css_color(runtime.get('style.dropdown.option.hover_text_color'), '#ffffff'))};"
        f"--frog-enum-dropdown-option-selected-fill:{html.escape(safe_css_color(runtime.get('style.dropdown.option.selected_fill_color'), '#2563eb'))};"
        f"--frog-enum-dropdown-option-selected-text:{html.escape(safe_css_color(runtime.get('style.dropdown.option.selected_text_color'), '#ffffff'))};"
        f"--frog-enum-dropdown-option-font-family:{html.escape(safe_css_font_family(runtime.get('style.dropdown.option.font_family'), 'Segoe UI,Arial,sans-serif'))};"
        f"--frog-enum-dropdown-option-font-size:{html.escape(dropdown_option_font_size)};"
        f"--frog-enum-dropdown-option-font-weight:{html.escape(safe_css_font_weight(runtime.get('style.dropdown.option.font_weight'), '400'))};"
        f"--frog-enum-dropdown-option-font-style:{html.escape(safe_css_font_style(runtime.get('style.dropdown.option.font_style'), 'normal'))};"
        f"--frog-enum-dropdown-option-padding-inline:{html.escape(safe_css_length(runtime.get('style.dropdown.option.padding_inline'), text_padding))};"
        f"--frog-enum-dropdown-option-height:{html.escape(safe_css_length(runtime.get('style.dropdown.option.height'), '28px'))};"
    )
    caption = html.escape(runtime_string(runtime, "caption.text", widget["widget_id"]))
    caption_overlay = (
        "<span class='enum-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor'"
        f" style='{caption_anchor_style(runtime, geometry)}'>{caption}</span>"
    )
    skin = render_enum_skin(asset_path, runtime)

    if is_control:
        value_id = f"{widget['widget_id']}_value"
        display_id = f"{widget['widget_id']}_display"
        dropdown_id = f"{widget['widget_id']}_dropdown"
        options = []
        dropdown_options = []
        for item in items:
            is_selected = item["id"] == selected["id"]
            disabled = " disabled" if not item.get("enabled", True) else ""
            options.append(
                f"<option value='{html.escape(item['id'])}'{' selected' if is_selected else ''}{disabled}>{html.escape(item['text'])}</option>"
            )
            dropdown_options.append(
                "<button type='button' class='enum-dropdown-option' role='option'"
                f" data-enum-value='{html.escape(item['id'])}' aria-selected='{'true' if is_selected else 'false'}'{disabled}"
                f" onclick=\"frogSelectEnumOption(this,'{value_id}','{display_id}','{dropdown_id}')\">{html.escape(item['text'])}</button>"
            )
        value_overlay = (
            f"<button id='{display_id}' type='button' class='enum-value-display-overlay enum-display-button'"
            " data-frog-part='value_display' data-svg-anchor='value_display.left_center'"
            f" aria-haspopup='listbox' aria-expanded='false' aria-controls='{dropdown_id}'"
            f" onclick=\"frogToggleEnumDropdown('{dropdown_id}','{display_id}')\""
            f" style='{value_style}color:{text_color};font-size:{text_size};font-weight:{text_weight};--frog-enum-text-vertical-offset:{text_offset};'>{html.escape(selected['text'])}</button>"
        )
        selector_overlay = ""
        if selector_visible:
            selector_overlay = (
                "<button type='button' class='enum-selector-overlay enum-selector-button' data-frog-part='selector_face'"
                f" aria-label='Open {caption}' aria-haspopup='listbox' aria-expanded='false' aria-controls='{dropdown_id}'"
                f" onclick=\"frogToggleEnumDropdown('{dropdown_id}','{display_id}')\" style='{selector_style}'></button>"
            )
        hidden_select = (
            f"<select id='{value_id}' name='mode_value' class='enum-select-state' data-frog-part='value_state'"
            f" aria-hidden='true' tabindex='-1' onchange=\"frogUpdateEnumDisplay(this,'{display_id}')\""
            f" oninput=\"frogUpdateEnumDisplay(this,'{display_id}')\" hidden>{''.join(options)}</select>"
        )
        dropdown = f"<div id='{dropdown_id}' class='enum-dropdown' data-frog-part='dropdown' role='listbox' aria-label='{caption} options' hidden style='{dropdown_style}'>{''.join(dropdown_options)}</div>"
        body = f"{skin}{caption_overlay}{value_overlay}{selector_overlay}{hidden_select}{dropdown}"
    else:
        body = (
            f"{skin}{caption_overlay}<output class='enum-value-overlay enum-indicator-value'"
            " data-frog-part='value_display' data-svg-anchor='value_display.left_center'"
            f" style='{value_style}color:{text_color};font-size:{text_size};font-weight:{text_weight};--frog-enum-text-vertical-offset:{text_offset};'>{html.escape(selected['text'])}</output>"
        )
    return f"<section class='frog-widget enum-widget {'enum-control' if is_control else 'enum-indicator'}'{attrs} style='{style}'>{body}</section>"


def render_boolean_widget(widget: dict[str, Any]) -> str:
    layout = widget["layout"]
    runtime = widget["runtime"]
    geometry = {"view_width": 160.0, "view_height": 80.0, "caption_x": 8.0, "caption_y": 15.0}
    is_control = widget["role"] == "control"
    value = bool(runtime["value"])
    visual_state = "true" if value else "false"
    hover_state = "hover_true" if value else "hover_false"
    pressed_state = "pressed_true" if value else "pressed_false"
    transition_state = "transition_true_to_false" if value else "transition_false_to_true"
    variant = runtime_string(runtime, "realization.variant", "circular" if widget["class_ref"].endswith("indicator") else "rectangular")
    next_value = "false" if value else "true"
    state_text_visible = runtime_bool(runtime, "state_text.visible", True)
    frame_visible = runtime_bool(runtime, "style.frame.visible", True)
    focus_visible = runtime_bool(runtime, "style.focus_ring.visible", False)

    false_fill = state_property(runtime, "style.inner.fill_color", "false", "#ffffff")
    true_fill = state_property(runtime, "style.inner.fill_color", "true", "#8bd86f")
    state_fill = true_fill if value else false_fill
    hover_fill = state_property(runtime, "style.inner.fill_color", hover_state, "#9be884" if value else "#eef6ff")
    pressed_fill = state_property(runtime, "style.inner.fill_color", pressed_state, "#6fc657" if value else "#dbeafe")
    false_border = state_property(runtime, "style.outer.border_color", "false", "#111827")
    true_border = state_property(runtime, "style.outer.border_color", "true", "#184a24")
    state_border = true_border if value else false_border
    hover_border = state_property(runtime, "style.outer.border_color", hover_state, "#166534" if value else "#2563eb")
    pressed_border = state_property(runtime, "style.outer.border_color", pressed_state, "#14532d" if value else "#1d4ed8")
    false_inner_border = state_property(runtime, "style.inner.border_color", "false", false_border)
    true_inner_border = state_property(runtime, "style.inner.border_color", "true", true_border)
    state_inner_border = true_inner_border if value else false_inner_border
    hover_inner_border = state_property(runtime, "style.inner.border_color", hover_state, hover_border)
    pressed_inner_border = state_property(runtime, "style.inner.border_color", pressed_state, pressed_border)
    false_text_color = state_property(runtime, "state_text.style.text_color", "false", "#111827")
    true_text_color = state_property(runtime, "state_text.style.text_color", "true", "#0b3d19")
    text_color = true_text_color if value else false_text_color
    false_state_text = runtime_string(runtime, "state_text.false_text", "FALSE")
    true_state_text = runtime_string(runtime, "state_text.true_text", "TRUE")
    caption_color = safe_css_color(runtime.get("caption.style.text_color"), "#111827")
    caption_size = safe_css_length(runtime.get("caption.style.font_size"), "18px")
    caption_weight = safe_css_font_weight(runtime.get("caption.style.font_weight"), "600")
    caption_family = safe_css_font_family(
        runtime.get("caption.style.font_family"),
        "system-ui, Segoe UI, Arial, sans-serif",
    )
    text_size = safe_css_length(runtime.get("state_text.style.font_size"), "18px")
    text_weight = safe_css_font_weight(runtime.get("state_text.style.font_weight"), "700")

    inner_left = runtime_string(runtime, "style.inner.left", "52px" if variant == "circular" else "18px")
    inner_top = runtime_string(runtime, "style.inner.top", "23px" if variant == "circular" else "31px")
    inner_width = runtime_string(runtime, "style.inner.width", "56px" if variant == "circular" else "124px")
    inner_height = runtime_string(runtime, "style.inner.height", "56px" if variant == "circular" else "34px")
    inner_border_width = safe_css_length(runtime.get("style.inner.border_width"), "2px")
    focus_color = safe_css_color(runtime.get("style.focus_ring.color"), "#2563eb")
    focus_width = safe_css_length(runtime.get("style.focus_ring.width"), "3px") if focus_visible else "0px"
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
        f" data-frog-fill-false='{html.escape(false_fill)}'"
        f" data-frog-fill-true='{html.escape(true_fill)}'"
        f" data-frog-inner-border-false='{html.escape(false_inner_border)}'"
        f" data-frog-inner-border-true='{html.escape(true_inner_border)}'"
        f" data-frog-text-color-false='{html.escape(false_text_color)}'"
        f" data-frog-text-color-true='{html.escape(true_text_color)}'"
        f" data-frog-text-false='{html.escape(false_state_text)}'"
        f" data-frog-text-true='{html.escape(true_state_text)}'"
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
        f"--boolean-inner-border-width:{inner_border_width};"
        f"--boolean-text:{text_color};--boolean-text-font-size:{text_size};--boolean-text-font-weight:{text_weight};"
        f"--boolean-caption-color:{caption_color};--boolean-caption-font-size:{caption_size};--boolean-caption-font-weight:{caption_weight};--boolean-caption-font-family:{caption_family};"
        f"--boolean-focus-color:{focus_color};--boolean-focus-width:{focus_width};"
        f"--boolean-transition:{transition_ms}ms {transition_timing};--boolean-pressed-inset:{pressed_inset};"
    )
    skin = (
        f"<span class='boolean-state-face' data-frog-part='inner_face' aria-hidden='true'></span>"
        f"<img class='boolean-skin' src='{html.escape(asset_url(runtime.get('asset_ref')))}' alt='' aria-hidden='true'>"
    )
    overlays = (
        "<span class='boolean-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor'"
        f" style='{caption_anchor_style(runtime, geometry)}'>{html.escape(runtime_string(runtime, 'caption.text', widget['widget_id']))}</span>"
    )
    if state_text_visible:
        text = true_state_text if value else false_state_text
        state_x = runtime_number(runtime, "state_text.anchor.x", 80.0)
        state_y = runtime_number(runtime, "state_text.anchor.y", 50.0)
        state_style = f"left:{css_percent(pct(state_x, geometry['view_width']))};top:{css_percent(pct(state_y, geometry['view_height']))};"
        overlays += f"<span class='boolean-state-overlay' data-frog-part='state_text' data-svg-anchor='state_text.center' style='{state_style}'>{html.escape(text)}</span>"

    if is_control:
        return (
            f"<button class='frog-widget boolean-widget boolean-control' type='submit' name='input_value' value='{next_value}'"
            f" data-toggle-target='{next_value}' aria-pressed='{'true' if value else 'false'}'{attrs} style='{style}'>{skin}{overlays}</button>"
        )
    return f"<section class='frog-widget boolean-widget boolean-indicator' aria-readonly='true'{attrs} style='{style}'>{skin}{overlays}</section>"


def load_button_svg_geometry(asset_path: Path | None) -> dict[str, float]:
    geometry = {
        "view_width": 340.0,
        "view_height": 220.0,
        "caption_x": 16.0,
        "caption_y": 46.0,
        "face_x": 26.0,
        "face_y": 86.0,
        "face_width": 288.0,
        "face_height": 80.0,
        "state_text_x": 170.0,
        "state_text_y": 127.0,
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
    geometry["caption_x"] = svg_attribute_float(svg, "caption_text", "x", geometry["caption_x"])
    geometry["caption_y"] = svg_attribute_float(svg, "caption_text", "y", geometry["caption_y"])
    geometry["face_x"] = svg_attribute_float(svg, "face", "x", geometry["face_x"])
    geometry["face_y"] = svg_attribute_float(svg, "face", "y", geometry["face_y"])
    geometry["face_width"] = svg_attribute_float(svg, "face", "width", geometry["face_width"])
    geometry["face_height"] = svg_attribute_float(svg, "face", "height", geometry["face_height"])
    geometry["state_text_x"] = svg_attribute_float(svg, "state_text", "x", geometry["state_text_x"])
    geometry["state_text_y"] = svg_attribute_float(svg, "state_text", "y", geometry["state_text_y"])
    return geometry


def button_anchor_style(x: float, y: float, geometry: dict[str, float]) -> str:
    return svg_anchor_style(x, y, geometry)


def button_box_style(x: float, y: float, width: float, height: float, geometry: dict[str, float]) -> str:
    return svg_box_style(x, y, width, height, geometry)


def button_caption_anchor_style(runtime: dict[str, Any], geometry: dict[str, float]) -> str:
    x = runtime_number(runtime, "caption.anchor.x", geometry["caption_x"])
    y = runtime_number(runtime, "caption.anchor.y", geometry["caption_y"])
    align = runtime_string(runtime, "caption.align.horizontal", "left")
    style = button_anchor_style(x, y, geometry)
    style += f"transform:{caption_transform_for_align(align)};"
    style += f"text-align:{caption_text_align(align)};"
    style += f"color:{html.escape(safe_css_color(runtime.get('caption.style.text_color'), '#111827'))};"
    if not runtime_bool(runtime, "caption.visible", True):
        style += "display:none;"
    return style


def render_button_widget(widget: dict[str, Any], asset_path: Path) -> str:
    runtime = widget["runtime"]
    layout = widget["layout"]
    geometry = load_button_svg_geometry(asset_path)
    value = bool(runtime.get("pressed", runtime.get("value", False)))
    visual_state = "true" if value else "false"
    hover_state = "hover_true" if value else "hover_false"
    pressed_state = "pressed_true" if value else "pressed_false"
    transition_state = "transition_true_to_false" if value else "transition_false_to_true"
    caption = runtime_string(runtime, "caption.text", widget["widget_id"])
    false_state_text = runtime_string(runtime, "state_text.false_text", "OFF")
    true_state_text = runtime_string(runtime, "state_text.true_text", "ON")
    state_text = true_state_text if value else false_state_text
    input_id = runtime_string(runtime, "binding.public_input_id", "trigger_pressed")
    state_text_visible = runtime_bool(runtime, "state_text.visible", True)

    frame_fill = safe_css_color(runtime.get("style.frame.fill_color"), "transparent")
    frame_stroke = safe_css_color(runtime.get("style.frame.border_color"), "transparent")
    frame_width = safe_css_length(runtime.get("style.frame.border_width"), "0px")
    face_fill = safe_css_color(state_property(runtime, "style.face.fill_color", visual_state, "#e2e8f0"), "#e2e8f0")
    face_hover_fill = safe_css_color(state_property(runtime, "style.face.fill_color", hover_state, face_fill), face_fill)
    face_pressed_fill = safe_css_color(state_property(runtime, "style.face.fill_color", pressed_state, face_fill), face_fill)
    face_stroke = safe_css_color(state_property(runtime, "style.face.border_color", visual_state, "#334155"), "#334155")
    face_stroke_width = safe_css_length(runtime.get("style.face.border_width"), "4px")
    state_face_fill = safe_css_color(state_property(runtime, "style.state_face.fill_color", visual_state, "transparent"), "transparent")
    state_face_hover_fill = safe_css_color(state_property(runtime, "style.state_face.fill_color", hover_state, state_face_fill), state_face_fill)
    state_face_pressed_fill = safe_css_color(state_property(runtime, "style.state_face.fill_color", pressed_state, state_face_fill), state_face_fill)
    state_face_stroke = safe_css_color(state_property(runtime, "style.state_face.border_color", visual_state, "transparent"), "transparent")
    state_face_hover_stroke = safe_css_color(state_property(runtime, "style.state_face.border_color", hover_state, state_face_stroke), state_face_stroke)
    state_face_pressed_stroke = safe_css_color(state_property(runtime, "style.state_face.border_color", pressed_state, state_face_stroke), state_face_stroke)
    state_face_stroke_width = safe_css_length(runtime.get("style.state_face.border_width"), "0px")
    caption_size = safe_css_length(runtime.get("caption.style.font_size"), "18px")
    caption_weight = safe_css_font_weight(runtime.get("caption.style.font_weight"), "600")
    caption_family = safe_css_font_family(runtime.get("caption.style.font_family"), "system-ui, Segoe UI, Arial, sans-serif")
    false_text_color = safe_css_color(state_property(runtime, "state_text.style.text_color", "false", "#111827"), "#111827")
    true_text_color = safe_css_color(state_property(runtime, "state_text.style.text_color", "true", "#06381c"), "#06381c")
    text_color = true_text_color if value else false_text_color
    text_size = safe_css_length(runtime.get("state_text.style.font_size"), "20px")
    text_weight = safe_css_font_weight(runtime.get("state_text.style.font_weight"), "700")
    focus_color = safe_css_color(runtime.get("style.focus_ring.color"), "#2563eb")
    focus_width = safe_css_length(runtime.get("style.focus_ring.width"), "3px") if runtime_bool(runtime, "style.focus_ring.visible", True) else "0px"
    pressed_inset = safe_css_length(runtime.get("style.pressed.inset"), "2px")
    transition_ms = runtime_string(runtime, "style.transition.duration_ms", "120")
    transition_timing = runtime_string(runtime, "style.transition.timing", "ease-out")
    asset_route = asset_url(runtime.get("asset_ref"))

    style = (
        f"position:absolute;left:{css_px(layout_int(layout, 'x', 0))};top:{css_px(layout_int(layout, 'y', 0))};"
        f"width:{css_px(layout_int(layout, 'width', 220))};height:{css_px(layout_int(layout, 'height', 140))};"
        f"--frog-button-frame-fill:{html.escape(frame_fill)};"
        f"--frog-button-frame-stroke:{html.escape(frame_stroke)};"
        f"--frog-button-frame-stroke-width:{html.escape(frame_width)};"
        f"--frog-button-face-fill:{html.escape(face_fill)};"
        f"--frog-button-face-hover-fill:{html.escape(face_hover_fill)};"
        f"--frog-button-face-pressed-fill:{html.escape(face_pressed_fill)};"
        f"--frog-button-face-stroke:{html.escape(face_stroke)};"
        f"--frog-button-face-stroke-width:{html.escape(face_stroke_width)};"
        f"--frog-button-state-face-fill:{html.escape(state_face_fill)};"
        f"--frog-button-state-face-hover-fill:{html.escape(state_face_hover_fill)};"
        f"--frog-button-state-face-pressed-fill:{html.escape(state_face_pressed_fill)};"
        f"--frog-button-state-face-stroke:{html.escape(state_face_stroke)};"
        f"--frog-button-state-face-hover-stroke:{html.escape(state_face_hover_stroke)};"
        f"--frog-button-state-face-pressed-stroke:{html.escape(state_face_pressed_stroke)};"
        f"--frog-button-state-face-stroke-width:{html.escape(state_face_stroke_width)};"
        f"--frog-button-caption-font-size:{html.escape(caption_size)};"
        f"--frog-button-caption-font-weight:{html.escape(caption_weight)};"
        f"--frog-button-caption-font-family:{html.escape(caption_family)};"
        f"--frog-button-state-text-fill:{html.escape(text_color)};"
        f"--frog-button-state-text-font-size:{html.escape(text_size)};"
        f"--frog-button-state-text-font-weight:{html.escape(text_weight)};"
        f"--frog-button-focus-color:{html.escape(focus_color)};"
        f"--frog-button-focus-width:{html.escape(focus_width)};"
        f"--frog-button-pressed-inset:{html.escape(pressed_inset)};"
        f"--frog-button-transition:{html.escape(transition_ms)}ms {html.escape(transition_timing)};"
    )
    if not runtime_bool(runtime, "visible", True):
        style += "display:none;"

    state_text_overlay = ""
    if state_text_visible:
        state_x = runtime_number(runtime, "state_text.anchor.x", geometry["state_text_x"])
        state_y = runtime_number(runtime, "state_text.anchor.y", geometry["state_text_y"])
        state_text_overlay = (
            "<span class='button-state-overlay' data-frog-part='state_text' data-svg-anchor='state_text.center'"
            f" style='{button_anchor_style(state_x, state_y, geometry)}'>{html.escape(state_text)}</span>"
        )

    return (
        "<div class='frog-widget button-widget button-control'"
        f" data-widget-id='{html.escape(widget['widget_id'])}'"
        f" data-class-ref='{html.escape(widget['class_ref'])}'"
        f" data-role='{html.escape(widget['role'])}'"
        f" data-asset-ref='{html.escape(str(runtime.get('asset_ref', '')))}'"
        f" data-asset-route='{html.escape(asset_route)}'"
        f" data-current-value='{'true' if value else 'false'}'"
        f" data-realization-variant='{html.escape(runtime_string(runtime, 'realization.variant', 'rectangular'))}'"
        " data-frog-visual-law='wfrog-realization-state-map'"
        f" data-frog-visual-state='{visual_state}'"
        f" data-frog-hover-state='{hover_state}'"
        f" data-frog-pressed-state='{pressed_state}'"
        f" data-frog-transition-state='{transition_state}'"
        f" data-frog-state-text-visible='{'true' if state_text_visible else 'false'}'"
        f" data-frog-state-text-false='{html.escape(false_state_text)}'"
        f" data-frog-state-text-true='{html.escape(true_state_text)}'"
        f" data-frog-state-text-color-false='{html.escape(false_text_color)}'"
        f" data-frog-state-text-color-true='{html.escape(true_text_color)}'"
        f" style='{style}'>"
        f"<div class='button-skin' data-frog-asset-consumed='true' aria-hidden='true'>{asset_path.read_text(encoding='utf-8')}</div>"
        "<span class='button-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor'"
        f" style='{button_caption_anchor_style(runtime, geometry)}'>{html.escape(caption)}</span>"
        f"{state_text_overlay}"
        "<button class='button-press-overlay' type='button'"
        f" name='{html.escape(input_id)}' value='true'"
        f" aria-label='{html.escape(caption)}' aria-pressed='{'true' if value else 'false'}'"
        f" data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='{html.escape(input_id)}'"
        " data-frog-host-overlay='input' data-frog-align-to-part='face'"
        f" style='{button_box_style(geometry['face_x'], geometry['face_y'], geometry['face_width'], geometry['face_height'], geometry)}'>"
        "</button></div>"
    )


def button_press_to_boolean_script() -> str:
    return """<script>
(() => {
  const form = document.querySelector("form[action='/run']");
  const buttonWidget = document.querySelector("[data-widget-id='trigger_button']");
  const overlay = document.querySelector("[data-widget-id='trigger_button'] .button-press-overlay");
  const indicator = document.querySelector("[data-widget-id='pressed_indicator']");
  if (!form || !buttonWidget || !overlay || !indicator) {
    return;
  }

  const buttonStateText = buttonWidget.querySelector(".button-state-overlay[data-frog-part='state_text']");
  const stateText = indicator.querySelector("[data-frog-part='state_text']");
  const inputId = overlay.dataset.frogPublicInputId || overlay.name || "trigger_pressed";
  let pressed = false;
  let eventQueue = Promise.resolve();

  const buttonProperty = (base, value) => {
    const suffix = value ? "True" : "False";
    return buttonWidget.dataset[`${base}${suffix}`] || "";
  };

  const indicatorProperty = (base, value) => {
    const suffix = value ? "True" : "False";
    return indicator.dataset[`${base}${suffix}`] || "";
  };

  const applyIndicator = (value) => {
    const state = value ? "true" : "false";
    indicator.dataset.currentValue = state;
    indicator.dataset.frogVisualState = state;
    indicator.dataset.frogHoverState = value ? "hover_true" : "hover_false";
    indicator.dataset.frogPressedState = value ? "pressed_true" : "pressed_false";
    indicator.dataset.frogTransitionState = value ? "transition_false_to_true" : "transition_true_to_false";
    indicator.style.setProperty("--boolean-fill", indicatorProperty("frogFill", value));
    indicator.style.setProperty("--boolean-inner-border", indicatorProperty("frogInnerBorder", value));
    indicator.style.setProperty("--boolean-text", indicatorProperty("frogTextColor", value));
    if (stateText) {
      stateText.textContent = indicatorProperty("frogText", value);
    }
  };

  const publish = (value) => {
    const body = new URLSearchParams();
    body.set(inputId, value ? "true" : "false");
    eventQueue = eventQueue
      .catch(() => {})
      .then(() => fetch("/event", {
        method: "POST",
        headers: {"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
        body
      }))
      .catch(() => {});
  };

  const setPressed = (value) => {
    if (pressed === value) {
      return;
    }
    pressed = value;
    overlay.setAttribute("aria-pressed", value ? "true" : "false");
    buttonWidget.dataset.currentValue = value ? "true" : "false";
    buttonWidget.dataset.frogVisualState = value ? "true" : "false";
    buttonWidget.dataset.frogPressedState = value ? "pressed_true" : "pressed_false";
    if (buttonStateText) {
      buttonStateText.textContent = buttonProperty("frogStateText", value);
      buttonStateText.style.color = buttonProperty("frogStateTextColor", value);
    }
    applyIndicator(value);
    publish(value);
  };

  form.addEventListener("submit", (event) => event.preventDefault());
  overlay.addEventListener("click", (event) => event.preventDefault());
  const press = (event) => {
    if (event && event.button !== undefined && event.button !== 0) {
      return;
    }
    if (event) {
      event.preventDefault();
    }
    setPressed(true);
  };
  overlay.addEventListener("pointerdown", (event) => {
    if (overlay.setPointerCapture) {
      overlay.setPointerCapture(event.pointerId);
    }
    press(event);
  });
  const release = (event) => {
    if (!pressed) {
      return;
    }
    if (event) {
      event.preventDefault();
    }
    setPressed(false);
  };
  overlay.addEventListener("pointerup", release);
  overlay.addEventListener("pointercancel", release);
  overlay.addEventListener("lostpointercapture", release);
  overlay.addEventListener("mousedown", press);
  window.addEventListener("mouseup", release);
  overlay.addEventListener("mouseleave", release);
  overlay.addEventListener("touchstart", press, {passive: false});
  overlay.addEventListener("touchend", release, {passive: false});
  overlay.addEventListener("touchcancel", release, {passive: false});
  overlay.addEventListener("blur", release);
  overlay.addEventListener("keydown", (event) => {
    if (event.key !== " " && event.key !== "Enter") {
      return;
    }
    event.preventDefault();
    setPressed(true);
  });
  overlay.addEventListener("keyup", (event) => {
    if (event.key !== " " && event.key !== "Enter") {
      return;
    }
    event.preventDefault();
    setPressed(false);
  });
})();
</script>"""


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
  display: block;
}}
.numeric-skin svg {{
  width: 100%;
  height: 100%;
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
  font-size: var(--frog-numeric-caption-font-size);
  font-family: var(--frog-numeric-caption-font-family);
  font-weight: var(--frog-numeric-caption-font-weight);
  color: var(--frog-numeric-caption-color);
  line-height: 1;
  white-space: nowrap;
  pointer-events: none;
}}
.numeric-value-overlay {{
  position: absolute;
  box-sizing: border-box;
  font-family: var(--frog-numeric-text-font-family);
  font-size: var(--frog-numeric-text-font-size);
  font-weight: var(--frog-numeric-text-font-weight);
  color: var(--frog-numeric-text-color);
  line-height: 1;
  border: 0;
  background: transparent;
}}
.numeric-control-editor {{
  padding: 0 4px;
  border-radius: 0;
  outline: 0;
  background: transparent;
  appearance: textfield;
  -moz-appearance: textfield;
}}
.numeric-control-editor::-webkit-outer-spin-button,
.numeric-control-editor::-webkit-inner-spin-button {{
  appearance: none;
  margin: 0;
}}
.numeric-control-editor:focus {{
  outline: 0;
  background: transparent;
}}
.numeric-indicator-value {{
  display: flex;
  align-items: center;
  padding: 0 4px;
  pointer-events: none;
}}
.numeric-step-overlay {{
  position: absolute;
  box-sizing: border-box;
  padding: 0;
  border: 1px solid var(--frog-numeric-step-border);
  border-radius: 0;
  background: var(--frog-numeric-step-fill);
  color: transparent;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
}}
.numeric-step-overlay:focus {{
  outline: 0;
}}
.numeric-step-overlay:active {{
  background: var(--frog-numeric-step-fill-pressed);
  border-color: var(--frog-numeric-step-border-pressed);
}}
.numeric-step-overlay::before {{
  content: '';
  display: block;
  width: 0;
  height: 0;
}}
.numeric-increment::before {{
  border-left: 5px solid transparent;
  border-right: 5px solid transparent;
  border-bottom: 6px solid var(--frog-numeric-step-symbol);
}}
.numeric-decrement::before {{
  border-left: 5px solid transparent;
  border-right: 5px solid transparent;
  border-top: 6px solid var(--frog-numeric-step-symbol);
}}
.numeric-increment:active::before {{
  border-bottom-color: var(--frog-numeric-step-symbol-pressed);
}}
.numeric-decrement:active::before {{
  border-top-color: var(--frog-numeric-step-symbol-pressed);
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
.numeric-step-overlay {{
  padding: 0;
  border-radius: 0;
  color: transparent;
  font-weight: 400;
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
<script>
document.addEventListener('click', function(event) {{
  const button = event.target.closest('.numeric-step-overlay');
  if (!button) return;
  const input = document.getElementById(button.dataset.target);
  if (!input || input.disabled) return;
  const step = Number(button.dataset.step || '1');
  const min = Number(input.min || '0');
  const max = Number(input.max || '65535');
  const next = Math.min(max, Math.max(min, Number(input.value || '0') + step));
  input.value = String(next);
  input.dispatchEvent(new Event('input', {{ bubbles: true }}));
}});
</script>
</head>
<body>
<h1>{html.escape(snapshot['ui_runtime']['panel']['title'])}</h1>
<p class="meta">Example 05 - .frog front panel + Default Numeric .wfrog realization assets + Python runtime</p>
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
.boolean-caption-overlay{{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:var(--boolean-caption-font-size);font-weight:var(--boolean-caption-font-weight);font-family:var(--boolean-caption-font-family);line-height:1;color:var(--boolean-caption-color);white-space:nowrap;pointer-events:none;z-index:3;}}
.boolean-state-face{{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:var(--boolean-inner-border-width) solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}}
.boolean-widget[data-realization-variant='circular'] .boolean-state-face{{border-radius:50%;}}
.boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{{box-shadow:none;}}
.boolean-control:hover .boolean-state-face{{background:var(--boolean-hover-fill);border-color:var(--boolean-hover-inner-border);box-shadow:inset 0 1px 0 rgba(255,255,255,.72),0 2px 5px rgba(15,23,42,.18);}}
.boolean-control[data-frog-frame-visible='false']:hover .boolean-state-face{{box-shadow:none;}}
.boolean-control:active .boolean-state-face{{background:var(--boolean-pressed-fill);border-color:var(--boolean-pressed-inner-border);box-shadow:inset 0 2px 4px rgba(15,23,42,.22);transform:translateY(var(--boolean-pressed-inset));}}
.boolean-control[data-frog-frame-visible='false']:active .boolean-state-face{{box-shadow:none;}}
.boolean-control:focus-visible .boolean-state-face{{outline:var(--boolean-focus-width) solid var(--boolean-focus-color);}}
.boolean-state-overlay{{position:absolute;transform:translate(-50%,-50%);text-align:center;font-size:var(--boolean-text-font-size);font-weight:var(--boolean-text-font-weight);line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;white-space:nowrap;}}
.actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}
.state-link{{font-size:16px;}}
.diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}
.diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}
</style>
</head>
<body>
<h1>{html.escape(panel['title'])}</h1>
<p class="meta">Example 06 - .frog front panel + Default Boolean .wfrog realization assets + Python runtime</p>
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


class StringBrowserUiRuntime:
    def __init__(
        self,
        *,
        contract_path: str | Path | None = None,
        wfrog_path: str | Path | None = None,
        host: str = "127.0.0.1",
        port: int = 0,
        open_browser: bool = True,
        native_kernel_bridge: NativeStringKernelBridge | None = None,
    ) -> None:
        self.runtime = StringRuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
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

        rendered_widgets = "".join(
            render_string_widget(widget, self.runtime.asset_map.get(str(widget["runtime"].get("asset_ref", "")).split(":", 1)[-1]))
            for widget in widgets
        )
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
.string-widget{{font-family:Segoe UI,Arial,sans-serif;}}
.string-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}
.string-skin svg{{width:100%;height:100%;display:block;--frog-string-label-display:inherit;--frog-string-caption-display:inherit;--frog-string-placeholder-display:inherit;--frog-string-frame-fill:inherit;--frog-string-frame-stroke:inherit;--frog-string-frame-stroke-width:inherit;--frog-string-text-region-fill:inherit;--frog-string-text-region-stroke:inherit;--frog-string-text-region-stroke-width:inherit;--frog-string-text-fill:inherit;--frog-string-text-font-size:inherit;--frog-string-text-font-weight:inherit;}}
.string-skin #label_text,.string-skin #caption_text,.string-skin #placeholder,.string-skin #text_value{{display:none;}}
.string-control:hover .string-skin svg{{--frog-string-text-region-fill:var(--frog-string-text-region-fill-hover);--frog-string-text-region-stroke:var(--frog-string-text-region-stroke-hover);--frog-string-text-region-stroke-width:var(--frog-string-text-region-stroke-width-hover);}}
.string-caption-overlay{{position:absolute;transform:translateY(-50%);font-size:var(--frog-string-caption-font-size);font-weight:var(--frog-string-caption-font-weight);font-family:var(--frog-string-caption-font-family);color:var(--frog-string-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}}
.string-value-overlay{{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1.2;border:0;background:transparent;}}
.string-control-editor{{padding:0 8px;outline:0;}}
.string-control-editor:focus{{outline:0;}}
.string-indicator-value{{display:flex;align-items:center;padding:0 8px;pointer-events:none;}}
.actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}
button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}
.diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}
.diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}
</style>
</head>
<body>
<h1>{html.escape(panel['title'])}</h1>
<p class="meta">Example 07 - .frog front panel + Default String .wfrog realization assets + Python runtime</p>
<dl class="runtime-facts" aria-label="Runtime facts">
  <div><dt>Runtime</dt><dd>Python reference runtime</dd></div>
  <div><dt>Execution</dt><dd>{'native kernel bridge' if uses_native_kernel else 'string contract executor'}</dd></div>
  <div><dt>Compiler backend</dt><dd>{'LLVM native string kernel artifact' if uses_native_kernel else 'none for Example 07'}</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(panel['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="{'llvm' if uses_native_kernel else 'none'}" data-execution-path="{'native_kernel_bridge' if uses_native_kernel else 'python_string_contract_executor'}" style="width:{panel_layout['width']}px;height:{panel_layout['height']}px;">
    {rendered_widgets}
  </div>
  <div class="actions"><button type="submit">Run Example 07</button><a class="state-link" href="/state.json">state.json</a></div>
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
        raw_value = form.get("input_text", ["hello world"])[0]
        try:
            if self.native_kernel_bridge is None:
                self.runtime.execute(control_value=raw_value)
            else:
                self.runtime.execute_with_native_kernel_bridge(self.native_kernel_bridge, control_value=raw_value)
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


class EnumBrowserUiRuntime:
    def __init__(
        self,
        *,
        contract_path: str | Path | None = None,
        wfrog_path: str | Path | None = None,
        host: str = "127.0.0.1",
        port: int = 0,
        open_browser: bool = True,
        native_kernel_bridge: NativeEnumKernelBridge | None = None,
    ) -> None:
        self.runtime = EnumRuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
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
        rendered_widgets = "".join(
            render_enum_widget(widget, self.runtime.asset_map.get(str(widget["runtime"].get("asset_ref", "")).split(":", 1)[-1]))
            for widget in widgets
        )
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
.front-panel{{position:relative;background:#ffffff;border-radius:10px;box-shadow:0 4px 14px rgba(15,23,42,0.08);overflow:visible;}}
.frog-widget{{position:absolute;box-sizing:border-box;}}
.enum-widget{{font-family:Segoe UI,Arial,sans-serif;overflow:visible;}}
.enum-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}
.enum-skin svg{{width:100%;height:100%;display:block;}}
.enum-skin #label_text,.enum-skin #caption_text,.enum-skin #value_display{{display:none;}}
.enum-caption-overlay{{position:absolute;transform:translateY(-50%);font-size:var(--frog-enum-caption-font-size);font-weight:var(--frog-enum-caption-font-weight);font-family:var(--frog-enum-caption-font-family);color:var(--frog-enum-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}}
.enum-value-overlay{{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:normal;border:0;background:transparent;}}
.enum-value-display-overlay{{position:absolute;box-sizing:border-box;display:flex;align-items:center;padding:0 var(--frog-enum-text-padding-inline);font-family:Segoe UI,Arial,sans-serif;line-height:normal;transform:translateY(var(--frog-enum-text-vertical-offset));z-index:3;}}
.enum-widget .enum-display-button{{border:0;background:transparent;text-align:left;justify-content:flex-start;appearance:none;cursor:pointer;}}
.enum-widget .enum-display-button:focus,.enum-widget .enum-display-button:focus-visible,.enum-widget .enum-display-button:active{{outline:0;box-shadow:none;}}
.enum-select-state{{display:none;}}
.enum-selector-overlay{{position:absolute;box-sizing:border-box;display:flex;align-items:center;justify-content:center;border-style:solid;border-width:var(--frog-enum-selector-stroke-width);border-radius:var(--frog-enum-selector-radius);}}
.enum-selector-overlay::after{{content:'';width:0;height:0;border-left:calc(var(--frog-enum-selector-symbol-width) / 2) solid transparent;border-right:calc(var(--frog-enum-selector-symbol-width) / 2) solid transparent;border-top:var(--frog-enum-selector-symbol-height) solid currentColor;}}
.enum-control:has(.enum-display-button:hover) .enum-skin #value_face,.enum-control:has(.enum-dropdown:not([hidden])) .enum-skin #value_face{{fill:var(--frog-enum-value-hover-fill) !important;}}
.enum-indicator-value{{display:flex;align-items:center;padding:0 var(--frog-enum-text-padding-inline);pointer-events:none;line-height:normal;transform:translateY(var(--frog-enum-text-vertical-offset));}}
.enum-dropdown{{position:absolute;box-sizing:border-box;z-index:30;background:var(--frog-enum-dropdown-fill);border:var(--frog-enum-dropdown-border-width) solid var(--frog-enum-dropdown-border);}}
.enum-dropdown[hidden]{{display:none;}}
.enum-dropdown-option{{width:100%;min-height:var(--frog-enum-dropdown-option-height);display:flex;align-items:center;justify-content:flex-start;padding:0 var(--frog-enum-dropdown-option-padding-inline);border:0;border-radius:0;background:var(--frog-enum-dropdown-option-fill);color:var(--frog-enum-dropdown-option-text);font-family:var(--frog-enum-dropdown-option-font-family);font-size:var(--frog-enum-dropdown-option-font-size);font-weight:var(--frog-enum-dropdown-option-font-weight);font-style:var(--frog-enum-dropdown-option-font-style);text-align:left;cursor:pointer;}}
.enum-dropdown-option:hover,.enum-dropdown-option:focus{{background:var(--frog-enum-dropdown-option-hover-fill);color:var(--frog-enum-dropdown-option-hover-text);outline:0;}}
.enum-dropdown-option[aria-selected='true']{{background:var(--frog-enum-dropdown-option-selected-fill);color:var(--frog-enum-dropdown-option-selected-text);}}
.enum-widget .enum-selector-button{{padding:0;border-style:solid;border-width:var(--frog-enum-selector-stroke-width);border-radius:var(--frog-enum-selector-radius);font-weight:400;cursor:pointer;appearance:none;z-index:4;background:var(--frog-enum-selector-fill);border-color:var(--frog-enum-selector-stroke);color:var(--frog-enum-selector-symbol);}}
.enum-widget .enum-selector-button:hover{{background:var(--frog-enum-selector-hover-fill);border-color:var(--frog-enum-selector-hover-stroke);color:var(--frog-enum-selector-hover-symbol);}}
.enum-widget .enum-selector-button:focus,.enum-widget .enum-selector-button:focus-visible,.enum-widget .enum-selector-button:active{{outline:0;box-shadow:none;}}
.actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}
button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}
.diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}
.diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}
</style>
<script>
function frogCloseEnumDropdown(menuId, displayId){{const menu=document.getElementById(menuId);const display=document.getElementById(displayId);if(menu){{menu.hidden=true;}}if(display){{display.setAttribute('aria-expanded','false');}}}}
function frogCloseOtherEnumDropdowns(menuId){{document.querySelectorAll('.enum-dropdown').forEach(function(m){{if(m.id!==menuId){{m.hidden=true;}}}});document.querySelectorAll('.enum-display-button,.enum-selector-button').forEach(function(b){{if(b.getAttribute('aria-controls')!==menuId){{b.setAttribute('aria-expanded','false');}}}});}}
function frogToggleEnumDropdown(menuId,displayId){{const menu=document.getElementById(menuId);const display=document.getElementById(displayId);if(!menu){{return;}}frogCloseOtherEnumDropdowns(menuId);menu.hidden=!menu.hidden;if(display){{display.setAttribute('aria-expanded',menu.hidden?'false':'true');}}}}
function frogUpdateEnumDisplay(select,displayId){{const d=document.getElementById(displayId);if(!d){{return;}}const o=select.options[select.selectedIndex];if(o){{d.textContent=o.textContent;}}const menu=document.getElementById(select.id.replace('_value','_dropdown'));if(menu){{menu.querySelectorAll('.enum-dropdown-option').forEach(function(option){{option.setAttribute('aria-selected',option.getAttribute('data-enum-value')===select.value?'true':'false');}});}}}}
function frogSelectEnumOption(option,selectId,displayId,menuId){{const s=document.getElementById(selectId);const d=document.getElementById(displayId);if(!s||!option){{return;}}const value=option.getAttribute('data-enum-value');s.value=value;if(d){{d.textContent=option.textContent;}}frogUpdateEnumDisplay(s,displayId);frogCloseEnumDropdown(menuId,displayId);s.dispatchEvent(new Event('input',{{bubbles:true}}));s.dispatchEvent(new Event('change',{{bubbles:true}}));}}
document.addEventListener('click',function(event){{if(!event.target.closest('.enum-widget')){{document.querySelectorAll('.enum-dropdown').forEach(function(m){{m.hidden=true;}});document.querySelectorAll('.enum-display-button,.enum-selector-button').forEach(function(b){{b.setAttribute('aria-expanded','false');}});}}}});
</script>
</head>
<body>
<h1>{html.escape(panel['title'])}</h1>
<p class="meta">Example 08 - .frog front panel + Default Enum .wfrog realization assets + Python runtime</p>
<dl class="runtime-facts" aria-label="Runtime facts">
  <div><dt>Runtime</dt><dd>Python reference runtime</dd></div>
  <div><dt>Execution</dt><dd>{'native kernel bridge' if uses_native_kernel else 'enum contract executor'}</dd></div>
  <div><dt>Compiler backend</dt><dd>{'LLVM native enum kernel artifact' if uses_native_kernel else 'none for Example 08'}</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(panel['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="{'llvm' if uses_native_kernel else 'none'}" data-execution-path="{'native_kernel_bridge' if uses_native_kernel else 'python_enum_contract_executor'}" style="width:{panel_layout['width']}px;height:{panel_layout['height']}px;">
    {rendered_widgets}
  </div>
  <div class="actions"><button type="submit">Run Example 08</button><a class="state-link" href="/state.json">state.json</a></div>
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
        raw_value = form.get("mode_value", ["run"])[0]
        try:
            if self.native_kernel_bridge is None:
                self.runtime.execute(control_value=raw_value)
            else:
                self.runtime.execute_with_native_kernel_bridge(self.native_kernel_bridge, control_value=raw_value)
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


def path_widget_binding(widget: dict[str, Any]) -> dict[str, Any]:
    binding = widget.get("binding")
    return binding if isinstance(binding, dict) else {}


def path_public_input_id(widget: dict[str, Any], contract_binding: dict[str, Any] | None = None) -> str:
    props = widget.get("props", {})
    binding = contract_binding or path_widget_binding(widget)
    return str(
        binding.get("public_input_id")
        or props.get("binding.public_input_id")
        or props.get("binding.preview_input_id")
        or ""
    )


def path_public_output_id(widget: dict[str, Any], contract_binding: dict[str, Any] | None = None) -> str:
    props = widget.get("props", {})
    binding = contract_binding or path_widget_binding(widget)
    return str(
        binding.get("public_output_id")
        or props.get("binding.public_output_id")
        or props.get("binding.preview_output_id")
        or ""
    )


class PathRuntimeCore:
    def __init__(self, *, contract_path: str | Path | None = None, wfrog_path: str | Path | None = None) -> None:
        self.contract_path = Path(contract_path or default_example09_contract_path()).resolve()
        self.wfrog_path = Path(wfrog_path or default_example09_wfrog_path()).resolve()
        self.contract = load_contract_json(self.contract_path)
        if contract_example_id(self.contract) != "09_path_value_roundtrip":
            raise RuntimeError("PathRuntimeCore expects Example 09.")
        unit = self.contract.get("units", [{}])[0]
        if unit.get("kind") != "path_value_roundtrip_ui_unit":
            raise RuntimeError("PathRuntimeCore expects path_value_roundtrip_ui_unit.")
        self.unit = unit
        self.package = load_contract_json(self.wfrog_path)
        self.panel = load_source_front_panel_from_contract(self.contract, self.contract_path)
        self.asset_map = {
            item["asset_id"]: (self.wfrog_path.parent / Path(item["path"])).resolve()
            for item in self.package.get("svg_assets", [])
        }
        self.contract_bindings = {
            item.get("widget_id"): item.get("binding", {})
            for item in unit.get("ui_bindings", {}).get("widgets", [])
            if isinstance(item, dict)
        }
        self.widgets = {
            entry["instance_id"]: entry
            for entry in self.panel.get("widgets", [])
            if entry.get("class_ref") in {"frog.widgets.path_control", "frog.widgets.path_indicator"}
        }
        for widget_id in ("path_input", "path_result"):
            if widget_id not in self.widgets:
                raise RuntimeError(f"Example 09 panel must contain {widget_id}.")
        for widget_id, widget in self.widgets.items():
            asset_ref = str(widget.get("visual", {}).get("asset_ref", ""))
            if not asset_ref.startswith("asset:"):
                raise RuntimeError(f"Path widget {widget_id} must reference a .wfrog SVG asset.")
            asset_id = asset_ref.split(":", 1)[1]
            if asset_id not in self.asset_map or not self.asset_map[asset_id].exists():
                raise RuntimeError(f"Path widget {widget_id} asset path must exist.")
        self.values = {
            widget_id: str(widget.get("props", {}).get("value", ""))
            for widget_id, widget in self.widgets.items()
        }
        self.last_result = ""
        self.execute_all({})

    def _input_id(self, widget: dict[str, Any]) -> str:
        return path_public_input_id(widget, self.contract_bindings.get(widget.get("instance_id")))

    def _output_id(self, widget: dict[str, Any]) -> str:
        return path_public_output_id(widget, self.contract_bindings.get(widget.get("instance_id")))

    def _set_input_value(self, input_id: str, value: str) -> None:
        if len(value.encode("utf-8")) > 256:
            raise RuntimeError(f"{input_id} must remain within 256 UTF-8 bytes.")
        for widget_id, widget in self.widgets.items():
            if self._input_id(widget) == input_id:
                self.values[widget_id] = value

    def _input_value(self, input_id: str) -> str:
        for widget_id, widget in self.widgets.items():
            if self._input_id(widget) == input_id:
                return self.values.get(widget_id, "")
        return ""

    def _publish_output(self, output_id: str, value: str) -> None:
        for widget_id, widget in self.widgets.items():
            if self._output_id(widget) == output_id:
                self.values[widget_id] = value

    def _execution_pairs(self) -> list[tuple[str, str]]:
        public_io = self.unit.get("public_io", {})
        inputs = public_io.get("inputs", [])
        outputs = public_io.get("outputs", [])
        default_input = inputs[0].get("id") if inputs else ""
        default_output = outputs[0].get("id") if outputs else ""
        pairs: list[tuple[str, str]] = []
        for widget in self.widgets.values():
            role = "control" if widget.get("class_ref") == "frog.widgets.path_control" else "indicator"
            if role != "control":
                continue
            input_id = self._input_id(widget)
            if not input_id:
                continue
            output_id = str(widget.get("props", {}).get("binding.output_id", ""))
            if not output_id and input_id == default_input:
                output_id = str(default_output)
            if output_id:
                pairs.append((input_id, output_id))
        return pairs

    def execute_all(self, control_values: dict[str, str]) -> dict[str, Any]:
        for input_id, value in control_values.items():
            self._set_input_value(input_id, value)
        for input_id, output_id in self._execution_pairs():
            result = self._input_value(input_id)
            self._publish_output(output_id, result)
            if output_id == self.unit.get("public_io", {}).get("outputs", [{}])[0].get("id"):
                self.last_result = result
        return self.execution_artifact()

    def execute(self, control_value: str | None = None) -> dict[str, Any]:
        values = {"input_path": str(control_value)} if control_value is not None else {}
        return self.execute_all(values)

    def execute_all_with_native_kernel_bridge(
        self,
        bridge: NativeStringKernelBridge,
        control_values: dict[str, str],
    ) -> dict[str, Any]:
        if bridge.manifest.source_lowered_unit != "Examples/09_path_value_roundtrip/main.lowering.json":
            raise RuntimeError("Unexpected native path kernel source lowered unit.")
        for input_id, value in control_values.items():
            self._set_input_value(input_id, value)
        for input_id, output_id in self._execution_pairs():
            result = bridge.run(self._input_value(input_id))
            if not result.ok:
                raise RuntimeError(result.diagnostic or "native path kernel execution failed.")
            self._publish_output(output_id, result.result)
            if output_id == self.unit.get("public_io", {}).get("outputs", [{}])[0].get("id"):
                self.last_result = result.result
        return self.execution_artifact()

    def execute_with_native_kernel_bridge(
        self,
        bridge: NativeStringKernelBridge,
        control_value: str | None = None,
    ) -> dict[str, Any]:
        values = {"input_path": str(control_value)} if control_value is not None else {}
        return self.execute_all_with_native_kernel_bridge(bridge, values)

    def _runtime_for(self, widget_id: str) -> dict[str, Any]:
        widget = self.widgets[widget_id]
        props = dict(widget.get("props", {}))
        visual = dict(widget.get("visual", {}))
        binding = self.contract_bindings.get(widget_id, {})
        runtime = {
            "value": self.values.get(widget_id, ""),
            "path.display_value": self.values.get(widget_id, ""),
            "path.kind": props.get("path.kind", "file"),
            "path.validation_state": props.get("path.validation_state", "unknown"),
            "label.text": props.get("label.text", ""),
            "caption.text": props.get("caption.text", widget_id),
            "asset_ref": visual.get("asset_ref"),
            "realization.variant": props.get("realization.variant", "rectangular_field"),
        }
        if "public_input_id" in binding:
            runtime["binding.public_input_id"] = binding["public_input_id"]
        if "public_output_id" in binding:
            runtime["binding.public_output_id"] = binding["public_output_id"]
        for key in [
            "caption.visible",
            "caption.anchor.x",
            "caption.anchor.y",
            "caption.align.horizontal",
            "caption.style.text_color",
            "caption.style.font_family",
            "caption.style.font_size",
            "caption.style.font_weight",
            "display.icon_visible",
            "display.validation_marker_visible",
            "display.text_overflow_visible",
            "browse.enabled",
            "browse.button_visible",
            "style.path_face.fill_color",
            "style.path_face.fill_color.hover",
            "style.path_face.border_color",
            "style.path_face.border_color.hover",
            "style.path_face.border_width",
            "style.path_display.color",
            "style.path_display.font_size",
            "style.path_display.font_weight",
            "style.path_display.padding_inline",
            "style.path_display.baseline_offset",
            "style.path_display.line_height",
            "style.path_icon.fill_color",
            "style.path_icon.front_fill_color",
            "style.path_icon.stroke_color",
            "style.path_icon.highlight_color",
            "style.browse_button.fill_color",
            "style.browse_button.fill_color.hover",
            "style.browse_button.border_color",
            "style.browse_button.border_color.hover",
            "style.browse_button.border_width",
            "style.browse_button.text_color",
            "style.browse_button.text_font_size",
            "binding.preview_input_id",
            "binding.preview_output_id",
            "binding.input_id",
            "binding.output_id",
            "interaction.enabled",
            "interaction.read_only",
        ]:
            if key in props:
                runtime[key] = props[key]
        return runtime

    def execution_artifact(self) -> dict[str, Any]:
        widgets = []
        ui_outputs = {}
        for widget_id, widget in self.widgets.items():
            role = "control" if widget.get("class_ref") == "frog.widgets.path_control" else "indicator"
            ui_outputs[widget_id] = self.values.get(widget_id, "")
            widgets.append({
                "widget_id": widget_id,
                "class_ref": widget["class_ref"],
                "role": role,
                "layout": dict(widget["layout"]),
                "runtime": self._runtime_for(widget_id),
            })
        return {
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {
                "unit_ids": ["main"],
                "backend_family": self.contract["backend_family"],
                "source_ref": dict(self.contract["source_ref"]),
            },
            "execution_summary": {
                "mode": "path_value_roundtrip",
                "executed_unit": "main",
                "operation": "copy",
                "input_path": self._input_value("input_path"),
                "result_path": self.last_result,
            },
            "outputs": {"public": {"result_path": self.last_result}, "ui": ui_outputs},
            "ui_runtime": {
                "panel": {
                    "panel_id": self.panel["panel_id"],
                    "title": self.panel["title"],
                    "class_ref": self.panel["class_ref"],
                    "layout": dict(self.panel["layout"]),
                },
                "widgets": widgets,
            },
            "diagnostics": [],
        }


def load_path_svg_geometry(asset_path: Path | None) -> dict[str, float]:
    geometry = {
        "view_width": 520.0,
        "view_height": 150.0,
        "caption_x": 16.0,
        "caption_y": 46.0,
        "path_face_x": 22.0,
        "path_face_y": 78.0,
        "path_face_width": 390.0,
        "path_face_height": 36.0,
        "path_text_x": 56.0,
        "path_text_y": 96.0,
        "browse_button_x": 424.0,
        "browse_button_y": 78.0,
        "browse_button_width": 34.0,
        "browse_button_height": 36.0,
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
    geometry["caption_x"] = svg_attribute_float(svg, "caption_text", "x", geometry["caption_x"])
    geometry["caption_y"] = svg_attribute_float(svg, "caption_text", "y", geometry["caption_y"])
    geometry["path_face_x"] = svg_attribute_float(svg, "path_face", "x", geometry["path_face_x"])
    geometry["path_face_y"] = svg_attribute_float(svg, "path_face", "y", geometry["path_face_y"])
    geometry["path_face_width"] = svg_attribute_float(svg, "path_face", "width", geometry["path_face_width"])
    geometry["path_face_height"] = svg_attribute_float(svg, "path_face", "height", geometry["path_face_height"])
    geometry["path_text_x"] = svg_attribute_float(svg, "path_display", "x", geometry["path_text_x"])
    geometry["path_text_y"] = svg_attribute_float(svg, "path_display", "y", geometry["path_text_y"])
    geometry["browse_button_x"] = svg_attribute_float(svg, "browse_button", "x", geometry["browse_button_x"])
    geometry["browse_button_y"] = svg_attribute_float(svg, "browse_button", "y", geometry["browse_button_y"])
    geometry["browse_button_width"] = svg_attribute_float(svg, "browse_button", "width", geometry["browse_button_width"])
    geometry["browse_button_height"] = svg_attribute_float(svg, "browse_button", "height", geometry["browse_button_height"])
    return geometry


def render_path_skin(asset_path: Path | None, runtime: dict[str, Any]) -> str:
    if asset_path is None or not asset_path.exists():
        return "<div class='path-skin missing-skin'></div>"
    icon_visible = runtime_bool(runtime, "display.icon_visible", True)
    browse_visible = runtime_bool(runtime, "browse.button_visible", False)
    validation_visible = runtime_bool(runtime, "display.validation_marker_visible", False)
    overflow_visible = runtime_bool(runtime, "display.text_overflow_visible", False)
    face_fill = safe_css_color(runtime.get("style.path_face.fill_color"), "#ffffff")
    face_stroke = safe_css_color(runtime.get("style.path_face.border_color"), "#64748b")
    button_fill = safe_css_color(runtime.get("style.browse_button.fill_color"), "#f8fafc")
    button_stroke = safe_css_color(runtime.get("style.browse_button.border_color"), "#64748b")
    style = (
        "--frog-path-label-display:none;"
        "--frog-path-caption-display:none;"
        f"--frog-path-face-fill:{html.escape(face_fill)};"
        f"--frog-path-face-stroke:{html.escape(face_stroke)};"
        f"--frog-path-face-stroke-width:{html.escape(safe_css_length(runtime.get('style.path_face.border_width'), '2px'))};"
        f"--frog-path-face-fill-hover:{html.escape(safe_css_color(runtime.get('style.path_face.fill_color.hover'), face_fill))};"
        f"--frog-path-face-stroke-hover:{html.escape(safe_css_color(runtime.get('style.path_face.border_color.hover'), face_stroke))};"
        f"--frog-path-text-fill:{html.escape(safe_css_color(runtime.get('style.path_display.color'), '#111827'))};"
        f"--frog-path-text-font-size:{html.escape(safe_css_length(runtime.get('style.path_display.font_size'), '15px'))};"
        f"--frog-path-text-font-weight:{html.escape(safe_css_font_weight(runtime.get('style.path_display.font_weight'), '400'))};"
        f"--frog-path-button-fill:{html.escape(button_fill)};"
        f"--frog-path-button-fill-hover:{html.escape(safe_css_color(runtime.get('style.browse_button.fill_color.hover'), button_fill))};"
        f"--frog-path-button-stroke:{html.escape(button_stroke)};"
        f"--frog-path-button-stroke-hover:{html.escape(safe_css_color(runtime.get('style.browse_button.border_color.hover'), button_stroke))};"
        f"--frog-path-button-stroke-width:{html.escape(safe_css_length(runtime.get('style.browse_button.border_width'), '1px'))};"
        f"--frog-path-button-text-fill:{html.escape(safe_css_color(runtime.get('style.browse_button.text_color'), '#111827'))};"
        f"--frog-path-button-text-font-size:{html.escape(safe_css_length(runtime.get('style.browse_button.text_font_size'), '13px'))};"
        f"--frog-path-icon-display:{'inline' if icon_visible else 'none'};"
        f"--frog-path-icon-fill:{html.escape(safe_css_color(runtime.get('style.path_icon.fill_color'), '#facc15'))};"
        f"--frog-path-icon-front-fill:{html.escape(safe_css_color(runtime.get('style.path_icon.front_fill_color'), '#fde68a'))};"
        f"--frog-path-icon-stroke:{html.escape(safe_css_color(runtime.get('style.path_icon.stroke_color'), '#b45309'))};"
        f"--frog-path-icon-highlight:{html.escape(safe_css_color(runtime.get('style.path_icon.highlight_color'), '#fff7cc'))};"
        f"--frog-path-browse-display:{'inline' if browse_visible else 'none'};"
        f"--frog-path-validation-display:{'inline' if validation_visible else 'none'};"
        f"--frog-path-overflow-display:{'inline' if overflow_visible else 'none'};"
    )
    return f"<div class='path-skin' aria-hidden='true' style='{style}'>{asset_path.read_text(encoding='utf-8')}</div>"


def render_path_widget(widget: dict[str, Any], asset_path: Path | None) -> str:
    layout = widget["layout"]
    runtime = widget["runtime"]
    is_control = widget["role"] == "control"
    geometry = load_path_svg_geometry(asset_path)
    asset_ref = str(runtime.get("asset_ref", ""))
    asset_route = f"/asset/{asset_ref.split(':', 1)[1]}" if asset_ref.startswith("asset:") else ""
    icon_visible = runtime_bool(runtime, "display.icon_visible", True)
    text_x = max(geometry["path_face_x"], geometry["path_text_x"]) if icon_visible else geometry["path_face_x"]
    text_width = max(0.0, geometry["path_face_width"] - (text_x - geometry["path_face_x"]))
    value_style = svg_box_style(text_x, geometry["path_face_y"], text_width, geometry["path_face_height"], geometry)
    browse_style = svg_box_style(
        geometry["browse_button_x"],
        geometry["browse_button_y"],
        geometry["browse_button_width"],
        geometry["browse_button_height"],
        geometry,
    )
    browse_visible = runtime_bool(runtime, "browse.button_visible", is_control)
    interaction_enabled = runtime_bool(runtime, "interaction.enabled", is_control)
    input_id = str(
        runtime.get("binding.public_input_id")
        or runtime.get("binding.preview_input_id")
        or f"{widget['widget_id']}_value"
    )
    value = html.escape(runtime_string(runtime, "value", ""))
    caption = html.escape(runtime_string(runtime, "caption.text", widget["widget_id"]))
    text_color = html.escape(safe_css_color(runtime.get("style.path_display.color"), "#111827"))
    text_size = html.escape(safe_css_length(runtime.get("style.path_display.font_size"), "15px"))
    text_weight = html.escape(safe_css_font_weight(runtime.get("style.path_display.font_weight"), "400"))
    text_padding = html.escape(safe_css_length(runtime.get("style.path_display.padding_inline"), "8px"))
    text_line_height = html.escape(
        safe_css_length(
            runtime.get("style.path_display.line_height"),
            css_px((geometry["path_face_height"] / geometry["view_height"]) * layout_int(layout, "height", 120)),
        )
    )
    text_baseline = html.escape(safe_css_signed_length(runtime.get("style.path_display.baseline_offset"), "0px"))
    attrs = (
        f" data-widget-id='{html.escape(widget['widget_id'])}'"
        f" data-class-ref='{html.escape(widget['class_ref'])}'"
        f" data-role='{html.escape(widget['role'])}'"
        " data-frog-visual-law='wfrog-realization-state-map'"
        f" data-frog-browse-visible='{'true' if browse_visible else 'false'}'"
        f" data-asset-route='{html.escape(asset_route)}'"
    )
    style = (
        f"position:absolute;left:{layout_int(layout, 'x', 0)}px;top:{layout_int(layout, 'y', 0)}px;"
        f"width:{layout_int(layout, 'width', 300)}px;height:{layout_int(layout, 'height', 120)}px;"
        f"--frog-path-caption-color:{html.escape(safe_css_color(runtime.get('caption.style.text_color'), '#111827'))};"
        f"--frog-path-caption-font-size:{html.escape(safe_css_length(runtime.get('caption.style.font_size'), '14px'))};"
        f"--frog-path-caption-font-weight:{html.escape(safe_css_font_weight(runtime.get('caption.style.font_weight'), '600'))};"
        f"--frog-path-caption-font-family:{html.escape(safe_css_font_family(runtime.get('caption.style.font_family'), 'system-ui, Segoe UI, Arial, sans-serif'))};"
        f"--frog-path-button-fill:{html.escape(safe_css_color(runtime.get('style.browse_button.fill_color'), '#f8fafc'))};"
        f"--frog-path-button-fill-hover:{html.escape(safe_css_color(runtime.get('style.browse_button.fill_color.hover'), '#e5eef9'))};"
    )
    caption_overlay = (
        "<span class='path-caption-overlay' data-frog-part='caption' data-svg-anchor='caption.anchor'"
        f" style='{caption_anchor_style(runtime, geometry)}'>{caption}</span>"
    )
    skin = render_path_skin(asset_path, runtime)
    if is_control:
        value_overlay = (
            f"<input id='{html.escape(widget['widget_id'])}_value' name='{html.escape(input_id)}' type='text'"
            " class='path-value-overlay path-control-editor' data-frog-part='path_display' data-svg-anchor='path_display.left_center'"
            f" data-frog-input-id='{html.escape(input_id)}'"
            f" style='{value_style}color:{text_color};font-size:{text_size};font-weight:{text_weight};padding:0 {text_padding};line-height:{text_line_height};transform:translateY({text_baseline});'"
            f" value='{value}'{' disabled' if not interaction_enabled else ''}>"
        )
        value_overlay += (
            f"<input id='{html.escape(widget['widget_id'])}_file_picker' type='file' class='path-file-picker' tabindex='-1' aria-hidden='true'"
            f" onchange=\"frogPathPicked(this,'{html.escape(widget['widget_id'])}_value')\">"
        )
        if browse_visible:
            value_overlay += (
                f"<label for='{html.escape(widget['widget_id'])}_file_picker' class='path-browse-overlay' data-frog-part='browse_button'"
                f" aria-label='Browse {caption}' style='{browse_style}'></label>"
            )
    else:
        value_overlay = (
            "<output class='path-value-overlay path-indicator-value' data-frog-part='path_display' data-svg-anchor='path_display.left_center'"
            f" style='{value_style}color:{text_color};font-size:{text_size};font-weight:{text_weight};padding:0 {text_padding};line-height:{text_line_height};transform:translateY({text_baseline});'>{value}</output>"
        )
    return f"<section class='frog-widget path-widget {'path-control' if is_control else 'path-indicator'}'{attrs} style='{style}'>{skin}{caption_overlay}{value_overlay}</section>"


class PathBrowserUiRuntime:
    def __init__(
        self,
        *,
        contract_path: str | Path | None = None,
        wfrog_path: str | Path | None = None,
        host: str = "127.0.0.1",
        port: int = 0,
        open_browser: bool = True,
        native_kernel_bridge: NativeStringKernelBridge | None = None,
    ) -> None:
        self.runtime = PathRuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
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

    def run_once(self, input_value: str) -> dict[str, Any]:
        if self.native_kernel_bridge is None:
            return self.runtime.execute(input_value)
        return self.runtime.execute_with_native_kernel_bridge(self.native_kernel_bridge, input_value)

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
        rendered_widgets = "".join(
            render_path_widget(widget, self.runtime.asset_map.get(str(widget["runtime"].get("asset_ref", "")).split(":", 1)[-1]))
            for widget in widgets
        )
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
.path-widget{{font-family:Segoe UI,Arial,sans-serif;}}
.path-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;}}
.path-skin svg{{width:100%;height:100%;display:block;}}
.path-skin #label_text,.path-skin #caption_text,.path-skin #path_display{{display:none;}}
.path-caption-overlay{{position:absolute;transform:translateY(-50%);font-size:var(--frog-path-caption-font-size);font-weight:var(--frog-path-caption-font-weight);font-family:var(--frog-path-caption-font-family);color:var(--frog-path-caption-color);line-height:1;white-space:nowrap;pointer-events:none;}}
.path-value-overlay{{position:absolute;box-sizing:border-box;font-family:Segoe UI,Arial,sans-serif;line-height:1;border:0;background:transparent;margin:0;}}
.path-control-editor{{outline:0;appearance:none;-webkit-appearance:none;}}
.path-control-editor:focus{{outline:0;}}
.path-indicator-value{{display:flex;align-items:center;pointer-events:none;overflow:hidden;white-space:nowrap;text-overflow:ellipsis;}}
.path-file-picker{{position:absolute;width:1px;height:1px;opacity:0;pointer-events:none;}}
.path-browse-overlay{{position:absolute;box-sizing:border-box;cursor:pointer;background:transparent;border:0;}}
.path-control:has(.path-control-editor:hover) .path-skin #path_face{{fill:var(--frog-path-face-fill-hover) !important;stroke:var(--frog-path-face-stroke-hover) !important;}}
.path-control:has(.path-browse-overlay:hover) .path-skin #browse_button{{fill:var(--frog-path-button-fill-hover) !important;stroke:var(--frog-path-button-stroke-hover) !important;}}
.actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}
button{{padding:8px 14px;border:0;border-radius:6px;cursor:pointer;background:#0f62fe;color:#ffffff;font-weight:600;}}
.diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}
.diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}
</style>
<script>
function frogPathPicked(input,targetId){{const target=document.getElementById(targetId);if(!target){{return;}}if(input.files&&input.files.length>0){{target.value=input.files[0].name;target.dispatchEvent(new Event('input',{{bubbles:true}}));target.dispatchEvent(new Event('change',{{bubbles:true}}));}}}}
</script>
</head>
<body>
<h1>{html.escape(panel['title'])}</h1>
<p class="meta">Example 09 - .frog front panel + Default Path .wfrog realization assets + Python runtime</p>
<dl class="runtime-facts" aria-label="Runtime facts">
  <div><dt>Runtime</dt><dd>Python reference runtime</dd></div>
  <div><dt>Execution</dt><dd>{'native kernel bridge' if uses_native_kernel else 'path contract executor'}</dd></div>
  <div><dt>Compiler backend</dt><dd>{'LLVM native path kernel artifact' if uses_native_kernel else 'none for Example 09'}</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(panel['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="{'llvm' if uses_native_kernel else 'none'}" data-execution-path="{'native_kernel_bridge' if uses_native_kernel else 'python_path_contract_executor'}" style="width:{panel_layout['width']}px;height:{panel_layout['height']}px;">
    {rendered_widgets}
  </div>
  <div class="actions"><button type="submit">Run Example 09</button><a class="state-link" href="/state.json">state.json</a></div>
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
        control_values = {key: values[0] for key, values in form.items() if key.startswith("input_path")}
        try:
            if self.native_kernel_bridge is None:
                self.runtime.execute_all(control_values)
            else:
                self.runtime.execute_all_with_native_kernel_bridge(self.native_kernel_bridge, control_values)
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


class ButtonBrowserUiRuntime:
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
        self.runtime = ButtonRuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
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

    def run_once(self, trigger_pressed: bool) -> dict[str, Any]:
        if self.native_kernel_bridge is None:
            artifact = self.runtime.execute(trigger_pressed)
        else:
            artifact = self.runtime.execute_with_native_kernel_bridge(self.native_kernel_bridge, trigger_pressed)
        self.last_error = None
        return artifact

    def state_snapshot(self) -> dict[str, Any]:
        return self.runtime.execution_artifact()

    def render_html(self) -> str:
        snapshot = self.state_snapshot()
        panel = snapshot["ui_runtime"]["panel"]
        widgets = {entry["widget_id"]: entry for entry in snapshot["ui_runtime"]["widgets"]}
        panel_layout = panel["layout"]
        panel_width = layout_int(panel_layout, "width", 520)
        panel_height = layout_int(panel_layout, "height", 180)
        error_block = ""
        if self.last_error:
            error_block = "<div class='diagnostic error'>" + html.escape(self.last_error) + "</div>"
        uses_native_kernel = self.native_kernel_bridge is not None
        button_html = render_button_widget(widgets["trigger_button"], self.runtime.widgets["trigger_button"]["asset_path"])
        indicator_html = render_boolean_widget(widgets["pressed_indicator"])

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
.button-widget{{overflow:visible;}}
.button-skin{{position:absolute;inset:0;width:100%;height:100%;display:block;z-index:1;}}
.button-skin svg{{width:100%;height:100%;display:block;}}
.button-skin [data-frog-part='label'],.button-skin [data-frog-part='caption'],.button-skin [data-frog-part='state_text']{{display:none!important;}}
.button-skin [data-frog-part='frame']{{fill:var(--frog-button-frame-fill)!important;stroke:var(--frog-button-frame-stroke)!important;stroke-width:var(--frog-button-frame-stroke-width)!important;}}
.button-skin [data-frog-part='face']{{fill:var(--frog-button-face-fill)!important;stroke:var(--frog-button-face-stroke)!important;stroke-width:var(--frog-button-face-stroke-width)!important;transition:fill var(--frog-button-transition),stroke var(--frog-button-transition),transform var(--frog-button-transition);}}
.button-skin [data-frog-part='state_face']{{fill:var(--frog-button-state-face-fill)!important;stroke:var(--frog-button-state-face-stroke)!important;stroke-width:var(--frog-button-state-face-stroke-width)!important;transition:fill var(--frog-button-transition),stroke var(--frog-button-transition),transform var(--frog-button-transition);}}
.button-skin [data-frog-part='focus_ring']{{display:none!important;stroke:var(--frog-button-focus-color)!important;stroke-width:var(--frog-button-focus-width)!important;}}
.button-widget:has(.button-press-overlay:hover) .button-skin [data-frog-part='face']{{fill:var(--frog-button-face-hover-fill)!important;}}
.button-widget:has(.button-press-overlay:hover) .button-skin [data-frog-part='state_face']{{fill:var(--frog-button-state-face-hover-fill)!important;stroke:var(--frog-button-state-face-hover-stroke)!important;}}
.button-widget:has(.button-press-overlay:active) .button-skin [data-frog-part='face']{{fill:var(--frog-button-face-pressed-fill)!important;transform:translateY(var(--frog-button-pressed-inset));}}
.button-widget:has(.button-press-overlay:active) .button-skin [data-frog-part='state_face']{{fill:var(--frog-button-state-face-pressed-fill)!important;stroke:var(--frog-button-state-face-pressed-stroke)!important;transform:translateY(var(--frog-button-pressed-inset));}}
.button-widget:has(.button-press-overlay:focus-visible) .button-skin [data-frog-part='focus_ring']{{display:inline!important;}}
.button-caption-overlay{{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:var(--frog-button-caption-font-size);font-weight:var(--frog-button-caption-font-weight);font-family:var(--frog-button-caption-font-family);line-height:1;white-space:nowrap;pointer-events:none;z-index:3;}}
.button-state-overlay{{position:absolute;transform:translate(-50%,-50%);font-size:var(--frog-button-state-text-font-size);font-weight:var(--frog-button-state-text-font-weight);line-height:1;color:var(--frog-button-state-text-fill);pointer-events:none;z-index:4;max-width:70%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}}
.button-press-overlay{{position:absolute;box-sizing:border-box;margin:0;padding:0;border:0;background:transparent;cursor:pointer;appearance:none;z-index:5;}}
.button-press-overlay:focus,.button-press-overlay:focus-visible,.button-press-overlay:active{{outline:0;box-shadow:none;}}
.boolean-widget{{border:0;padding:0;background:transparent;font:inherit;color:inherit;overflow:visible;}}
.boolean-indicator{{pointer-events:none;}}
.boolean-skin{{position:absolute;inset:0;width:100%;height:100%;object-fit:fill;display:block;pointer-events:none;z-index:2;}}
.boolean-caption-overlay{{position:absolute;left:0;top:0;transform:translateY(-50%);text-align:left;font-size:var(--boolean-caption-font-size);font-weight:var(--boolean-caption-font-weight);font-family:var(--boolean-caption-font-family);line-height:1;color:var(--boolean-caption-color);white-space:nowrap;pointer-events:none;z-index:3;}}
.boolean-state-face{{position:absolute;left:var(--boolean-inner-left);top:var(--boolean-inner-top);width:var(--boolean-inner-width);height:var(--boolean-inner-height);border:var(--boolean-inner-border-width) solid var(--boolean-inner-border);border-radius:7px;background:var(--boolean-fill);box-shadow:inset 0 1px 0 rgba(255,255,255,.6),0 1px 2px rgba(15,23,42,.16);transition:background var(--boolean-transition),border-color var(--boolean-transition),box-shadow var(--boolean-transition),transform var(--boolean-transition);z-index:1;}}
.boolean-widget[data-realization-variant='circular'] .boolean-state-face{{border-radius:50%;}}
.boolean-widget[data-frog-frame-visible='false'] .boolean-state-face{{box-shadow:none;}}
.boolean-state-overlay{{position:absolute;transform:translate(-50%,-50%);text-align:center;font-size:var(--boolean-text-font-size);font-weight:var(--boolean-text-font-weight);line-height:1;color:var(--boolean-text);pointer-events:none;z-index:4;white-space:nowrap;}}
.actions{{margin-top:16px;display:flex;gap:12px;align-items:center;}}
.state-link{{font-size:16px;}}
.diagnostic{{margin:12px 0;padding:10px 12px;border-radius:6px;}}
.diagnostic.error{{background:#fff1f2;color:#9f1239;border:1px solid #fecdd3;}}
</style>
</head>
<body>
<h1>{html.escape(panel['title'])}</h1>
<p class="meta">Example 10 - .frog front panel + Default Button/Boolean .wfrog realization assets + Python runtime</p>
<dl class="runtime-facts" aria-label="Runtime facts">
  <div><dt>Runtime</dt><dd>Python reference runtime</dd></div>
  <div><dt>Execution</dt><dd>{'native kernel bridge' if uses_native_kernel else 'button contract executor'}</dd></div>
  <div><dt>Compiler backend</dt><dd>{'LLVM native Button bool kernel artifact' if uses_native_kernel else 'none for Example 10'}</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(panel['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="{'llvm' if uses_native_kernel else 'none'}" data-execution-path="{'native_kernel_bridge' if uses_native_kernel else 'python_button_contract_executor'}" style="width:{panel_width}px;height:{panel_height}px;">
    {button_html}
    {indicator_html}
  </div>
  <div class="actions"><a class="state-link" href="/state.json">state.json</a></div>
</form>
{button_press_to_boolean_script()}
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
        if parsed.path not in {"/run", "/event"}:
            self._serve_bytes(handler, b"not found", "text/plain; charset=utf-8", status=404)
            return
        length = int(handler.headers.get("Content-Length", "0"))
        body = handler.rfile.read(length).decode("utf-8")
        form = urllib.parse.parse_qs(body, keep_blank_values=True)
        raw_value = form.get("trigger_pressed", ["false"])[0]
        try:
            artifact = self.run_once(parse_bool_input(raw_value))
        except Exception as exc:  # pragma: no cover
            self.last_error = str(exc)
            artifact = self.state_snapshot()
        if parsed.path == "/event":
            self._serve_bytes(handler, json.dumps(artifact, indent=2).encode("utf-8"), "application/json; charset=utf-8", status=200 if self.last_error is None else 500)
            return
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
    native_kernel_bridge: NativeKernelBridge | NativeBoolKernelBridge | NativeStringKernelBridge | NativeEnumKernelBridge | None = None,
) -> BrowserUiRuntime | BooleanBrowserUiRuntime | StringBrowserUiRuntime | EnumBrowserUiRuntime | PathBrowserUiRuntime | ButtonBrowserUiRuntime:
    if wants_example06(example) or is_example06_contract(contract_path):
        return BooleanBrowserUiRuntime(
            contract_path=contract_path or default_example06_contract_path(),
            wfrog_path=wfrog_path or default_example06_wfrog_path(),
            host=host,
            port=port,
            open_browser=open_browser,
            native_kernel_bridge=native_kernel_bridge if isinstance(native_kernel_bridge, NativeBoolKernelBridge) else None,
        )
    if wants_example07(example) or is_example07_contract(contract_path):
        return StringBrowserUiRuntime(
            contract_path=contract_path or default_example07_contract_path(),
            wfrog_path=wfrog_path or default_example07_wfrog_path(),
            host=host,
            port=port,
            open_browser=open_browser,
            native_kernel_bridge=native_kernel_bridge if isinstance(native_kernel_bridge, NativeStringKernelBridge) else None,
        )
    if wants_example08(example) or is_example08_contract(contract_path):
        return EnumBrowserUiRuntime(
            contract_path=contract_path or default_example08_contract_path(),
            wfrog_path=wfrog_path or default_example08_wfrog_path(),
            host=host,
            port=port,
            open_browser=open_browser,
            native_kernel_bridge=native_kernel_bridge if isinstance(native_kernel_bridge, NativeEnumKernelBridge) else None,
        )
    if wants_example09(example) or is_example09_contract(contract_path):
        return PathBrowserUiRuntime(
            contract_path=contract_path or default_example09_contract_path(),
            wfrog_path=wfrog_path or default_example09_wfrog_path(),
            host=host,
            port=port,
            open_browser=open_browser,
            native_kernel_bridge=native_kernel_bridge if isinstance(native_kernel_bridge, NativeStringKernelBridge) else None,
        )
    if wants_example10(example) or is_example10_contract(contract_path):
        return ButtonBrowserUiRuntime(
            contract_path=contract_path or default_example10_contract_path(),
            wfrog_path=wfrog_path or default_example10_wfrog_path(),
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
