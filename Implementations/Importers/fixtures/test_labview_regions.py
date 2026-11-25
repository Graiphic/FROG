# SPDX-License-Identifier: Apache-2.0
"""Verify editable LabVIEW topology against explicitly supplied real source VIs."""

import argparse
from collections import Counter
from dataclasses import replace
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import re
import sys
import warnings
from xml.etree import ElementTree


IMPORTERS = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(IMPORTERS), str(IMPORTERS / "vendor" / "pylabview")]
warnings.filterwarnings("ignore", category=SyntaxWarning, message="invalid escape sequence")

from pylabview import LVblock, LVheap, LVrsrcontainer

from import_labview_bundle import saved_vi_icon
from import_labview_editable import (
    BindingTypes, cluster_literal, named_cluster_operation, project_editable, resource_primitive, static_control_targets,
)
from import_labview_interfaces import read_connector
from import_labview_profile import ATTR_UID, TYPES, ProfileRefusal, field, identity, items, kind, one_section, project
from import_labview_regions import STRUCTURE_KINDS, project_region_owner
from import_labview_source_diagram import decode_source_diagram, merge_error_ports


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def definition_index(vi):
    return {item.attribs[ATTR_UID]: item for item in one_section(vi, b"BDHb").objects
            if kind(item) is not None and ATTR_UID in item.attribs and
            item.getScopeInfo() != LVheap.NODE_SCOPE.TagClose}


def type_name(binding):
    if binding.enum_labels:
        width = binding.type_kind.removeprefix("UnitUInt")
        return f'enum<"fixture-type-{binding.type_id}",u{width}>'
    return TYPES.get(binding.type_kind, "unknown")


def positive(vi):
    graph = decode_source_diagram(vi)
    definitions = definition_index(vi)
    require(all(issue.code == "signal_state_unqualified" for issue in graph.issues), graph.issues)
    require({i for i, entry in definitions.items() if kind(entry) == "diag"} == graph.diagrams.keys(),
            "The owned graph did not retain every stored region.")
    require({i for i, entry in definitions.items() if kind(entry) == "signal"} == graph.signals.keys(),
            "The owned graph did not retain every stored signal.")
    require({i for i, entry in definitions.items() if kind(entry) in ("fPTerm", "term")} == graph.terminals.keys(),
            "The owned graph did not retain every stored terminal.")
    merge_profiles = {}
    for node in graph.nodes.values():
        ports = merge_error_ports(graph, node)
        if ports is None:
            continue
        merge_profiles[node.uid] = ports
        alternative = replace(node, field_names=(*node.field_names, "unreviewedMergeMode"))
        require(merge_error_ports(graph, alternative) is None,
                "The first-error projection accepted an unreviewed alternate mode.")
        parameters = node.fields["dcoList"]
        reverse = replace(parameters, children=tuple(reversed(parameters.children)))
        alternative = replace(node, fields=node.fields | {"dcoList": reverse})
        require(merge_error_ports(graph, alternative) is None,
                "The first-error projection accepted conflicting connector order.")
    for signal in graph.signals.values():
        original = definitions[signal.uid]
        saved = tuple(identity(item, "test/BDHb") for item in items(field(original, "termList", "test/BDHb"), "test/BDHb"))
        require(signal.endpoints == saved and signal.source in saved and
                signal.targets == tuple(uid for uid in saved if uid != signal.source),
                f"Signal {signal.uid} lost an original source, consumer or member order.")
        require(all(graph.terminals[uid].diagram == signal.diagram for uid in saved),
                f"Signal {signal.uid} crossed a region boundary.")
        require(graph.terminals[signal.source].direction == "source" and
                all(graph.terminals[uid].direction == "sink" for uid in signal.targets),
                f"Signal {signal.uid} was not oriented from its saved terminals.")
    for diagram in graph.diagrams.values():
        if diagram.uid != graph.root:
            require(graph.nodes[diagram.owner].regions[diagram.index] == diagram.uid,
                    f"Diagram {diagram.uid} lost its owner or its region order.")

    diagrams = {uid: {"nodes": [], "edges": [], "zoom_percent": 100} for uid in graph.diagrams}
    mapped = set()
    active = set()
    projected = 0
    for node in graph.nodes.values():
        if node.kind not in STRUCTURE_KINDS or not node.regions:
            continue
        native, endpoints, notes = project_region_owner(graph, node, diagrams, type_name)
        require(not mapped & endpoints.keys(), f"A source face of node {node.uid} was mapped more than once.")
        require(native["id"] == f"lv-node-{node.uid}" and len(native["regions"]) == len(node.regions) and
                all(row["diagram"] is diagrams[uid] for uid, row in zip(node.regions, native["regions"])),
                f"Node {node.uid} changed its region IDs or order.")
        require(all(owner == native["id"] and name for owner, name in endpoints.values()),
                f"Node {node.uid} produced a port without a valid native owner.")
        if node.kind == "select":
            require(native["structure_type"] == "case" and native["case_profile"] == "frog.case-editing@1" and
                    native["type"] == "frog.studio.structures.case_structure",
                    f"Case {node.uid} would open without its regions in the native codec.")
        if node.kind == "commentNode":
            active.add(graph.selections[node.uid].active_region)
        mapped.update(endpoints)
        projected += 1
        json.dumps(notes)

    for signal in graph.signals.values():
        require(all(uid in mapped for uid in signal.endpoints
                    if graph.nodes.get(graph.bindings[graph.terminals[uid].binding].owner) and
                    graph.nodes[graph.bindings[graph.terminals[uid].binding].owner].kind in STRUCTURE_KINDS),
                f"Signal {signal.uid} lost a native editable structure face.")
    return (graph, {"root": graph.root, "diagrams": len(graph.diagrams), "nodes": len(graph.nodes),
                    "terminals": len(graph.terminals), "signals": len(graph.signals),
                    "structural_faces": len(mapped), "projected_structures": projected,
                    "fanout_signals": sum(len(signal.targets) > 1 for signal in graph.signals.values()),
                    "largest_fanout": max((len(signal.targets) for signal in graph.signals.values()), default=0),
                    "node_classes": dict(Counter(node.kind for node in graph.nodes.values())), "merge_profiles": merge_profiles,
                    "raw_type_codes": {uid: binding.type_code for uid, binding in graph.bindings.items()
                                       if binding.type_code is not None and binding.type_kind is None},
                    "unqualified_signal_states": {issue.uid: graph.signals[issue.uid].state for issue in graph.issues},
                    "active_disable_regions": sorted(uid for uid in active if uid is not None)})


