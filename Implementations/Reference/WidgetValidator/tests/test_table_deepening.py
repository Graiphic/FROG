from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
WIDGET_DOC = ROOT / "Libraries" / "Widgets" / "Table.md"
MANIFEST = ROOT / "Libraries" / "Realizations" / "Default" / "table.default.wfrog"
DEFAULT_DIR = ROOT / "Libraries" / "Realizations" / "Default"

EXPECTED_PARTS = {
    "root",
    "label",
    "caption",
    "frame",
    "header_region",
    "column_header_region",
    "row_header_region",
    "grid_region",
    "body_region",
    "row",
    "cell",
    "active_cell",
    "selection_face",
    "edit_overlay",
    "sort_indicator",
    "resize_handle",
    "vertical_scrollbar",
    "horizontal_scrollbar",
    "focus_ring",
}


def test_table_widget_doc_defines_deepened_public_surface() -> None:
    text = WIDGET_DOC.read_text(encoding="utf-8")
    for token in [
        "frog.widgets.table_control",
        "frog.widgets.table_indicator",
        "frog.table",
        "columns.schema",
        "selection.active_cell",
        "editing.commit_policy",
        "viewport.row_offset",
        "sort.keys",
        "filter.expression",
        "active_cell",
        "edit_overlay",
        "resize_handle",
    ]:
        assert token in text


def test_table_manifest_declares_expected_parts_and_bindings() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    realization = data["realizations"][0]
    assert set(realization["supported_parts"]) == EXPECTED_PARTS
    assert "cell" in realization["supported_selection_modes"]
    assert "range" in realization["supported_selection_modes"]
    assert "ascending" in realization["supported_sort_directions"]

    property_members = {item["member"] for item in data["property_bindings"]}
    assert "columns.schema" in property_members
    assert "cells.*" in property_members
    assert "selection.*" in property_members
    assert "editing.*" in property_members
    assert "sort.*" in property_members
    assert "filter.*" in property_members


def test_table_svg_resources_expose_all_public_part_markers() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    parts = set()
    for resource in data["resources"]:
        path = DEFAULT_DIR / resource["path"]
        text = path.read_text(encoding="utf-8")
        parts |= set(re.findall(r"data-frog-part=[\\\"']([^\\\"']+)[\\\"']", text))
    assert EXPECTED_PARTS <= parts
