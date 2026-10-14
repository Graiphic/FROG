"""Focused canonical source→open IR acceptance; no private runtime claims."""
from __future__ import annotations

import copy
import hashlib
import json
import subprocess
import sys
from dataclasses import replace
from pathlib import Path

import jsonschema
import pytest
from referencing import Registry, Resource

from Implementations.Reference.common import FrogPipelineError
from Implementations.Reference.Deriver.canonical_execution_ir import (
    CanonicalIRLimits, derive_canonical_ir, check_constructed_ir,
)

ROOT = Path(__file__).resolve().parents[4]
EXAMPLE = ROOT / "Examples/02_ui_value_roundtrip/main.frog"


def validator():
    # Independent published Draft 2020-12 validation. Missing schema/dependency
    # is a failure, never a skipped acceptance check or a producer-local schema.
    schemas = [json.loads(path.read_text(encoding="utf-8")) for path in (ROOT / "IR/schema").glob("*.json")]
    registry = Registry().with_resources((schema["$id"], Resource.from_contents(schema)) for schema in schemas)
    root = next(schema for schema in schemas if schema["$id"].endswith("/frog.execution-ir.schema.json"))
    return jsonschema.Draft202012Validator(root, registry=registry)


def source():
    return json.loads(EXAMPLE.read_text(encoding="utf-8"))


def write_source(tmp_path, document, name="source.frog"):
    path = tmp_path / name
    path.write_text(json.dumps(document, ensure_ascii=False, allow_nan=False), encoding="utf-8", newline="\n")
    return path


def derive(path):
    return derive_canonical_ir(path, document_id="acceptance:example02")


def resolve_pointer(document, pointer):
    value = document
    for item in pointer.split("/")[1:]:
        item = item.replace("~1", "/").replace("~0", "~")
        value = value[int(item)] if isinstance(value, list) else value[item]
    return value


def assert_recoverable(artifact, path):
    document = json.loads(path.read_bytes().decode("utf-8"))
    unit = artifact["unit"]
    objects = {record["id"]: record for record in unit["objects"]}
    connections = {record["id"]: record for record in unit["connections"]}
    declarations = {record["id"]: record for record in unit["correspondence"]}
    raw_nodes = {node["id"]: node for node in document["diagram"]["nodes"]}
    raw_edges = {edge["id"]: edge for edge in document["diagram"]["edges"]}
    raw_widgets = {widget["id"]: widget for widget in document["front_panel"]["widgets"]}
    assert artifact["schema_family"] == "frog.execution_ir"
    assert artifact["document_id"] != unit["id"]
    assert "artifact_kind" not in artifact
    assert len(objects) == len(raw_nodes)
    assert len(connections) == len(raw_edges)
    assert unit["regions"] == []
    source_prefix = path.resolve().as_uri() + "#"
    for mapping in unit["source_map"]:
        assert mapping["source_ref"].startswith(source_prefix)
        original = resolve_pointer(document, mapping["source_ref"][len(source_prefix):])
        if mapping["source_kind"] == "diagram_node":
            assert objects[mapping["target_refs"][0]]["attributes"]["source_node_id"] == original["id"]
        elif mapping["source_kind"] == "diagram_edge":
            assert connections[mapping["target_refs"][0]]["metadata"]["source_edge_id"] == original["id"]
        elif mapping["source_kind"] == "widget_instance":
            assert mapping["relation"] == "non_primary"  # declaration ≠ participation object
            assert all(objects[target]["attributes"]["widget_id"] == original["id"] for target in mapping["target_refs"])
        elif mapping["source_kind"] == "widget_member":
            assert all(objects[target]["attributes"]["default_value"] == original for target in mapping["target_refs"])
    for record in objects.values():
        original = raw_nodes[record["attributes"]["source_node_id"]]
        assert record["family"] == original["kind"]
        assert record["classification"]["primary_or_support"] == "primary"
        assert all(port["type_ref"] == "f64" and port["boundary_kind"] == "none" for port in record["ports"])
        assert all(port["metadata"] == {"source_node_id": original["id"], "source_port_id": port["id"]} for port in record["ports"])
        if record["family"] == "primitive":
            assert record["kind"] == "frog.core.add"
            assert [(p["id"], p["direction"]) for p in record["ports"]] == [("a", "in"), ("b", "in"), ("result", "out")]
        else:
            widget = raw_widgets[original["widget"]]
            assert record["attributes"]["widget_id"] == widget["id"]
            assert record["attributes"]["widget_class"] == widget.get("widget", widget.get("class_ref"))
            assert record["kind"] == "widget_value"
            assert record["role"] == widget["role"]
            assert record["ports"][0]["direction"] == ("out" if widget["role"] == "control" else "in")
            relation = declarations[record["correspondence_refs"][0]]
            assert relation["relation_kind"] == "declaration_reference"
            assert record["id"] in relation["target_refs"]
    for edge in connections.values():
        original = raw_edges[edge["metadata"]["source_edge_id"]]
        assert edge["kind"] == "dataflow"
        for side in ("from", "to"):
            node = objects[edge[side]["object_id"]]
            assert node["attributes"]["source_node_id"] == original[side]["node"]
            assert edge[side]["port_id"] == original[side]["port"]
    for relation in declarations.values():
        assert relation["source_ref"].startswith(source_prefix)
        resolve_pointer(document, relation["source_ref"][len(source_prefix):])
    assert list(validator().iter_errors(artifact)) == []


