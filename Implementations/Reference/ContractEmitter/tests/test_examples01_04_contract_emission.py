from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
SCRIPT = ROOT / "Implementations" / "Reference" / "ContractEmitter" / "emit_examples01_04_contracts.py"


def test_examples01_04_contract_emission_check_passes() -> None:
    result = subprocess.run([sys.executable, str(SCRIPT), "--check"], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Contract emission check ok: 01_pure_addition" in result.stdout
