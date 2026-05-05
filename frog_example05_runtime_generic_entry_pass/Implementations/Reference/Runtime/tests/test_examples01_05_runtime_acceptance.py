from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
SCRIPT = ROOT / "Implementations" / "Reference" / "Runtime" / "check_examples01_05_runtime_acceptance.py"


def test_examples01_05_runtime_acceptance_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Runtime acceptance status: ok" in result.stdout


def test_examples01_05_runtime_acceptance_generic_only_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-specialized-example05"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Runtime acceptance ok: 05_bounded_ui_accumulator" in result.stdout