def test_published_example02_is_semantically_admitted_and_schema_valid():
    before = EXAMPLE.read_bytes()
    first = derive(EXAMPLE)
    assert_recoverable(first, EXAMPLE)
    assert first == derive(EXAMPLE)
    assert EXAMPLE.read_bytes() == before
    assert "Exact source bytes: sha256:" + hashlib.sha256(before).hexdigest() in first["metadata"]["notes"]
    assert [record["attributes"].get("default_value") for record in first["unit"]["objects"] if record.get("role") == "control"] == [1.0, 2.0]


def test_real_source_exact_byte_boundary_is_admitted_and_one_byte_less_is_unsupported(monkeypatch):
    data = EXAMPLE.read_bytes()
    # The producer must use a bounded stream read, not allocate the whole file
    # through Path.read_bytes before deciding whether to admit it.
    def unbounded_read_forbidden(_path):
        raise AssertionError("Producer used an unbounded source read")
    with monkeypatch.context() as guard:
        guard.setattr(Path, "read_bytes", unbounded_read_forbidden)
        artifact = derive_canonical_ir(EXAMPLE, document_id="acceptance:example02",
                                       limits=CanonicalIRLimits(max_source_bytes=len(data)))
        with pytest.raises(FrogPipelineError) as raised:
            derive_canonical_ir(EXAMPLE, document_id="acceptance:example02",
                                limits=CanonicalIRLimits(max_source_bytes=len(data) - 1))
    assert raised.value.error_code == "source_resource_limit"
    assert raised.value.diagnostics[0]["status"] == "unsupported"
    assert raised.value.diagnostics[0]["location"] == "/"
    assert_recoverable(artifact, EXAMPLE)
    assert "Exact source bytes: sha256:" + hashlib.sha256(data).hexdigest() in artifact["metadata"]["notes"]
    assert EXAMPLE.read_bytes() == data


@pytest.mark.parametrize("budget,exact,location", [
    ("max_nodes", 4, "/diagram/nodes"),
    ("max_edges", 3, "/diagram/edges"),
    ("max_widgets", 3, "/front_panel/widgets"),
])
def test_real_source_inventory_exact_boundary_and_one_below(budget, exact, location):
    limits = replace(CanonicalIRLimits(), **{budget: exact})
    artifact = derive_canonical_ir(EXAMPLE, document_id="audit:02", limits=limits)
    assert_recoverable(artifact, EXAMPLE)
    with pytest.raises(FrogPipelineError) as raised:
        derive_canonical_ir(EXAMPLE, document_id="audit:02", limits=replace(limits, **{budget: exact - 1}))
    assert raised.value.error_code == "source_resource_limit"
    assert raised.value.diagnostics[0]["status"] == "unsupported"
    assert raised.value.diagnostics[0]["location"] == location


