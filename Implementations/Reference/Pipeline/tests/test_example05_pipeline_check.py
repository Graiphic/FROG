from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
PIPELINE = ROOT / "Implementations" / "Reference" / "Pipeline" / "check_example05_pipeline.py"


def test_example05_pipeline_check_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(PIPELINE)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Pipeline status: ok" in result.stdout
    assert "runtime acceptance" in result.stdout
    assert "LLVM module" in result.stdout