def verified_resources(vi, graph):
    types = BindingTypes(vi)
    wired = {uid for signal in graph.signals.values() for uid in signal.endpoints}
    matched = []
    targets = static_control_targets(vi, graph, types)
    for uid, target in targets.items():
        node = graph.nodes[uid]
        ddo = node.fields["ddo"]
        require(ddo.uid == target["display_uid"], "A static reference changed its saved display identity.")
        missing = replace(node, fields=dict(node.fields, ddo=replace(ddo, uid=-1)))
        altered = replace(graph, nodes={**graph.nodes, uid: missing})
        require(uid not in static_control_targets(vi, altered, types),
                "A missing display UID was rebound to a different control.")
    for binding in graph.bindings.values():
        if binding.kind != "bDConstDCO" or not binding.terminals:
            continue
        literal = cluster_literal(binding.terminals[0], binding, types)
        if literal:
            owner, fields = literal
            require(all(f["constant_owner_node_id"] == owner["id"] and f["constant_field_order"] == i
                        for i, f in enumerate(fields)), "A literal field lost its original owner or order.")
            value = binding.fields["ConstValue"]
            altered = replace(binding, fields=dict(binding.fields, ConstValue=replace(value, payload=value.payload + b"\x01")))
            require(cluster_literal(binding.terminals[0], altered, types) is None, "A partial cluster literal was accepted.")
    for node in graph.nodes.values():
        if node.kind == "select" and graph.selections[node.uid].exact_string_table:
            selection = graph.selections[node.uid]
            regions = {uid: {"nodes": [], "edges": []} for uid in node.regions}
            native, _, notes = project_region_owner(graph, node, regions, types)
            if (types(graph.bindings.get(selection.selector)) == "string" and
                    all(r.start_kind == r.end_kind == 0 and r.start == r.end and
                        0 <= r.start < len(selection.exact_string_table) for r in selection.range_rules)):
                expected = [(f"lv-region-{r.region}", node.fields["SelectStringArray"].children[r.start].payload.decode(vi.textEncoding))
                            for r in selection.range_rules]
                actual = [(region["id"], match["value"]) for region in native["regions"]
                          for match in region.get("matches", []) if match["kind"] == "string_exact"]
                require(sorted(actual) == sorted(expected), "String Case lost a message or its target region.")
                require(not any(n["code"] == "case_rule_unqualified" for n in notes), "A known message was left unresolved.")
                if selection.range_rules:
                    altered_rule = replace(selection.range_rules[0], start=-1, end=-1)
                    altered = replace(selection, range_rules=(altered_rule, *selection.range_rules[1:]))
                    rejected = replace(graph, selections={**graph.selections, node.uid: altered})
                    _, _, notes = project_region_owner(rejected, node, regions, types)
                    require(any(n["code"] == "case_rule_unqualified" for n in notes),
                            "An invalid String lookup index was admitted.")
        translation = resource_primitive(node, graph, types, wired)
        if translation:
            native, ports = translation
            require(ports.keys() == set(node.terminals), "A service port was lost.")
            wrong = replace(node, fields=dict(node.fields, primIndex=replace(node.fields["primIndex"], value=-1)))
            require(resource_primitive(wrong, graph, types, wired) is None, "Ambiguous primitive identity accepted.")
            matched.append(native["palette_catalog_id"])
        projection = named_cluster_operation(node, graph, types)
        if projection:
            native, ports = projection
            require(ports.keys() == set(node.terminals), "A named field extraction port was lost.")
            candidates = [graph.bindings[graph.terminals[uid].binding] for uid in node.terminals]
            for binding in candidates:
                if "i" in binding.fields:
                    fields = dict(binding.fields, i=replace(binding.fields["i"], value=99999))
                    altered = replace(graph, bindings={**graph.bindings, binding.uid: replace(binding, fields=fields)})
                    require(named_cluster_operation(node, altered, types) is None, "An out-of-range field identity was accepted.")
            matched.append(native["palette_catalog_id"])
    return matched


