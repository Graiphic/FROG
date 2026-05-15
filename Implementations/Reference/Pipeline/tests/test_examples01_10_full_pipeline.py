from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
SCRIPT = ROOT / "Implementations" / "Reference" / "Pipeline" / "check_examples01_10_full.py"


def test_examples01_10_full_pipeline_passes() -> None:
    result = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Examples 01-10 full status: ok" in result.stdout