def test_real_source_identity_exact_byte_boundary_and_one_below():
    doc = source()
    identifiers = [node["id"] for node in doc["diagram"]["nodes"]] + [edge["id"] for edge in doc["diagram"]["edges"]]
    identifiers += [widget["id"] for widget in doc["front_panel"]["widgets"]]
    identifiers += [value for edge in doc["diagram"]["edges"] for side in ("from", "to") for value in edge[side].values()]
    exact = max(len(identity.encode("utf-8")) for identity in identifiers)
    limits = CanonicalIRLimits(max_id_bytes=exact)
    assert_recoverable(derive_canonical_ir(EXAMPLE, document_id="audit:02", limits=limits), EXAMPLE)
    with pytest.raises(FrogPipelineError) as raised:
        derive_canonical_ir(EXAMPLE, document_id="audit:02", limits=replace(limits, max_id_bytes=exact - 1))
    assert raised.value.error_code == "source_resource_limit"
    assert raised.value.diagnostics[0]["status"] == "unsupported"
    assert raised.value.diagnostics[0]["location"].endswith("/id")


@pytest.mark.parametrize("limit", [0, -1, True, CanonicalIRLimits().max_source_bytes + 1])
def test_invalid_or_excessive_budget_cannot_disable_the_declared_producer_ceiling(limit):
    with pytest.raises(FrogPipelineError) as raised:
        derive_canonical_ir(EXAMPLE, document_id="audit:02", limits=CanonicalIRLimits(max_source_bytes=limit))
    assert raised.value.error_code == "invalid_admission_limits"
    assert raised.value.diagnostics[0]["status"] == "unsupported"


def test_ids_names_and_array_order_do_not_select_an_example_pattern(tmp_path):
    doc = source()
    doc["metadata"]["name"] = "different & unrelated program"
    rename = {node["id"]: "node / é🙂:" + node["id"] for node in doc["diagram"]["nodes"]}
    widget_rename = {widget["id"]: "widget / é🙂:" + widget["id"] for widget in doc["front_panel"]["widgets"]}
    for node in doc["diagram"]["nodes"]:
        node["id"] = rename[node["id"]]
        if "widget" in node:
            node["widget"] = widget_rename[node["widget"]]
    for widget in doc["front_panel"]["widgets"]:
        widget["id"] = widget_rename[widget["id"]]
    for edge in doc["diagram"]["edges"]:
        edge["id"] = "edge / é🙂:" + edge["id"]
        for side in ("from", "to"):
            edge[side]["node"] = rename[edge[side]["node"]]
    doc["diagram"]["nodes"].reverse()
    doc["diagram"]["edges"].reverse()
    doc["front_panel"]["widgets"].reverse()
    path = write_source(tmp_path, doc)
    artifact = derive(path)
    assert_recoverable(artifact, path)
    assert len({record["id"] for record in artifact["unit"]["objects"]}) == 4


def test_admission_handles_real_add_chain_and_fanout_not_fixed_four_node_pattern(tmp_path):
    doc = source()
    doc["diagram"]["nodes"].append({"id": "second-add", "kind": "primitive", "type": "frog.core.add"})
    doc["diagram"]["edges"][2]["to"] = {"node": "second-add", "port": "a"}
    doc["diagram"]["edges"].extend([
        {"id": "fanout", "from": {"node": "ctrl_b_value", "port": "value"}, "to": {"node": "second-add", "port": "b"}},
        {"id": "result", "from": {"node": "second-add", "port": "result"}, "to": {"node": "ind_result_value", "port": "value"}},
    ])
    path = write_source(tmp_path, doc)
    assert_recoverable(derive(path), path)


