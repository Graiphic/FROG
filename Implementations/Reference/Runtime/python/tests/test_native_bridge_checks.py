"""Keep the Example 05 native proof aligned with its shared acceptance snapshot."""

import copy
import json
from pathlib import Path
import runpy
from types import SimpleNamespace

import pytest


@pytest.fixture
def native_headless_check(monkeypatch):
    runtime_root = Path(__file__).resolve().parents[2]
    namespace = runpy.run_path(
        str(runtime_root / "check_example05_cpp_native_kernel_bridge.py")
    )
    check = namespace["check_native_runtime_headless"]
    snapshot = json.loads(
        (runtime_root / "acceptance" / "example05_input_3.snapshot.json")
        .read_text(encoding="utf-8")
    )
    build_dir = namespace["ROOT"] / "b" / "headless-check-unit-test"
    monkeypatch.setitem(
        check.__globals__, "executable_path",
        lambda directory, name: directory / name,
    )

    def run_check(artifact):
        monkeypatch.setitem(
            check.__globals__, "run",
            lambda command: SimpleNamespace(stdout=json.dumps(artifact)),
        )
        check(build_dir)

    return run_check, snapshot


def test_native_headless_accepts_current_shared_snapshot(native_headless_check):
    check, snapshot = native_headless_check
    check(snapshot)


@pytest.mark.parametrize("obsolete_layout", ["panel", "control", "indicator"])
def test_native_headless_rejects_obsolete_layout(
    native_headless_check, obsolete_layout
):
    check, snapshot = native_headless_check
    artifact = copy.deepcopy(snapshot)
    ui = artifact["ui_runtime"]
    if obsolete_layout == "panel":
        ui["panel"]["layout"]["height"] = 170
    else:
        index = 0 if obsolete_layout == "control" else 1
        ui["widgets"][index]["layout"]["width"] = 220
    with pytest.raises(AssertionError):
        check(artifact)