def negatives(vi, graph):
    definitions = definition_index(vi)
    changed = 0
    for signal in graph.signals.values():
        target = next((graph.terminals[uid] for uid in signal.targets
                       if graph.terminals[uid].direction_evidence == "terminal_flags"), None)
        if target is None:
            continue
        flag = field(definitions[target.uid], "objFlags", "test/BDHb")
        original = flag.value
        try:
            flag.value = (original & ~0x8000) | 0x1000
            graph_bad = decode_source_diagram(vi)
            wire = graph_bad.signals[signal.uid]
            require(wire.endpoints == signal.endpoints and wire.source is None and not wire.targets and
                    any(issue.uid == signal.uid and issue.code == "signal_orientation_unresolved"
                        for issue in graph_bad.issues), "The reader guessed a producer on an ambiguous wire.")
            changed += 1
        finally:
            flag.value = original
        break

    for signal in graph.signals.values():
        foreign = next((terminal.uid for terminal in graph.terminals.values() if terminal.diagram != signal.diagram), None)
        if foreign is None:
            continue
        references = items(field(definitions[signal.uid], "termList", "test/BDHb"), "test/BDHb")
        target = references[-1]
        original = target.attribs[ATTR_UID]
        try:
            target.attribs[ATTR_UID] = foreign
            graph_bad = decode_source_diagram(vi)
            wire = graph_bad.signals[signal.uid]
            require(wire.endpoints[-1] == foreign and wire.source is None and
                    any(issue.uid == signal.uid and issue.code == "signal_endpoint_scope" and issue.related == (foreign,)
                        for issue in graph_bad.issues), "The reader accepted a cross-region wire.")
            changed += 1
        finally:
            target.attribs[ATTR_UID] = original
        break
    require(decode_source_diagram(vi) == graph, "The reader changed the original heap after an unsuccessful projection.")
    return changed


