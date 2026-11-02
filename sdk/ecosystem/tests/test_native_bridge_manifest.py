"""Native bridge provenance references through the existing manifest owner.

These are declared contract fixtures, not evidence that any binary was built.
Existing library/bridge fixture files remain unchanged; no Frontend receipt is
fabricated for the synthetic independently compiled bridge manifest.
"""
from __future__ import annotations

import copy
import json
from pathlib import Path

import pytest

from sdk.ecosystem.contract import ContractError, validate_manifest, validate_schema


FIXTURES = Path(__file__).resolve().parents[1] / "fixtures"


def library():
    return json.loads((FIXTURES / "library-compiled.json").read_text(encoding="utf-8"))


def bridge(*, native=True):
    document = library()
    document["profile"] = "bridge"
    document["identity"]["package"] = "fixture-native-bridge" if native else "fixture-frontend-bridge"
    document["contributions"][0].update(id="fixture.bridge", kind="bridge")
    if native:
        target = document["targets"][0]
        previous = target.pop("frontend_provenance")
        target["build_provenance"] = "contracts/native-build-provenance.json"
        next(file for file in document["files"] if file["path"] == previous)["path"] = target["build_provenance"]
    return document


def reject(document, suffix, code, path=None):
    with pytest.raises(ContractError) as caught:
        validate_manifest(document, suffix)
    assert caught.value.code == code
    if path is not None:
        assert caught.value.path == path


def test_native_bridge_accepts_only_its_inventoried_build_provenance_without_mutation():
    document = bridge()
    before = copy.deepcopy(document)
    assert "frontend_provenance" not in document["targets"][0]
    validate_schema(document, "manifest")
    accepted = validate_manifest(document, ".bridgefrog")
    assert accepted == before and document == before and accepted is not document


def test_legacy_compiled_library_keeps_its_original_frontend_requirement():
    path = FIXTURES / "library-compiled.json"
    raw = path.read_bytes()
    document = library()
    before = copy.deepcopy(document)
    assert "frontend_provenance" in document["targets"][0]
    assert "build_provenance" not in document["targets"][0]
    assert validate_manifest(document, ".bfrog") == before
    assert document == before and path.read_bytes() == raw


def test_bridge_may_still_use_the_legacy_frontend_provenance_form():
    document = bridge(native=False)
    assert validate_manifest(document, ".bridgefrog") == document


@pytest.mark.parametrize("native", [False, True])
def test_compiled_target_rejects_absent_or_both_provenance_fields(native):
    document = bridge(native=native)
    target = document["targets"][0]
    chosen = "build_provenance" if native else "frontend_provenance"
    reference = target.pop(chosen)
    reject(document, ".bridgefrog", "contract.schema", "/targets/0")
    target["frontend_provenance"] = reference
    target["build_provenance"] = reference
    reject(document, ".bridgefrog", "contract.schema", "/targets/0")


@pytest.mark.parametrize("native", [False, True])
def test_validator_checks_the_inventory_for_the_actual_chosen_provenance(native):
    document = bridge(native=native)
    chosen = "build_provenance" if native else "frontend_provenance"
    document["targets"][0][chosen] = "contracts/not-in-inventory.json"
    reject(document, ".bridgefrog", "contract.missing_inventory", f"/targets/0/{chosen}")


@pytest.mark.parametrize("profile,suffix", [("library", ".bfrog"), ("driver", ".driverfrog")])
def test_native_build_provenance_is_not_a_substitute_for_other_profiles(profile, suffix):
    document = library()
    document["profile"] = profile
    target = document["targets"][0]
    target["build_provenance"] = target.pop("frontend_provenance")
    reject(document, suffix, "contract.schema", "/targets/0")


@pytest.mark.parametrize("targets", [None, []])
def test_compiled_native_bridge_still_requires_nonempty_targets(targets):
    document = bridge()
    if targets is None:
        del document["targets"]
    else:
        document["targets"] = targets
    reject(document, ".bridgefrog", "contract.schema")


def test_native_build_provenance_still_requires_a_portable_inventory_path():
    document = bridge()
    document["targets"][0]["build_provenance"] = "../native-build.json"
    reject(document, ".bridgefrog", "contract.path", "/targets/0/build_provenance")
