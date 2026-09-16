from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from Implementations.Reference.Runtime import boolean_runtime
from Implementations.Reference.Runtime.boolean_runtime import (
    BooleanRuntimeModelError,
    merge_boolean_realization_defaults,
)
from Implementations.Reference.Runtime.python.ui_runtime import build_runtime


INDICATOR = "frog.widgets.boolean_indicator"
CONTROL = "frog.widgets.boolean_control"


@pytest.mark.parametrize(
    ("package_props", "instance_props", "caption_size"),
    [({}, {}, "19px"), ({"caption.style.font_size": "23px"}, {}, "23px"),
     ({"caption.style.font_size": "23px"}, {"caption.style.font_size": "29px"}, "29px")],
)
def test_realization_package_instance_precedence_preserves_source(
    tmp_path: Path, package_props: dict, instance_props: dict, caption_size: str,
) -> None:
    manifest = {"default_widget_properties": [{
        "class_id": INDICATOR, "role": "indicator",
        "props": {"caption.style.font_size": "19px", "state_text.style.font_size": "21px",
                  "state_text.visible": True, "caption.anchor.x": 16, "value": True},
        "visual": {"asset_ref": "asset:manifest"},
    }]}
    path = tmp_path / "boolean.wfrog"
    path.write_text(json.dumps(manifest), encoding="utf-8")
    package = {
        "realization_refs": [{"package_ref": path.name, "target_classes": [INDICATOR]}],
        "default_widget_properties": [{"class_id": INDICATOR, "role": "indicator", "props": package_props}],
    }
    panel = {"widgets": [{
        "instance_id": "status", "class_ref": INDICATOR, "role": "indicator",
        "props": {**instance_props, "state_text.visible": False, "caption.anchor.x": 0, "value": False},
        "visual": {"asset_ref": "asset:instance"},
    }]}
    original_panel, original_package = deepcopy(panel), deepcopy(package)

    merged = merge_boolean_realization_defaults(panel, package, tmp_path)
    widget = merged["widgets"][0]
    assert widget["props"]["caption.style.font_size"] == caption_size
    assert widget["props"]["state_text.style.font_size"] == "21px"
    assert widget["props"]["state_text.visible"] is False
    assert widget["props"]["caption.anchor.x"] == 0
    assert widget["props"]["value"] is False
    assert widget["visual"]["asset_ref"] == "asset:instance"
    assert panel == original_panel
    assert package == original_package
    assert json.loads(path.read_text(encoding="utf-8")) == manifest


def test_realization_defaults_respect_reference_targets_and_widget_role(tmp_path: Path) -> None:
    manifest = {"default_widget_properties": [
        {"class_id": INDICATOR, "role": "indicator", "props": {"caption.style.font_size": "19px"}},
        {"class_id": INDICATOR, "role": "control", "props": {"caption.style.font_size": "99px"}},
        {"class_id": CONTROL, "role": "control", "props": {"caption.style.font_size": "77px"}},
        {"class_id": "frog.widgets.button", "props": {"style.button_face.fill_color.false": "#123456"}},
    ]}
    path = tmp_path / "boolean.wfrog"
    path.write_text(json.dumps(manifest), encoding="utf-8")
    package = {"realization_refs": [{"package_ref": path.name, "target_classes": [INDICATOR]}]}
    panel = {"widgets": [
        {"class_ref": INDICATOR, "role": "indicator", "props": {}},
        {"class_ref": CONTROL, "role": "control", "props": {}},
        {"class_ref": "frog.widgets.button", "role": "control", "props": {"style.face.fill_color.false": "#abcdef"}},
    ]}

    widgets = merge_boolean_realization_defaults(panel, package, tmp_path)["widgets"]
    assert widgets[0]["props"] == {"caption.style.font_size": "19px"}
    assert widgets[1]["props"] == {}
    assert widgets[2]["props"] == panel["widgets"][2]["props"]


@pytest.mark.parametrize("example", ["06", "10"])
def test_boolean_realizations_are_resolved_once_before_runtime_events(
    monkeypatch: pytest.MonkeyPatch, example: str,
) -> None:
    loaded_paths = []
    load_json_file = boolean_runtime.load_json_file

    def track_load(path: Path):
        loaded_paths.append(path)
        return load_json_file(path)

    monkeypatch.setattr(boolean_runtime, "load_json_file", track_load)
    browser = build_runtime(example=example, open_browser=False)
    assert len(loaded_paths) == 1
    assert loaded_paths[0].name == "boolean.default.wfrog"
    browser.render_html()
    browser.runtime.execute(True)
    browser.render_html()
    browser.runtime.execute(False)
    assert len(loaded_paths) == 1


@pytest.mark.parametrize("contents", [
    None,
    '{"default_widget_properties":{}}',
    '{"x":1,"x":2}',
    '{"default_widget_properties":[{"class_id":["frog.widgets.boolean_indicator"]}]}',
    '{"default_widget_properties":[{"target_class":["frog.widgets.boolean_indicator"]}]}',
])
def test_invalid_declared_realization_reports_an_error(tmp_path: Path, contents: str | None) -> None:
    path = tmp_path / "boolean.wfrog"
    if contents is not None:
        path.write_text(contents, encoding="utf-8")
    package = {"realization_refs": [{"package_ref": path.name, "target_classes": [INDICATOR]}]}
    with pytest.raises(BooleanRuntimeModelError):
        merge_boolean_realization_defaults({"widgets": []}, package, tmp_path)
