from __future__ import annotations

import copy
import os
import subprocess
import sys
from pathlib import Path

import pytest

from Implementations.Reference.Deriver.fir_deriver import (
    DerivationError, canonical_json_bytes, derive_fir_from_source, load_json,
)
from Implementations.Reference.Pipeline.check_examples01_10_full import EXAMPLES

ROOT = Path(__file__).resolve().parents[4]
ADDITION = "Examples/01_pure_addition/main.frog"
ACCUMULATOR = "Examples/05_bounded_ui_accumulator/main.frog"
PICTURE = "Examples/16_picture_logo_jpeg/main.frog"
SOURCES = [item.source for item in EXAMPLES] + [PICTURE]


@pytest.mark.parametrize("version", ["99.0", 99, "0.2", None, True, {}])
def test_unsupported_envelope_cannot_reach_a_matching_derivation_rule(version):
    source = load_json(ROOT / ADDITION)
    source["spec_version"] = version
    before = copy.deepcopy(source)
    with pytest.raises(DerivationError, match="source envelope.*spec_version"):
        derive_fir_from_source(source, ADDITION)
    assert source == before


@pytest.mark.parametrize("version", ["0.1", 0.1])
def test_both_public_version_encodings_keep_identical_fir(version):
    source = load_json(ROOT / ADDITION)
    expected = canonical_json_bytes(derive_fir_from_source(source, ADDITION))
    source["spec_version"] = version
    assert canonical_json_bytes(derive_fir_from_source(source, ADDITION)) == expected


def test_required_sections_and_policy_profiles_are_not_bypassed():
    source = load_json(ROOT / ADDITION)
    del source["metadata"]
    with pytest.raises(DerivationError, match="missing_top_level_sections"):
        derive_fir_from_source(source, ADDITION)
    source = load_json(ROOT / ADDITION)
    source["execution_policy"] = {"version": 99}
    with pytest.raises(DerivationError, match="unsupported_policy_version"):
        derive_fir_from_source(source, ADDITION)


@pytest.mark.parametrize("raw,code", [
    (b'{"spec_version":"99.0","spec_version":"0.1"}', "duplicate_json_member"),
    (b'{"nested":{"a":1,"\\u0061":2}}', "duplicate_json_member"),
    (b'{"value":NaN}', "invalid_json_number"),
    (b'{"value":Infinity}', "invalid_json_number"),
    (b'{"value":-Infinity}', "invalid_json_number"),
    (b'{"value":1e400}', "unsupported_json_number"),
    (b'{"value":1e-400}', "unsupported_json_number"),
    (b'{"value":"\xff"}', "invalid_utf8"),
    (b'{"value":', "invalid_json"),
    (b'[]', "invalid_top_level"),
])
def test_deriver_uses_the_common_strict_json_boundary(tmp_path, raw, code):
    path = tmp_path / "source.frog"
    path.write_bytes(raw)
    with pytest.raises(DerivationError, match=code):
        load_json(path)


@pytest.mark.parametrize("source_rel", SOURCES)
@pytest.mark.parametrize("extra", ["executable_node", "malformed_node", "edge"])
def test_no_registered_rule_ignores_added_top_level_graph_entries(source_rel, extra):
    source = load_json(ROOT / source_rel)
    diagram = source["diagram"]
    if extra == "executable_node":
        diagram["nodes"].append({"id": "ignored_effect", "kind": "primitive", "type": "frog.ui.property_write"})
    elif extra == "malformed_node":
        diagram["nodes"].append(None)
    else:
        diagram["edges"].append({"id": "ignored_edge", "from": {"node": "missing", "port": "out"},
                                 "to": {"node": "also_missing", "port": "in"}})
    before = copy.deepcopy(source)
    with pytest.raises(DerivationError, match="unsupported graph shape"):
        derive_fir_from_source(source, source_rel)
    assert source == before


@pytest.mark.parametrize("field", ["region", "regions", "diagram", "body"])
def test_flat_patterns_refuse_added_nested_graphs(field):
    source = load_json(ROOT / ADDITION)
    source["diagram"]["nodes"][0][field] = {"nodes": [{"id": "hidden", "kind": "primitive",
                                                         "type": "frog.ui.property_write"}], "edges": []}
    with pytest.raises(DerivationError, match="unsupported nested"):
        derive_fir_from_source(source, ADDITION)


