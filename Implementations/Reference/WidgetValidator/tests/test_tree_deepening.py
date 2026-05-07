from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
WIDGET_DOC = ROOT / "Libraries" / "Widgets" / "Tree.md"
MANIFEST = ROOT / "Libraries" / "Realizations" / "Default" / "tree.default.wfrog"
DEFAULT_DIR = ROOT / "Libraries" / "Realizations" / "Default"

EXPECTED_PARTS = {
    "root",
    "label",
    "caption",
    "frame",
    "node_region",
    "node_row",
    "node_indent",
    "node_connector",
    "expander_face",
    "checkbox_face",
    "node_icon",
    "node_label",
    "active_node",
    "selection_face",
    "edit_overlay",
    "drag_feedback",
    "drop_target",
    "vertical_scrollbar",
    "horizontal_scrollbar",
    "focus_ring",
}


def test_tree_widget_doc_defines_deepened_public_surface() -> None:
    text = WIDGET_DOC.read_text(encoding="utf-8")
    for token in [
        "frog.widgets.tree_control",
        "frog.widgets.tree_indicator",
        "frog.tree",
        "nodes[].id",
        "nodes[].path",
        "selection.selected_node_ids",
        "expansion.expanded_node_ids",
        "check.indeterminate_node_ids",
        "drag.active_node_ids",
        "drop.target_node_id",
        "node_icon",
        "checkbox_face",
        "drag_feedback",
    ]:
        assert token in text


def test_tree_manifest_declares_expected_parts_and_bindings() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    realization = data["realizations"][0]
    assert set(realization["supported_parts"]) == EXPECTED_PARTS
    assert "single_node" in realization["supported_selection_modes"]
    assert "multi_node" in realization["supported_selection_modes"]
    assert "tri_state" in realization["supported_check_modes"]
    assert "inside" in realization["supported_drop_positions"]

    property_members = {item["member"] for item in data["property_bindings"]}
    assert "nodes.*" in property_members
    assert "selection.*" in property_members
    assert "expansion.*" in property_members
    assert "check.*" in property_members
    assert "drag.*" in property_members
    assert "drop.*" in property_members


def test_tree_svg_resources_expose_all_public_part_markers() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    parts = set()
    for resource in data["resources"]:
        path = DEFAULT_DIR / resource["path"]
        text = path.read_text(encoding="utf-8")
        parts |= set(re.findall(r"data-frog-part=[\\\"']([^\\\"']+)[\\\"']", text))
    assert EXPECTED_PARTS <= parts
