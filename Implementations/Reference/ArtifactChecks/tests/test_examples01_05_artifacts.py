from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
CHECKER = ROOT / "Implementations" / "Reference" / "ArtifactChecks" / "check_examples01_05_artifacts.py"


def test_examples01_05_artifact_preflight_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Artifact preflight status: ok" in result.stdout