def test_canonical_class_refs_and_unused_widget_declaration_remain_distinct(tmp_path):
    doc = source()
    for widget in doc["front_panel"]["widgets"]:
        role = widget["role"]
        widget.pop("widget")
        widget["class_ref"] = "frog.widgets.numeric_" + role
    unused = copy.deepcopy(doc["front_panel"]["widgets"][0])
    unused["id"] = "unused-control"
    doc["front_panel"]["widgets"].append(unused)
    path = write_source(tmp_path, doc)
    artifact = derive(path)
    assert_recoverable(artifact, path)
    assert len(artifact["unit"]["objects"]) == 4
    mapping = next(record for record in artifact["unit"]["source_map"] if record["source_kind"] == "widget_instance" and record["source_ref"].endswith("/3"))
    assert mapping["target_kind"] == "none"
    assert mapping["target_refs"] == []
    assert mapping["relation"] == "non_primary"


@pytest.mark.parametrize("change,code,status", [
    (lambda d: d["diagram"]["nodes"][2].update(kind="structure"), "unsupported_node_family", "unsupported"),
    (lambda d: d["diagram"]["nodes"][2].update(type="frog.core.mul"), "unsupported_primitive", "unsupported"),
    (lambda d: d["diagram"]["nodes"][2].update(kind=[]), "invalid_node_kind", "rejected"),
    (lambda d: d["diagram"]["nodes"][2].pop("type"), "invalid_primitive_identity", "rejected"),
    (lambda d: d["diagram"]["nodes"][2].update(ports=[]), "unsupported_source_field", "unsupported"),
    (lambda d: d["diagram"]["nodes"][0].update(widget="missing"), "unknown_widget", "rejected"),
    (lambda d: d["diagram"]["edges"][0]["to"].update(node="missing"), "unknown_edge_node", "rejected"),
    (lambda d: d["diagram"]["edges"][0]["to"].update(port="result"), "invalid_edge_direction", "rejected"),
    (lambda d: d["diagram"]["edges"][0]["to"].update(port="unknown"), "unknown_edge_port", "rejected"),
    (lambda d: d["diagram"]["edges"][0]["to"].update(port="input_3"), "unsupported_variadic_add", "unsupported"),
    (lambda d: d["diagram"]["edges"][0].update(type="i32"), "unsupported_source_field", "unsupported"),
    (lambda d: d["front_panel"]["widgets"][0].update(value_type="i32"), "unsupported_value_type", "unsupported"),
    (lambda d: d["front_panel"]["widgets"][0].update(role="indicator"), "incompatible_widget_role", "rejected"),
    (lambda d: d["front_panel"]["widgets"][0]["props"].update(default_value=True), "invalid_f64_default", "rejected"),
    (lambda d: d["front_panel"]["widgets"][0]["props"].update(default_value="1.0"), "invalid_f64_default", "rejected"),
    (lambda d: d["front_panel"]["widgets"][0]["props"].update(default_value=9007199254740993), "invalid_f64_default", "rejected"),
    (lambda d: d["front_panel"]["widgets"][0]["props"].update(**{"data_type.representation": "i32"}), "conflicting_value_type", "rejected"),
    (lambda d: d["diagram"].update(wire_fragments=[{"id": "incomplete"}]), "incomplete_wire_fragments", "rejected"),
    (lambda d: d["interface"]["inputs"].append({"id": "a", "type": "f64"}), "unsupported_public_interface", "unsupported"),
    (lambda d: d["interface"]["outputs"].append({"id": "out", "type": "f64"}), "unsupported_public_interface", "unsupported"),
    (lambda d: d.update(host={"version": 1, "launch": {"trigger": "manual"}}), "unsupported_source_policy", "unsupported"),
])
def test_mutated_source_has_no_ir_and_precise_diagnostic(tmp_path, change, code, status):
    document = source()
    change(document)
    path = write_source(tmp_path, document)
    before = path.read_bytes()
    with pytest.raises(FrogPipelineError) as raised:
        derive(path)
    assert raised.value.error_code == code
    assert raised.value.stage == "semantic_validation"
    assert raised.value.diagnostics[0]["status"] == status
    assert raised.value.diagnostics[0]["location"].startswith("/")
    assert path.read_bytes() == before


@pytest.mark.parametrize("collection,code", [("nodes", "duplicate_node_id"), ("edges", "duplicate_edge_id")])
def test_duplicate_graph_identity_is_rejected(tmp_path, collection, code):
    doc = source()
    doc["diagram"][collection].append(copy.deepcopy(doc["diagram"][collection][0]))
    with pytest.raises(FrogPipelineError) as raised:
        derive(write_source(tmp_path, doc))
    assert raised.value.error_code == code


