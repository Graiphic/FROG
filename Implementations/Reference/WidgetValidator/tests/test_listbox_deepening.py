from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
WIDGET_DOC = ROOT / "Libraries" / "Widgets" / "Listbox.md"
MANIFEST = ROOT / "Libraries" / "Realizations" / "Default" / "listbox.default.wfrog"
DEFAULT_DIR = ROOT / "Libraries" / "Realizations" / "Default"

EXPECTED_PARTS = {
    "root",
    "placement_bounds",
    "main_body",
    "label",
    "caption",
    "item_region",
    "item_row",
    "item_icon",
    "item_label",
    "selection_face",
    "hover_face",
    "disabled_face",
    "vertical_scrollbar",
    "horizontal_scrollbar",
    "focus_ring",
}


def test_listbox_widget_doc_defines_deepened_public_surface() -> None:
    text = WIDGET_DOC.read_text(encoding="utf-8")
    for token in [
        "frog.widgets.listbox_control",
        "frog.widgets.listbox_indicator",
        "frog.listbox_selection",
        "items[].id",
        "items[].icon",
        "selection.selected_ids",
        "active.index",
        "hover.index",
        "viewport.item_offset",
        "search.query",
        "filter.expression",
        "placement_bounds",
        "main_body",
    ]:
        assert token in text


def test_listbox_manifest_declares_expected_parts_and_bindings() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    realization = data["realizations"][0]
    assert set(realization["supported_parts"]) == EXPECTED_PARTS
    assert "single" in realization["supported_selection_modes"]
    assert "multiple" in realization["supported_selection_modes"]
    assert "range" in realization["supported_selection_modes"]
    assert "icon" in realization["supported_item_features"]

    property_members = {item["member"] for item in data["property_bindings"]}
    assert "items.*" in property_members
    assert "items.icon" in property_members
    assert "selection.*" in property_members
    assert "hover.*" in property_members
    assert "filter.*" in property_members


def test_listbox_svg_resources_expose_all_public_part_markers() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    parts = set()
    for resource in data["resources"]:
        path = DEFAULT_DIR / resource["path"]
        text = path.read_text(encoding="utf-8")
        parts |= set(re.findall(r"data-frog-part=[\\\"']([^\\\"']+)[\\\"']", text))
    assert EXPECTED_PARTS <= parts
