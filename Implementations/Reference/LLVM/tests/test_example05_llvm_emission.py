from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
EMITTER = ROOT / "Implementations" / "Reference" / "LLVM" / "tools" / "emit_llvm_module.py"
LOWERING = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.lowering.json"
EXPECTED = ROOT / "Implementations" / "Reference" / "LLVM" / "examples" / "05_bounded_ui_accumulator" / "module.ll"


def test_example05_llvm_emission_check_passes() -> None:
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


def test_example05_llvm_emitter_writes_loop_module_with_overflow_rejection(tmp_path: Path) -> None:
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
    assert "%state_current = phi i16" in generated
    assert "%state_next = trunc i32 %sum_i32 to i16" in generated
    assert "%overflow = icmp ugt i32 %sum_i32, 65535" in generated
    assert "ret i32 -1" in generated
    assert "status=error" in generated
    assert "error=final_state must remain in the u16 domain." in generated
    assert "mul i16 %input_value" not in generated