def test_missing_driver_multiple_drivers_and_combinational_cycle_are_not_repaired(tmp_path):
    doc = source()
    del doc["diagram"]["edges"][1]
    with pytest.raises(FrogPipelineError, match="real driver"):
        derive(write_source(tmp_path, doc))
    doc = source()
    extra = copy.deepcopy(doc["diagram"]["edges"][0])
    extra["id"] = "duplicate-driver"
    doc["diagram"]["edges"].append(extra)
    with pytest.raises(FrogPipelineError) as raised:
        derive(write_source(tmp_path, doc))
    assert raised.value.error_code == "multiple_input_drivers"
    doc = source()
    doc["diagram"]["edges"][0]["from"] = {"node": "add_1", "port": "result"}
    with pytest.raises(FrogPipelineError) as raised:
        derive(write_source(tmp_path, doc))
    assert raised.value.error_code == "combinational_cycle"


@pytest.mark.parametrize("family", ["widget_reference", "subfrog", "interface_input", "interface_output", "constant", "state", "event_structure", "unknown"])
def test_unimplemented_families_never_fall_back_to_private_or_empty_ir(tmp_path, family):
    doc = source()
    doc["diagram"]["nodes"][0]["kind"] = family
    with pytest.raises(FrogPipelineError) as raised:
        derive(write_source(tmp_path, doc))
    assert raised.value.error_code == "unsupported_node_family"
    assert raised.value.diagnostics[0]["status"] == "unsupported"


def test_cache_ide_geometry_and_program_name_never_become_semantic_authority(tmp_path):
    doc = source()
    path = write_source(tmp_path, doc)
    first = derive(path)
    doc["metadata"]["name"] = "forged pattern selector"
    doc["cache"] = {"frog.ir": {"schema_family": "frog.execution_ir", "unit": {"objects": [{"kind": "fake"}]}}}
    doc["ide"] = {"selected_nodes": ["missing"], "runtime_value": 999, "provenance": {"author": "test"}}
    doc["front_panel"]["canvas"] = {"width": 1, "height": 1}
    doc["front_panel"]["widgets"][0]["layout"]["width"] = 999
    second = derive(write_source(tmp_path, doc))
    assert first["unit"]["objects"] == second["unit"]["objects"]
    assert first["unit"]["connections"] == second["unit"]["connections"]
    assert first["unit"]["source_map"] == second["unit"]["source_map"]
    assert first["metadata"]["notes"] != second["metadata"]["notes"]  # exact bytes really changed
    assert {r["source_ref"].split("#", 1)[1] for r in second["unit"]["correspondence"]} >= {"/cache", "/ide"}
    assert_recoverable(second, path)
    doc["diagram"]["edges"][0]["to"]["node"] = "missing"
    with pytest.raises(FrogPipelineError) as raised:
        derive(write_source(tmp_path, doc))
    assert raised.value.error_code == "unknown_edge_node"  # cached IR cannot conceal broken source


def test_original_newlines_have_exact_identity_not_normalized_text_hash(tmp_path):
    path = tmp_path / "crlf.frog"
    crlf = json.dumps(source(), ensure_ascii=False, indent=2).replace("\n", "\r\n").encode("utf-8")
    path.write_bytes(crlf)
    artifact = derive(path)
    assert "Exact source bytes: sha256:" + hashlib.sha256(crlf).hexdigest() in artifact["metadata"]["notes"]
    assert path.read_bytes() == crlf
    assert_recoverable(artifact, path)


@pytest.mark.parametrize("content,code", [
    (b'{"metadata":{},"metadata":{}}', "duplicate_json_member"),
    (b'{"spec_version": NaN}', "invalid_json_number"),
    (b'{"spec_version": 1e999}', "unsupported_json_number"),
    (b'{"spec_version": 1e-999}', "unsupported_json_number"),
    (b'\xff', "invalid_source"),
])
def test_existing_strict_reader_rules_are_preserved(content, code, tmp_path):
    path = tmp_path / "strict.frog"
    path.write_bytes(content)
    with pytest.raises(FrogPipelineError) as raised:
        derive(path)
    assert raised.value.error_code == code
    assert path.read_bytes() == content


