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
        for route in acceptance["ui"]["expected_routes"]:
            if route in {"/", "/run"}:
                continue
            assert route in html

        for asset_route in ("/asset/numeric_control_svg", "/asset/numeric_indicator_svg"):
            connection.request("GET", asset_route)
            asset_response = connection.getresponse()
            asset_body = asset_response.read().decode("utf-8")
            assert asset_response.status == 200
            assert "<svg" in asset_body

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
