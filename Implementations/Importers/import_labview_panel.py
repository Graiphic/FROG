# SPDX-License-Identifier: Apache-2.0
"""Build native panel widgets from saved DCO identities, data types and defaults."""

from copy import deepcopy
from functools import cache
import json
from pathlib import Path

from pylabview.LVdatafill import DataFillArray, DataFillPath, DataFillSimpleRefnum
from import_labview_interfaces import EmptyMapDefault

from import_labview_heap_graph import (
    HeapTypes, Widget, data_value, flags, implicit_value, inline_panel_default, members, panel_defaults, rect, waveform_display,
)
from import_labview_profile import (
    ProfileRefusal, bounds, field, heap_root, identity, kind, number, require,
)
from import_labview_structured import declaration


ERROR_CLUSTER_TYPE = 'cluster<"status":bool,"code":i32,"source":string>'


@cache
def _error_prototypes():
    prototypes = json.loads(Path(__file__).with_name("Import-WidgetPrototypes.json").read_bytes())
    return {role: prototypes["error_" + role] for role in ("control", "indicator")}


def _error_widget(uid, name, box, role, elements):
    widget = deepcopy(_error_prototypes()[role])
    widget.update(id=f"lv-widget-{uid}", label=name, valueType=ERROR_CLUSTER_TYPE)
    widget["bounds"].update(x=box[0], y=box[1])
    widget["props"]["label.visible"] = True
    for template, element in zip(widget["props"]["cluster.elements"], elements, strict=True):
        child, source = template["widget"], element["widget"]
        child.update(id=source["id"], label=source["label"])
        child["props"].update(source["props"])
        if "state" in source:
            child["state"] = source["state"]
    return widget


def label(body, location, encoding):
    parts = members(body, "partsList", location, optional=True)
    labels = [part for part in parts if kind(part) == "label" and number(part, "partID", location) == 16]
    if not labels:
        return ""
    require(len(labels) == 1, "panel_label", "Multiple saved labels refer to one widget.", location)
    record = field(labels[0], "textRec", location)
    return field(record, "text", location).content.decode(encoding)


def native_bounds(raw):
    left, top, right, bottom = raw
    return bounds(left, top, max(1, right - left), max(1, bottom - top))


def unbound_reference(source, encoding):
    children = source.value_children()
    return {"profile": "frog.studio.application_control@1", "kind": "reference",
            "expected_class": source.reference_class(), "method_id": "", "properties": [], "parameters": [],
            "target": {"kind": "unbound", "owner_document": "", "widget_id": ""},
            "value_schema": resource_payload(children[0], encoding) if children else
                {"canonical": "unknown", "choice_kind": "none", "choices": [], "field_names": [], "children": []}}


def resource_payload(schema, encoding, definition=()):
    """The public ApplicationControl codec stores both the spelling and its child schemas."""
    if schema.family == "TypeDef":
        return resource_payload(schema.children[0], encoding, schema.definition)
    enums = {}
    spelling = schema.native_type(encoding, enums, definition)
    if spelling is None:
        return None
    children = [resource_payload(child, encoding) for child in schema.value_children()]
    fields = [child.field_label().decode(encoding) for child in schema.children] if schema.family == "Cluster" else []
    record = {"canonical": spelling, "choice_kind": "none", "choices": [], "field_names": fields, "children": children}
    if spelling in enums:
        _, scalar, labels = enums[spelling]
        record.update(choice_kind="enum", choices=[{"id": f"lv-ordinal-{i}", "label": label, "value": i,
                      "disabled": False, "visible": True} for i, label in enumerate(labels)],
                      children=[{"canonical": scalar, "choice_kind": "none", "choices": [], "field_names": [], "children": []}])
    return record


