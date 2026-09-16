from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
WIDGET_DOC = ROOT / "Libraries" / "Widgets" / "Tab.md"
MANIFEST = ROOT / "Libraries" / "Realizations" / "Default" / "tab.default.wfrog"
DEFAULT_DIR = ROOT / "Libraries" / "Realizations" / "Default"

EXPECTED_PARTS = {
    "root", "label", "caption", "frame", "tab_header_region", "tab_item",
    "tab_item_label", "tab_item_icon", "tab_close_button", "selection_face",
    "overflow_button", "overflow_menu", "tab_scroll_previous", "tab_scroll_next",
    "page_label_display", "page_label_display_face", "page_label_display_text",
    "page_label_display_previous", "page_label_display_next",
    "page_region", "page_container", "page_content", "focus_ring",
}


def test_tab_widget_doc_defines_deepened_public_surface() -> None:
    text = WIDGET_DOC.read_text(encoding="utf-8")
    for token in [
        "frog.widgets.tab_control", "frog.widgets.tab_indicator", "frog.tab_selection",
        "pages[].content_ref", "selection.previous_id", "headers.placement",
        "page_region.active_page_id", "overflow.hidden_page_ids", "reorder.drop_index",
        "close.policy", "page_container", "page_content",
        "page_label_display.selected_page_id", "page_label_display_previous",
        "page_label_display_next",
    ]:
        assert token in text


def test_tab_manifest_declares_expected_parts_and_bindings() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    realization = data["realizations"][0]
    assert set(realization["supported_parts"]) == EXPECTED_PARTS
    assert "top" in realization["supported_header_placements"]
    assert "left" in realization["supported_header_placements"]
    assert "request" in realization["supported_close_policies"]
    property_members = {item["member"] for item in data["property_bindings"]}
    assert "pages.*" in property_members
    assert "selection.*" in property_members
    assert "headers.*" in property_members
    assert "page_label_display.*" in property_members
    assert "page_region.*" in property_members
    assert "overflow.*" in property_members
    assert "close.*" in property_members
    assert "reorder.*" in property_members
    page_label_parts = {part for part in EXPECTED_PARTS if part.startswith("page_label_display")}
    binding_kinds = {item["part"]: item["binding_kind"] for item in data["part_bindings"]}
    for part in page_label_parts:
        assert binding_kinds[part] == "svg_or_host_surface"


def test_tab_svg_resources_expose_all_public_part_markers() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    parts = set()
    for resource in data["resources"]:
        path = DEFAULT_DIR / resource["path"]
        text = path.read_text(encoding="utf-8")
        parts |= set(re.findall(r"data-frog-part=[\"']([^\"']+)[\"']", text))
    assert EXPECTED_PARTS <= parts
