from __future__ import annotations

import html
import json
import threading
import urllib.parse
import webbrowser
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any, Dict, Optional

try:
    from .runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
except ImportError:  # pragma: no cover
    from runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path


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


def build_runtime(
    *,
    contract_path: str | Path | None = None,
    wfrog_path: str | Path | None = None,
    host: str = "127.0.0.1",
    port: int = 0,
    open_browser: bool = True,
) -> BrowserUiRuntime:
    return BrowserUiRuntime(
        contract_path=contract_path or default_contract_path(),
        wfrog_path=wfrog_path or default_wfrog_path(),
        host=host,
        port=port,
        open_browser=open_browser,
    )
