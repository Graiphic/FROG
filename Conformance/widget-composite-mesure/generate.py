# SPDX-License-Identifier: Apache-2.0
"""Generate the editable Studio draft witness from the adjacent package."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
raw = (ROOT / "Mesure.wfrog").read_bytes()
package = json.loads(raw)
metadata = package["package"]
reference = f"wfrog:local/{metadata['id']}@{metadata['version']}#{hashlib.sha256(raw).hexdigest()}"
composite = package["composite_classes"][0]


def box(x, y, width=60, height=30):
    return dict(x=x, y=y, width=width, height=height)


def node(identity, operation, x, y, **fields):
    return dict(id=identity, kind="primitive", type="frog.studio." + operation,
                palette_catalog_id=operation, layout=box(x, y), **fields)


def edge(identity, source, outlet, target, inlet):
    return {"id": "wire-" + identity, "from": {"node": source, "port": outlet},
            "to": {"node": target, "port": inlet}}


def widget_node(part, x, y):
    widget = "widget-1." + part
    return dict(id=widget + "_value", kind="widget_value", type=types[part],
                widget=widget, layout=box(x, y))


widgets = []
types = {}
for part in composite["children"]:
    identity = "widget-1." + part["id"]
    types[part["id"]] = part["type"]
    binding = next(item for item in composite["bindings"] if item["part"] == part["id"])
    props = {}
    if part["action"]:
        props = {"behavior.mechanical_action": part["action"],
                 "state_text.false_text": part["label"], "state_text.true_text": part["label"],
                 "state_text.visible": True}
    elif part["type"] == "f64":
        props = {"data_type.representation": "f64", "representation.kind": "double"}
    widgets.append(dict(id=identity, classId=part["class"], role=part["role"], valueType=part["type"],
                        label=f"{metadata['name']} / {binding['id']}",
                        bounds=part["bounds"] | {"x": part["bounds"]["x"] + 32, "y": part["bounds"]["y"] + 64},
                        props=props, state={"value": False if part["type"] == "bool" else 0},
                        runtime={"wfrog": reference, "realization": part["realization"], "asset": part["asset"]}))

event = dict(id="event", kind="structure", type="frog.studio.structures.event_structure",
             palette_catalog_id="structures.event_structure", structure_type="event_structure",
             layout=box(106, 76, 600, 340), boundary={"inputs": [], "outputs": []},
             structure_terminals={
                 "timeout": dict(type="i64", outer_visible=True, exposed_in_body=True, read_only=True,
                                 outer_port="timeout", body_port="timeout_value", role="timeout_milliseconds"),
                 "event_data": dict(outer_visible=False, exposed_in_body=True, read_only=True, role="event_data",
                                    fields=[dict(id="source", type="string", body_port="event_data.source"),
                                            dict(id="type", type="string", body_port="event_data.type"),
                                            dict(id="time", type="u64", body_port="event_data.time")])},
             regions=[dict(id="pressed", event=dict(kind="value_change", source="widget-1.button",
                               studio_bindings=[dict(kind="value_change", source="widget-1.button")]),
                           event_label='"Mesure / activation": Value Change', event_data_fields=["new_value"],
                           diagram=dict(
                               nodes=[node("high", "numeric.numeric_constant", 200, 120, value_type="f64", text="24.68"),
                                      node("low", "numeric.numeric_constant", 200, 242, value_type="f64", text="12.34"),
                                      node("select", "structures.select", 366, 186),
                                      widget_node("display", 540, 190), widget_node("led", 540, 276)],
                               edges=[edge("high", "high", "value", "select", "true"),
                                      edge("low", "low", "value", "select", "false"),
                                      edge("choice", "event", "event_data.new_value", "select", "select"),
                                      edge("display", "select", "result", "widget-1.display_value", "value"),
                                      edge("led", "event", "event_data.new_value", "widget-1.led_value", "value")]))])

loop = dict(id="loop", kind="structure", type="frog.studio.structures.while_loop",
            palette_catalog_id="structures.while_loop", structure_type="while_loop",
            layout=box(56, 42, 820, 480), boundary={"inputs": [], "outputs": []},
            structure_terminals={
                "condition": dict(type="bool", outer_visible=False, exposed_in_body=True, read_only=False,
                                  body_port="condition", role="stop_if_true"),
                "index": dict(type="i64", outer_visible=False, exposed_in_body=True, read_only=True,
                              body_port="iteration", role="index")},
            while_loop_stop_if_true=True,
            regions=[dict(id="body", diagram=dict(
                nodes=[event, node("continue", "boolean.false_constant", 741, 438, value_type="bool", text="false")],
                edges=[edge("continue", "continue", "value", "loop", "condition")]))])

document = dict(format="frog.document.draft", draft_revision=3, language="FROG", title="Mesure",
                frontPanel={"grid": {"pitch": 8}, "widgets": widgets,
                            "groups": [{"id": "group-1", "object_ids": [w["id"] for w in widgets]}]},
                diagram={"nodes": [loop, widget_node("button", 54, 575)], "edges": []})
(ROOT / "Mesure.frog").write_text(json.dumps(document, ensure_ascii=False, indent=2) + "\n",
                                 encoding="utf-8", newline="\n")