def enum_widget(uid, name, box, role, schema, stored, encoding, location):
    choices = {}
    dtype = schema.native_type(encoding, choices)
    if dtype not in choices:
        return None
    concrete = schema
    while concrete.family == "TypeDef":
        if stored is not None:
            require(stored.tdType.name == "TypeDef" and len(stored.value) == 1,
                    "panel_default", "Malformed stored Enum typedef value.", location)
            stored = stored.value[0]
        concrete = concrete.children[0]
    require(stored is None or (stored.tdType.name == concrete.family and type(stored.value) is int),
            "panel_default", "The Enum default has no verified ordinal.", location)
    require(all(row[1:] == (None, None) for row in concrete.detail),
            "panel_enum", "The Enum items do not have verified sequential values.", location)
    domain, scalar, labels = choices[dtype]
    selected = 0 if stored is None else stored.value
    require(0 <= selected < len(labels), "panel_enum", "The selected Enum ordinal is outside its source domain.", location)
    items = [{"id": f"lv-ordinal-{i}", "text": label, "numeric_value": i, "enabled": True, "visible": True}
             for i, label in enumerate(labels)]
    return {"id": f"lv-widget-{uid}", "classId": f"frog.widgets.enum_{role}", "role": role,
            "valueType": "enum_item_id", "label": name, "bounds": native_bounds(box),
            "props": {"enum.domain_id": domain, "data_type.representation": scalar, "items": items,
                      "items.sequential_values": True, "value": items[selected]["id"], "label.visible": True},
            "state": {"value.selected_value": selected}}


