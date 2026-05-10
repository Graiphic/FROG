from __future__ import annotations

import html
import json
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
    from ..contract_executor import execute_contract_case, load_json as load_contract_json
except ImportError:  # pragma: no cover
    sys.path.append(str(Path(__file__).resolve().parents[1]))
    from runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
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

    def execute(self, control_value: bool | None = None) -> dict[str, Any]:
        if control_value is not None:
            self.current_value = bool(control_value)
        return execute_contract_case(
            self.contract,
            {"input_value": self.current_value},
            {"wfrog": self.package},
        )

    def execution_artifact(self) -> dict[str, Any]:
        return self.execute()


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
    ) -> None:
        self.runtime = Slice05RuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
        self.host = host
        self.port = port
        self.open_browser = open_browser
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
        ctrl = widgets["ctrl_input"]["runtime"]
        ind = widgets["ind_result"]["runtime"]
        ctrl_asset = widgets["ctrl_input"]["runtime"].get("asset_ref")
        ind_asset = widgets["ind_result"]["runtime"].get("asset_ref")
        ctrl_asset_url = f"/asset/{ctrl_asset.split(':', 1)[1]}" if ctrl_asset else ""
        ind_asset_url = f"/asset/{ind_asset.split(':', 1)[1]}" if ind_asset else ""

        error_block = ""
        if self.last_error:
            error_block = (
                "<div class='diagnostic error'>"
                + html.escape(self.last_error)
                + "</div>"
            )

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
.panel {{
  width: 460px;
  min-height: 170px;
  background: #ffffff;
  border-radius: 10px;
  padding: 16px;
  box-shadow: 0 4px 14px rgba(15, 23, 42, 0.08);
}}
.widgets {{
  display: flex;
  gap: 24px;
  align-items: flex-start;
}}
.widget {{
  width: 180px;
  padding: 12px;
  border-radius: 8px;
  border: 2px solid #cbd2d9;
  background: #fbfdff;
}}
.widget svg, .widget img {{
  display: block;
  width: 100%;
  height: 32px;
  object-fit: contain;
  margin-bottom: 8px;
}}
.widget label {{
  display: block;
  margin-bottom: 8px;
  font-size: 12px;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.04em;
}}
.widget input, .widget output {{
  display: block;
  width: 100%;
  padding: 8px 10px;
  box-sizing: border-box;
  border-radius: 6px;
  border: 1px solid #9aa5b1;
  font-size: 16px;
}}
.widget output {{
  background: #f8fff0;
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
  <div><dt>Execution</dt><dd>contract executor</dd></div>
  <div><dt>Compiler backend</dt><dd>none in runtime path</dd></div>
</dl>
{error_block}
<div class="panel">
  <form method="post" action="/run">
    <div class="widgets">
      <section class="widget" style="border-color:{html.escape(str(ctrl['foreground_color']))}">
        <label>{html.escape(str(ctrl['label']))}</label>
        <img src="{html.escape(ctrl_asset_url)}" alt="">
        <input name="input_value" type="number" min="0" max="65535" value="{html.escape(str(ctrl['value']))}" {'disabled' if not ctrl['enabled'] else ''}>
      </section>
      <section class="widget" style="border-color:{html.escape(str(ind['foreground_color']))}">
        <label>{html.escape(str(ind['label']))}</label>
        <img src="{html.escape(ind_asset_url)}" alt="">
        <output>{html.escape(str(ind['value']))}</output>
      </section>
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
</div>
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
            self.runtime.execute(control_value=int(raw_value))
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
    ) -> None:
        self.runtime = BooleanRuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
        self.host = host
        self.port = port
        self.open_browser = open_browser
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
  <div><dt>Execution</dt><dd>boolean contract executor</dd></div>
  <div><dt>Compiler backend</dt><dd>none for Example 06</dd></div>
</dl>
{error_block}
<form method="post" action="/run">
  <div class="front-panel" data-panel-id="{html.escape(panel['panel_id'])}" data-coordinate-space="panel_pixels" data-runtime-language="python" data-compiler-backend="none" data-execution-path="python_boolean_contract_executor" style="width:{panel_layout['width']}px;height:{panel_layout['height']}px;">
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
            self.runtime.execute(control_value=parse_bool_input(raw_value))
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
) -> BrowserUiRuntime | BooleanBrowserUiRuntime:
    if wants_example06(example) or is_example06_contract(contract_path):
        return BooleanBrowserUiRuntime(
            contract_path=contract_path or default_example06_contract_path(),
            wfrog_path=wfrog_path or default_example06_wfrog_path(),
            host=host,
            port=port,
            open_browser=open_browser,
        )
    return BrowserUiRuntime(
        contract_path=contract_path or default_contract_path(),
        wfrog_path=wfrog_path or default_wfrog_path(),
        host=host,
        port=port,
        open_browser=open_browser,
    )
