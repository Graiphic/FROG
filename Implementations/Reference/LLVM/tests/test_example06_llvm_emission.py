from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
EMITTER = ROOT / "Implementations" / "Reference" / "LLVM" / "tools" / "emit_llvm_module.py"
LOWERING = ROOT / "Examples" / "06_boolean_value_roundtrip" / "main.lowering.json"
EXPECTED = ROOT / "Implementations" / "Reference" / "LLVM" / "examples" / "06_boolean_value_roundtrip" / "module.ll"


def test_example06_llvm_emission_check_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(EMITTER), "--lowering", str(LOWERING), "--expected", str(EXPECTED), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "LLVM module emission check: ok" in result.stdout


def test_example06_llvm_emitter_writes_module(tmp_path: Path) -> None:
    output = tmp_path / "module.ll"
    result = subprocess.run(
        [sys.executable, str(EMITTER), "--lowering", str(LOWERING), "--output", str(output)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr

    generated = output.read_text(encoding="utf-8").replace("\r\n", "\n").rstrip()
    expected = EXPECTED.read_text(encoding="utf-8").replace("\r\n", "\n").rstrip()

    assert generated == expected
    assert "define i1 @frog_example06_copy_bool" in generated
    assert "public_output=%s" in generated
    assert "status=ok" in generated