def presentation(cli, path):
    data = cli.read_snapshot(path)
    digest = hashlib.sha256(data).hexdigest()
    vi = cli.inspect(data, path, {"diagnostics": []})
    try:
        document, mapping = project(vi, path, digest)
    except ProfileRefusal:
        documents, mapping = project_editable(path, vi)
        document = documents[Path("model.frog")]

    evidence = mapping["presentation"]
    source_icon = evidence["icon"]
    icon = document["icon"]["svg"]
    require(source_icon["status"] == "saved", "The real VI fixture needs a saved icon.")
    block = vi.blocks[source_icon["resource"].encode("ascii")]
    raw = bytes(block.getRawData(next(iter(block.sections))))
    decoder = getattr(LVblock, source_icon["resource"])(vi, vi.po)
    upstream = decoder.parseRSRCRawImage(32, 32, source_icon["bits_per_pixel"], 1, io.BytesIO(raw)).convert("RGB")
    root = ElementTree.fromstring(icon)
    require(root.get("viewBox") == "0 0 32 32", "The translated icon has the wrong canvas.")
    pixels = [None] * 1024
    for path_element in root:
        require(path_element.tag.endswith("}path"), "An unexpected external icon element was exported.")
        fill = bytes.fromhex(path_element.attrib["fill"][1:])
        for x, y, width, back in re.findall(r"M(\d+) (\d+)h(\d+)v1h-(\d+)z", path_element.attrib["d"]):
            x, y, width, back = map(int, (x, y, width, back))
            require(x + width <= 32 and y < 32 and width == back, "An icon run exceeds its source row.")
            for position in range(y * 32 + x, y * 32 + x + width):
                require(pixels[position] is None, "An icon pixel was drawn twice.")
                pixels[position] = tuple(fill)
    require(pixels == list(upstream.get_flattened_data()), "The exported icon differs from its saved bitmap.")
    saved_size = block.size
    try:
        block.size += 1
        _, fallback = saved_vi_icon(vi)
        require(source_icon["resource"] in fallback["invalid_resources"] and
                fallback["resource"] != source_icon["resource"], "An invalid icon payload was accepted.")
    finally:
        block.size = saved_size

    connector = read_connector(vi)
    status = evidence["connector"]["status"]
    bound_widgets = {widget["id"]: widget for widget in document["frontPanel"]["widgets"]
                     if "binding" in widget}
    declared = {(port["id"], direction): port for direction in ("inputs", "outputs")
                for port in document["interface"][direction]}
    expected = {(f"slot-{binding.slot}", "outputs" if binding.output else "inputs") for binding in connector.bindings}
    if status == "verified":
        require(declared.keys() == expected and len(bound_widgets) == len(expected),
                "The import invented or dropped connector slots.")
        for binding in connector.bindings:
            key = "public_output_id" if binding.output else "public_input_id"
            direction = "outputs" if binding.output else "inputs"
            slot = f"slot-{binding.slot}"
            widget = bound_widgets[f"lv-widget-{binding.widget_uid}"]
            require(widget["binding"][key] == slot, "A public connector slot was applied to another widget.")
            require(declared[(slot, direction)].get("label") == (widget["label"] or slot),
                    "A public connector slot lost the display name of its bound widget.")
    else:
        require(status == "unsupported" and expected and not declared and not bound_widgets,
                "An unverified partial interface was published.")
    require(cli.read_snapshot(path) == data, "Checking presentation modified the LabVIEW source.")
    return {"file": path.name, "source_sha256": digest, "profile": mapping["profile"],
            "saved_bitmap_sha256": hashlib.sha256(raw).hexdigest(), "matching_pixels": len(pixels),
            "public_labels": {slot: port["label"] for (slot, _), port in declared.items()},
            "icon_svg_sha256": hashlib.sha256(icon.encode()).hexdigest(), **evidence}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", action="append", type=Path, default=[])
    parser.add_argument("--presentation-source", action="append", type=Path, default=[])
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    if not (args.source or args.presentation_source):
        parser.error("Supply at least one --source or --presentation-source VI.")
    spec = importlib.util.spec_from_file_location("labview_cli", IMPORTERS / "Import-LabView.py")
    cli = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(cli)
    receipts = []
    for source in args.source:
        source = source.resolve(strict=True)
        before = cli.read_snapshot(source)
        vi, inventory = cli.index_resources(before, source, LVrsrcontainer, LVblock)
        cli.decode_resources(vi, inventory, LVheap, {"diagnostics": []})
        graph, counts = positive(vi)
        counts["native_source_operations"] = verified_resources(vi, graph)
        digest = hashlib.sha256(before).hexdigest()
        counts["negative_heap_mutations"] = negatives(vi, graph)
        require(hashlib.sha256(cli.read_snapshot(source)).hexdigest() == digest, "The actual VI file was changed.")
        receipts.append({"file": source.name, "source_sha256": digest, **counts})
        print(f"LABVIEW_SOURCE_DIAGRAM_PASS {source.name} {counts}")
    for source in args.presentation_source:
        proof = presentation(cli, source.resolve(strict=True))
        receipts.append(proof)
        print(f"LABVIEW_PRESENTATION_PASS {source.name} {proof['icon']['resource']} "
              f"pixels={proof['matching_pixels']} slots={len(proof['connector']['bindings'])} "
              f"connector={proof['connector']['status']}")
    if args.report is not None:
        args.report.write_text(json.dumps(receipts, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
