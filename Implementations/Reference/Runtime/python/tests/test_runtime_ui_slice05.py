from __future__ import annotations

import http.client
import json
from pathlib import Path
import urllib.parse

from Implementations.Reference.Runtime.python.runtime_core import find_repo_root
from Implementations.Reference.Runtime.python.ui_runtime import build_runtime


def _repo_root() -> Path:
    return find_repo_root(Path(__file__).resolve())


def _load_acceptance() -> dict:
    path = _repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json"
    return json.loads(path.read_text(encoding="utf-8"))


def _resolve_repo_path(relative_path: str) -> Path:
    return _repo_root() / relative_path


def test_slice05_ui_routes_and_state_match_shared_acceptance() -> None:
    acceptance = _load_acceptance()
    contract_path = _resolve_repo_path(acceptance["artifact_refs"]["contract_path"])
    wfrog_path = _resolve_repo_path(acceptance["artifact_refs"]["wfrog_path"])
    snapshot_path = _resolve_repo_path(acceptance["artifact_refs"]["snapshot_path"])
    expected_snapshot = json.loads(snapshot_path.read_text(encoding="utf-8"))

    runtime = build_runtime(
        contract_path=contract_path,
        wfrog_path=wfrog_path,
        host="127.0.0.1",
        port=0,
        open_browser=False,
    )
    httpd, thread = runtime.serve_in_thread()
    try:
        host, port = httpd.server_address
        connection = http.client.HTTPConnection(host, port, timeout=5)

        connection.request("GET", "/")
        response = connection.getresponse()
        html = response.read().decode("utf-8")
        assert response.status == 200
        assert "Python reference runtime" in html
        assert "contract executor" in html
        assert "none in runtime path" in html
        assert 'class="front-panel"' in html
        assert 'data-panel-id="main_panel"' in html
        assert 'data-coordinate-space="panel_pixels"' in html
        assert 'data-runtime-language="python"' in html
        assert 'data-compiler-backend="none"' in html
        assert 'style="width:500px;height:170px;"' in html
        assert "data-widget-id='ctrl_input'" in html
        assert "data-widget-id='ind_result'" in html
        assert "data-class-ref='frog.widgets.numeric_control'" in html
        assert "data-class-ref='frog.widgets.numeric_indicator'" in html
        assert "data-frog-visual-law='wfrog-realization-state-map'" in html
        assert "data-asset-route='/asset/numeric_rectangular_svg'" in html
        assert "left:20px;top:24px;width:220px;height:88px;" in html
        assert "left:260px;top:24px;width:220px;height:88px;" in html
        assert "class='numeric-skin'" in html
        assert "<svg" in html
        assert "data-frog-part='caption'" in html
        assert "data-svg-anchor='caption.anchor'" in html
        assert "data-frog-part='text_value'" in html
        assert "data-svg-anchor='text_value.center'" in html
        assert "data-frog-part='increment_up'" in html
        assert "data-frog-method='increment'" in html
        assert "data-frog-part='increment_down'" in html
        assert "data-frog-method='decrement'" in html
        assert "data-frog-button-state-law='normal-pressed'" in html
        assert "--frog-numeric-frame-fill:transparent" in html
        assert "--frog-numeric-frame-stroke:transparent" in html
        assert "--frog-numeric-frame-stroke-width:2px" in html
        assert "--frog-numeric-step-fill:#5B9BD5" in html
        assert "--frog-numeric-step-fill-pressed:#2B4F7B" in html
        assert "--frog-numeric-caption-font-size:12px;" in html
        assert "--frog-numeric-caption-font-weight:400;" in html
        assert "--frog-numeric-text-color:#111827;" in html
        assert "--frog-numeric-text-font-size:11px;" in html
        assert "font-size: var(--frog-numeric-caption-font-size)" in html
        assert "font-size: 12px;\n  line-height: 1;\n  white-space: nowrap;" not in html
        assert "Current runtime snapshot" not in html
        assert "<pre>" not in html
        for route in acceptance["ui"]["expected_routes"]:
            if route in {"/", "/run"}:
                continue
            assert route in html

        connection.request("GET", "/asset/numeric_rectangular_svg")
        asset_response = connection.getresponse()
        asset_body = asset_response.read().decode("utf-8")
        assert asset_response.status == 200
        assert "<svg" in asset_body
        assert 'viewBox="0 0 380 150"' in asset_body
        assert 'id="caption_text"' in asset_body
        assert 'id="frame"' in asset_body
        assert 'width="222"' in asset_body
        assert 'id="value_face"' in asset_body
        assert 'id="increment_up"' in asset_body
        assert 'id="increment_down"' in asset_body
        assert 'data-frog-states="normal pressed"' in asset_body
        assert 'data-frog-state-law="normal-pressed-color-only"' in asset_body
        assert 'data-frog-bind="style.increment_button.*"' in asset_body

        body = urllib.parse.urlencode({"input_value": str(acceptance["headless"]["input_value"])})
        connection.request(
            "POST",
            "/run",
            body=body,
            headers={"Content-Type": "application/x-www-form-urlencoded"},
        )
        post_response = connection.getresponse()
        post_response.read()
        assert post_response.status == 303

        connection.request("GET", "/state.json")
        state_response = connection.getresponse()
        state = json.loads(state_response.read().decode("utf-8"))
        assert state_response.status == 200
        assert state == expected_snapshot
    finally:
        httpd.shutdown()
        thread.join(timeout=2)
        httpd.server_close()
