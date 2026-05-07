from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
WIDGET_DOC = ROOT / "Libraries" / "Widgets" / "Chart.md"
MANIFEST = ROOT / "Libraries" / "Realizations" / "Default" / "chart.default.wfrog"
DEFAULT_DIR = ROOT / "Libraries" / "Realizations" / "Default"

EXPECTED_PARTS = {
    "root", "label", "caption", "frame", "plot_area", "history_region", "plot_layer",
    "plot_polyline", "plot_marker", "grid", "axis_tick", "x_scale", "y_scale",
    "x_scale_label", "y_scale_label", "plot_legend", "scale_legend", "cursor_legend",
    "graph_palette", "x_scrollbar", "y_scrollbar", "cursor_layer", "cursor_readout",
    "plot_image_back", "plot_image_middle", "plot_image_front", "annotation_layer",
    "threshold_band", "focus_ring",
}


def test_chart_widget_doc_defines_deepened_public_surface() -> None:
    text = WIDGET_DOC.read_text(encoding="utf-8")
    for token in [
        "frog.widgets.waveform_chart",
        "frog.waveform_chart_value",
        "samples.timestamp_policy",
        "timebase.mode",
        "history.retention_policy",
        "plots[].interpolation",
        "cursors[].snap_mode",
        "thresholds.items",
        "annotation_layer",
        "cursor_readout",
        "plot_polyline",
    ]:
        assert token in text


def test_chart_manifest_declares_expected_parts_and_bindings() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    realization = data["realizations"][0]
    assert set(realization["supported_parts"]) == EXPECTED_PARTS
    assert "waveform" in realization["supported_payload_kinds"]
    assert "relative_time" in realization["supported_timebase_modes"]
    assert "nearest_sample" in realization["supported_cursor_snap_modes"]

    property_members = {item["member"] for item in data["property_bindings"]}
    assert "history.*" in property_members
    assert "samples.*" in property_members
    assert "timebase.*" in property_members
    assert "thresholds.*" in property_members
    assert "annotations.*" in property_members


def test_chart_svg_resources_expose_all_public_part_markers() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    parts = set()
    for resource in data["resources"]:
        path = DEFAULT_DIR / resource["path"]
        text = path.read_text(encoding="utf-8")
        parts |= set(re.findall(r"data-frog-part=[\"']([^\"']+)[\"']", text))
    assert EXPECTED_PARTS <= parts
