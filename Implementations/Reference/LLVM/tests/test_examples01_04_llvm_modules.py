from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
SCRIPT = ROOT / "Implementations" / "Reference" / "LLVM" / "tools" / "check_examples01_04_llvm_modules.py"


def test_examples01_04_llvm_module_checks_pass() -> None:
    result = subprocess.run([sys.executable, str(SCRIPT), "--check"], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    assert result.returncode == 0, result.stdout + result.stderr
    assert "LLVM module check ok: Example 01" in result.stdout