@pytest.mark.parametrize("mutation", ["node", "edge", "type", "region", "input", "output", "fragment"])
def test_accumulator_region_has_its_own_exact_recursive_signature(mutation):
    source = load_json(ROOT / ACCUMULATOR)
    loop = next(node for node in source["diagram"]["nodes"] if node["kind"] == "for_loop")
    body = loop["region"]
    if mutation == "node":
        body["nodes"].append({"id": "hidden", "kind": "primitive", "type": "frog.ui.property_write"})
    elif mutation == "edge":
        body["edges"].append(copy.deepcopy(body["edges"][0]))
    elif mutation == "type":
        body["nodes"][0]["type"] = "frog.ui.property_write"
    elif mutation == "region":
        body["nodes"][0]["region"] = {"nodes": [], "edges": []}
    elif mutation == "input":
        body["inputs"].append({"id": "hidden", "type": "u16"})
    elif mutation == "output":
        body["outputs"][0]["from"]["node"] = "delay_state"
    else:
        body["wire_fragments"] = [{"id": "unfinished"}]
    with pytest.raises(DerivationError, match="unsupported source pattern"):
        derive_fir_from_source(source, ACCUMULATOR)


def test_same_arity_does_not_hide_changed_node_kinds_or_dangling_endpoints():
    source = load_json(ROOT / ADDITION)
    source["diagram"]["nodes"][2]["type"] = "frog.ui.property_write"
    with pytest.raises(DerivationError, match="kind/type inventory"):
        derive_fir_from_source(source, ADDITION)
    source = load_json(ROOT / ADDITION)
    source["diagram"]["edges"][0]["from"]["node"] = "missing"
    with pytest.raises(DerivationError, match="unsupported graph endpoint"):
        derive_fir_from_source(source, ADDITION)


@pytest.mark.parametrize("edge_index", range(4))
@pytest.mark.parametrize("direction,field", [("from", "node"), ("from", "port"), ("to", "node"), ("to", "port")])
def test_each_accumulator_internal_endpoint_must_match_the_emitted_recurrence(edge_index, direction, field):
    source = load_json(ROOT / ACCUMULATOR)
    loop = next(node for node in source["diagram"]["nodes"] if node["kind"] == "for_loop")
    endpoint = loop["region"]["edges"][edge_index][direction]
    if field == "node":
        # Keep endpoints in scope, so GraphShape still accepts their inventory.
        # r_e1 specifically reproduces input -> initial-state substitution.
        endpoint[field] = "loop_initial_state" if endpoint[field] != "loop_initial_state" else "loop_input_value"
    else:
        endpoint[field] = "changed_port"
    before = copy.deepcopy(source)
    with pytest.raises(DerivationError, match="unsupported internal wiring"):
        derive_fir_from_source(source, ACCUMULATOR)
    assert source == before


def test_accumulator_internal_connection_order_and_edge_ids_are_not_semantic_authority():
    source = load_json(ROOT / ACCUMULATOR)
    expected = canonical_json_bytes(derive_fir_from_source(source, ACCUMULATOR))
    loop = next(node for node in source["diagram"]["nodes"] if node["kind"] == "for_loop")
    loop["region"]["edges"].reverse()
    for index, edge in enumerate(loop["region"]["edges"]):
        edge["id"] = f"renamed_edge_{index}"
    assert canonical_json_bytes(derive_fir_from_source(source, ACCUMULATOR)) == expected


@pytest.mark.parametrize("source_rel", SOURCES[5:])
def test_examples06_through16_still_have_explicit_legacy_metadata_name_guards(source_rel):
    source = load_json(ROOT / source_rel)
    source["metadata"]["name"] = "renamed source"
    with pytest.raises(DerivationError, match=r"metadata\.name"):
        derive_fir_from_source(source, source_rel)


def test_graph_inventory_is_not_array_order_or_metadata_name_dispatch():
    source = load_json(ROOT / ADDITION)
    expected = canonical_json_bytes(derive_fir_from_source(source, ADDITION))
    source["metadata"]["name"] = "not an example selector"
    source["diagram"]["nodes"].reverse()
    source["diagram"]["edges"].reverse()
    assert canonical_json_bytes(derive_fir_from_source(source, ADDITION)) == expected


def test_direct_deriver_cli_does_not_require_repository_pythonpath(tmp_path):
    env = os.environ.copy()
    env.pop("PYTHONPATH", None)
    result = subprocess.run([
        sys.executable, "-B", str(ROOT / "Implementations/Reference/Deriver/derive_fir.py"),
        "--source", ADDITION, "--expected", "Examples/01_pure_addition/main.fir.json", "--check",
    ], cwd=tmp_path, env=env, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stdout + result.stderr
    assert "FIR derivation check: ok" in result.stdout
