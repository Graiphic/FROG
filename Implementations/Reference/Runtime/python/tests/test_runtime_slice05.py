from __future__ import annotations

import json
from pathlib import Path

import pytest

from Implementations.Reference.Runtime.python.execute_contract import execute_contract
from Implementations.Reference.Runtime.python.runtime_core import RuntimeExecutionError, find_repo_root


def _repo_root() -> Path:
    return find_repo_root(Path(__file__).resolve())


def _load_acceptance() -> dict:
    path = _repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json"
    return json.loads(path.read_text(encoding="utf-8"))


def _resolve_repo_path(relative_path: str) -> Path:
    return _repo_root() / relative_path


def test_slice05_headless_matches_shared_snapshot() -> None:
    acceptance = _load_acceptance()
    contract_path = _resolve_repo_path(acceptance["artifact_refs"]["contract_path"])
    wfrog_path = _resolve_repo_path(acceptance["artifact_refs"]["wfrog_path"])
    snapshot_path = _resolve_repo_path(acceptance["artifact_refs"]["snapshot_path"])
    expected = json.loads(snapshot_path.read_text(encoding="utf-8"))

    actual = execute_contract(
        acceptance["headless"]["input_value"],
        contract_path=contract_path,
        wfrog_path=wfrog_path,
    )

    assert actual == expected


def test_slice05_overflow_matches_shared_acceptance() -> None:
    acceptance = _load_acceptance()
    contract_path = _resolve_repo_path(acceptance["artifact_refs"]["contract_path"])
    wfrog_path = _resolve_repo_path(acceptance["artifact_refs"]["wfrog_path"])

    with pytest.raises(RuntimeExecutionError, match=acceptance["overflow"]["expected_error"]):
        execute_contract(
            acceptance["overflow"]["input_value"],
            contract_path=contract_path,
            wfrog_path=wfrog_path,
        )