def panel_widget(uid, body, descriptor, stored, role, types, encoding, depth=0):
    loc = f"FPHb/uid:{uid}"
    require(depth < 20, "panel_depth", "Cluster nesting exceeds the supported depth.", loc)
    name = label(body, loc, encoding)
    box = rect(body, "bounds", loc)
    family = "Map" if descriptor.fullType() == 0x74 else getattr(descriptor.fullType(), "name", None)
    require(family is not None, "panel_type", f"Unsupported saved type code {descriptor.fullType()}.", loc)
    if kind(body) == "stdGraph":
        dtype = types.descriptor(descriptor, loc)
        require(types.of(body, loc) == dtype, "panel_type", "The graph display and its terminal disagree.", loc)
        waveform = waveform_display(body, dtype, role, loc, encoding)
        value = implicit_value(dtype) if stored is None else data_value(stored, dtype, types, loc)
        return declaration(uid, Widget(uid, name, dtype, role, box, value, waveform))
    if kind(body) == "stdRing" and family in {"UnitUInt8", "UnitUInt16", "UnitUInt32"}:
        widget = enum_widget(uid, name, box, role, types.describe(descriptor, loc), stored, encoding, loc)
        require(widget is not None, "panel_enum", "This Enum has no verified set of ordinal values.", loc)
        return widget
    if family == "MeasureData" and kind(body) == "absTime" and types.describe(descriptor, loc).native_type(encoding) == "timestamp":
        require(stored is None, "panel_timestamp", "This Timestamp requires translation of its saved initializer.", loc)
        widget = declaration(uid, Widget(uid, name, "timestamp", role, box, 0))
        widget["bounds"] = native_bounds(box)
        widget["props"].update({"representation.kind": "timestamp", "data_type.named_numeric_size": "Timestamp"})
        return widget
    if family == "TypeDef" and kind(body) == "typeDef":
        widget = enum_widget(uid, name, box, role, types.describe(descriptor, loc), stored, encoding, loc)
        if widget is not None:
            return widget
        parts = [part for part in members(body, "partsList", loc)
                 if field(part, "partID", loc, optional=True) is not None and number(part, "partID", loc) == 8010]
        require(len(parts) == 1 and len(descriptor.clients) == 1, "panel_typedef",
                "The typedef has no unique element display or underlying type.", loc)
        underlying = types._child(descriptor.clients[0])
        display_type = types.heap(parts[0], "typeDesc", loc)
        require(types.describe(display_type, loc).representation() == types.describe(underlying, loc).representation(),
                "panel_typedef", "The typedef display disagrees with the underlying type.", loc)
        if stored is not None:
            require(stored.tdType.name == "TypeDef" and len(stored.value) == 1, "panel_default",
                    "The typedef default does not contain exactly one underlying value.", loc)
            stored = stored.value[0]
        widget = panel_widget(uid, parts[0], underlying, stored, role, types, encoding, depth + 1)
        widget["label"] = name
        if widget["valueType"] == ERROR_CLUSTER_TYPE:
            widget["bounds"].update(x=box[0], y=box[1])
        else:
            widget["bounds"] = native_bounds(box)
        return widget
    if family == "Array" and kind(body) == "tableControl":
        source = types.describe(descriptor, loc)
        require(source.native_type(encoding) == "array<string,rank=2>" and
                types.describe(types.heap(body, "typeDesc", loc), loc).representation() == source.representation(),
                "panel_table", "The table display must carry a two-dimensional string array.", loc)
        require(stored is None or (isinstance(stored, DataFillArray) and stored.dimensions == [0, 0]
                and not stored.value), "panel_table", "A populated table requires translation of its saved cells.", loc)
        box_bounds = native_bounds(box)
        # Keep the visible grid inside the saved table, including its frame.
        # The array renderer uses the cell template and its pitch to size the widget.
        inner_width = max(24, int(box_bounds["width"]) - 16)
        inner_height = max(20, int(box_bounds["height"]) - 16)
        cols = max(1, min(12, inner_width // 80))
        # The default GS String face occupies at least 36 logical pixels
        # after Studio measures the application font on open.
        rows = max(1, min(16, inner_height // 36))
        cell_width, cell_height = inner_width // cols, inner_height // rows
        cell = declaration(uid, Widget(uid, "", "string", role, (0, 0, cell_width, cell_height), ""))
        cell["bounds"].update(x=0, y=0, width=cell_width, height=cell_height)
        cell["id"] += "-cell"
        cell["props"]["label.visible"] = False
        return {"id": f"lv-widget-{uid}", "classId": f"frog.widgets.array_{role}", "role": role,
                "label": name, "bounds": box_bounds, "valueType": source.native_type(encoding),
                "props": {"dimensions.rank": 2, "dimensions.shape": [0, 0], "dimensions.allow_resize": True,
                          "index_display.visible": False, "viewport.first_indices": [0, 0],
                          "viewport.visible_counts": [rows, cols], "viewport.orientation": "matrix",
                          "element.cell_width": cell_width, "element.cell_height": cell_height, "element.gap": 0,
                          "element.class_id": cell["classId"], "element.role": role, "element.value_type": "string",
                          "element.template": cell, "label.visible": True}}
    if family == "Array" and kind(body) == "indArr":
        source = types.describe(descriptor, loc)
        dtype = source.native_type(encoding)
        rank = len(source.detail)
        require(dtype and rank in (1, 2) and len(source.children) == 1,
                "panel_array", "This array has no verified dynamic rank and element type.", loc)
        require(stored is None or (isinstance(stored, DataFillArray) and stored.dimensions == [0] * rank
                and not stored.value and types.describe(stored.td, loc).representation() == source.representation()),
                "panel_array", "The saved array initializer requires explicit cell translation.", loc)
        display = field(body, "ddo", loc)
        item = types.heap(display, "typeDesc", loc)
        require(types.describe(item, loc).representation() == source.children[0].representation(),
                "panel_array", "The array element display and connector type disagree.", loc)
        template = panel_widget(identity(display, loc), display, item, None, role, types, encoding, depth + 1)
        element = source.children[0].native_type(encoding)
        template["bounds"].update(x=0, y=0)
        template["props"]["label.visible"] = False
        return {"id": f"lv-widget-{uid}", "classId": f"frog.widgets.array_{role}", "role": role,
                "label": name, "bounds": native_bounds(box), "valueType": dtype,
                "props": {"dimensions.rank": rank, "dimensions.shape": [0] * rank, "dimensions.allow_resize": True,
                          "index_display.visible": True, "viewport.first_indices": [0] * rank,
                          "viewport.visible_counts": [1] * rank, "viewport.orientation": "vertical", "label.visible": True,
                          "element.class_id": template["classId"], "element.role": role,
                          "element.value_type": element, "element.template": template}}
    if family == "Path":
        empty = (isinstance(stored, DataFillPath) and stored.value.ident == b"PTH0" and
                 stored.value.tpval == 0 and not stored.value.content and not stored.value.canZeroFill)
        require(kind(body) == "stdPath" and (stored is None or empty), "panel_path",
                "This Path display requires translation of its saved path value.", loc)
        return {"id": f"lv-widget-{uid}", "classId": f"frog.widgets.path_{role}", "role": role,
                "label": name, "bounds": native_bounds(box), "valueType": "path",
                "props": {"text.value": "", "label.visible": True}}
    if family == "Map":
        source = types.describe(descriptor, loc)
        schema = source.native_type(encoding)
        require(kind(body) == "Class0186" and schema and len(source.children) == 2 and
                (stored is None or isinstance(stored, EmptyMapDefault)),
                "panel_map", "This Map requires a verified key, element type and empty default.", loc)
        key, value = (resource_payload(child, encoding) for child in source.children)
        return {"id": f"lv-widget-{uid}", "classId": "frog.studio.widgets.collection", "role": role,
                "label": name, "bounds": native_bounds(box), "valueType": schema,
                "props": {"label.visible": True, "collection.state": {"profile": "frog.studio.collection@1",
                    "kind": "map", "comparison_policy": "frog.studio.collection.order@1",
                    "key_type": key, "value_type": value, "data": {"entries": []}}}}
    if family in ("Refnum", "LVVariant"):
        source = types.describe(descriptor, loc)
        schema = source.native_type(encoding)
        if source.class_id:
            require(kind(body) == "udClassDDO" and stored is None, "panel_class",
                    "This class display requires translation of its saved object value.", loc)
            return {"id": f"lv-widget-{uid}", "classId": "frog.widgets.variant_control", "role": role,
                    "label": name, "bounds": native_bounds(box), "valueType": schema, "props": {"label.visible": True}}
        ref = family == "Refnum"
        require(schema is not None and kind(body) == ("stdRefNum" if ref else "stdLvVariant"),
                "panel_type", "The resource or Variant has no matching native display and type.", loc)
        null_reference = (ref and isinstance(stored, DataFillSimpleRefnum) and
                          stored.tdSubType == descriptor.reftype and stored.value == 0)
        require(stored is None or null_reference, "panel_default",
                "A persisted reference or Variant default requires explicit value translation.", loc)
        widget = {"id": f"lv-widget-{uid}", "classId": "frog.studio.widgets.reference" if ref else "frog.widgets.variant_control",
                  "role": role, "label": name, "valueType": schema, "bounds": native_bounds(box),
                  "props": {"label.visible": True}}
        if ref:
            widget["studio_application_control"] = unbound_reference(source, encoding)
        return widget
    if family != "Cluster":
        dtype = types.descriptor(descriptor, loc)
        value = implicit_value(dtype) if stored is None else data_value(stored, dtype, types, loc)
        widget = declaration(uid, Widget(uid, name, dtype, role, box, value))
        widget["bounds"] = native_bounds(box)
        return widget
    require(kind(body) == "stdClust", "panel_cluster", "The cluster's display body is unavailable.", loc)
    pane = field(body, "paneHierarchy", loc)
    displays = {identity(part, loc): part for part in members(pane, "zPlaneList", loc)}
    ids = [identity(part, loc) for part in members(body, "ddoList", loc)]
    require(len(ids) == len(set(ids)) == len(descriptor.clients) and set(ids) <= displays.keys(),
            "panel_cluster", "Cluster fields could not be matched to the element display list.", loc)
    if stored is not None:
        require(stored.tdType.name == "Cluster" and len(stored.value) == len(ids),
                "panel_default", "The stored cluster value does not match its type.", loc)
    elements = []
    for index, (child_uid, client) in enumerate(zip(ids, descriptor.clients)):
        child = displays[child_uid]
        child_type = types.consolidated.getFlatType(client.index)
        actual = types.consolidated.getTopType(types.slice.indexShift + number(child, "typeDesc", loc) - 1)
        require(actual.fullType() == child_type.fullType(), "panel_cluster", "A field display and type disagree.", loc)
        widget = panel_widget(child_uid, child, child_type, None if stored is None else stored.value[index],
                              role, types, encoding, depth + 1)
        field_name = child_type.label.decode(encoding) if child_type.label else widget["label"]
        field_id = field_name if field_name and not any(e["field_id"] == field_name for e in elements) else f"lv-field-{child_uid}"
        elements.append({"field_id": field_id, "technical_name": field_name, "logical_order": index, "widget": widget})
    if ([e["field_id"] for e in elements] == ["status", "code", "source"]
            and types.describe(descriptor, loc).native_type(encoding) == ERROR_CLUSTER_TYPE):
        return _error_widget(uid, name, box, role, elements)
    if elements:
        left = min(e["widget"]["bounds"]["x"] for e in elements)
        top = min(e["widget"]["bounds"]["y"] for e in elements)
        for elem in elements:
            elem["widget"]["bounds"]["x"] += 8 - left
            elem["widget"]["bounds"]["y"] += 22 - top
    dtype = "cluster:" + "|".join(e["widget"]["valueType"] for e in elements)
    return {"id": f"lv-widget-{uid}", "classId": f"frog.widgets.cluster_{role}", "role": role,
            "label": name, "valueType": dtype, "bounds": native_bounds(box),
            "props": {"cluster.elements": elements, "cluster.auto_sizing": "none", "label.visible": True}}


def project_panel(vi):
    if getattr(vi, "unread_panel_defaults", False):
        return {}, [{"resource": "DFDS", "reason": "Panel defaults could not be decoded. The source terminals are retained for review."}]
    types = HeapTypes(vi)
    root = field(heap_root(vi, b"FPHb"), "root", "FPHb")
    pane = field(root, "paneHierarchy", "FPHb")
    order = [identity(o, "FPHb") for o in members(root, "ddoList", "FPHb")]
    result, issues = {}, []
    encoding = getattr(vi, "textEncoding", "mac_roman")
    try:
        defaults = panel_defaults(vi, types, order)
    except ProfileRefusal as exc:
        # Retain the source diagram even if the data-space mapping is outside
        # the profile. Never instantiate a widget with a fabricated default.
        return {}, [{"resource": exc.resource, "reason": str(exc), "code": exc.code}]
    for obj in members(pane, "zPlaneList", "FPHb"):
        uid = identity(obj, "FPHb")
        loc = f"FPHb/uid:{uid}"
        if kind(obj) != "fPDCO":
            issues.append({"resource": loc, "kind": kind(obj), "reason": "Panel decoration retained in the VI."})
            continue
        body = None
        try:
            body = field(obj, "ddo", loc)
            heap_id = number(obj, "typeDesc", loc)
            require(0 < heap_id <= types.slice.tdCount, "panel_type", "Invalid DCO type index.", loc)
            desc = types.consolidated.getTopType(types.slice.indexShift + heap_id - 1)
            role = "indicator" if flags(obj, loc) & 1 else "control"
            record = None
            if defaults:
                require(uid in defaults and defaults[uid][2] == role and defaults[uid][0] is not None and
                        defaults[uid][0].fullType() == desc.fullType(),
                        "panel_default", "The default table and the panel object disagree.", loc)
                if getattr(desc.fullType(), "name", None) == "Refnum":
                    require(types.describe(desc, loc).representation() == types.describe(defaults[uid][0], loc).representation(),
                            "panel_default", "The reference and its data-space subtype disagree.", loc)
                record = defaults[uid][1]
            else:
                record = inline_panel_default(vi, obj, desc, loc)
            result[uid] = panel_widget(uid, body, desc, record, role, types, encoding)
        except (ProfileRefusal, UnicodeError) as exc:
            issue = {"resource": loc, "kind": kind(obj), "reason": str(exc),
                     "code": getattr(exc, "code", "panel_label")}
            if body is not None:
                try:
                    issue["object"] = label(body, loc, encoding)
                except (ProfileRefusal, UnicodeError):
                    pass  # The label itself may be the unreadable field.
            issues.append(issue)
    if result:
        left = min(w["bounds"]["x"] for w in result.values())
        top = min(w["bounds"]["y"] for w in result.values())
        for widget in result.values():
            widget["bounds"]["x"] += 32 - left
            widget["bounds"]["y"] += 48 - top
    return result, issues