@pytest.mark.parametrize("change", [
    lambda a: a["unit"]["objects"][2].update(kind="frog.core.mul"),
    lambda a: a["unit"]["objects"][2]["ports"][0].update(id="unknown"),
    lambda a: a["unit"]["objects"][2]["ports"][0].update(type_ref="i32"),
    lambda a: a["unit"]["connections"][0]["to"].update(object_id="missing"),
    lambda a: a["unit"]["connections"][0].update(**{"from": {"object_id": a["unit"]["objects"][2]["id"], "port_id": "result"}}),
    lambda a: a["unit"]["source_map"].pop(0),
    lambda a: a["unit"]["correspondence"].pop(0),
])
def test_construction_coherence_is_stronger_than_structural_schema(change):
    artifact = derive(EXAMPLE)
    change(artifact)
    # The generic published schema does not resolve graph endpoints or type
    # identities. These mutations are structurally admissible yet wrong.
    assert list(validator().iter_errors(artifact)) == []
    with pytest.raises(FrogPipelineError) as raised:
        check_constructed_ir(artifact)
    assert raised.value.error_code == "invalid_constructed_ir"


def test_published_schema_independently_rejects_missing_categories_and_reference_fir():
    for field in ("objects", "connections", "regions", "source_map", "correspondence"):
        invalid = derive(EXAMPLE)
        del invalid["unit"][field]
        assert not validator().is_valid(invalid)
    reference = json.loads((ROOT / "Examples/02_ui_value_roundtrip/main.fir.json").read_text(encoding="utf-8"))
    assert reference["artifact_kind"] == "frog_fir_unit"
    assert not validator().is_valid(reference)


def test_command_emits_real_open_artifact_and_rejection_does_not_overwrite_output(tmp_path):
    output = tmp_path / "example02.execution-ir.json"
    module = ROOT / "Implementations/Reference/Deriver/canonical_execution_ir.py"
    run = subprocess.run([sys.executable, "-B", str(module), str(EXAMPLE), "--document-id", "acceptance:example02", "--output", str(output)],
                         capture_output=True, text=True, encoding="utf-8", check=False)
    assert run.returncode == 0, run.stderr
    assert_recoverable(json.loads(output.read_text(encoding="utf-8")), EXAMPLE)
    before = output.read_bytes()
    public_source = ROOT / "Examples/01_pure_addition/main.frog"
    run = subprocess.run([sys.executable, "-B", str(module), str(public_source), "--document-id", "acceptance:unsupported", "--output", str(output)],
                         capture_output=True, text=True, encoding="utf-8", check=False)
    assert run.returncode == 2
    assert json.loads(run.stderr)["error_code"] == "unsupported_public_interface"
    assert output.read_bytes() == before


def test_cli_byte_limit_uses_real_source_and_preserves_existing_output(tmp_path):
    output = tmp_path / "example02.execution-ir.json"
    module = ROOT / "Implementations/Reference/Deriver/canonical_execution_ir.py"
    data = EXAMPLE.read_bytes()
    command = [sys.executable, "-B", str(module), str(EXAMPLE), "--document-id", "audit:02", "--output", str(output), "--max-source-bytes"]
    admitted = subprocess.run(command + [str(len(data))], capture_output=True, text=True, encoding="utf-8", check=False)
    assert admitted.returncode == 0, admitted.stderr
    assert_recoverable(json.loads(output.read_text(encoding="utf-8")), EXAMPLE)
    before = output.read_bytes()
    rejected = subprocess.run(command + [str(len(data) - 1)], capture_output=True, text=True, encoding="utf-8", check=False)
    assert rejected.returncode == 2
    diagnostic = json.loads(rejected.stderr)
    assert diagnostic["error_code"] == "source_resource_limit"
    assert diagnostic["diagnostics"][0]["status"] == "unsupported"
    assert output.read_bytes() == before
    assert EXAMPLE.read_bytes() == data
