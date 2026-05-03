from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
CHECKER = ROOT / "Implementations" / "Reference" / "Runtime" / "check_example05_runtime_acceptance.py"


def test_example05_runtime_acceptance_check_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Runtime acceptance check: ok" in result.stdout
