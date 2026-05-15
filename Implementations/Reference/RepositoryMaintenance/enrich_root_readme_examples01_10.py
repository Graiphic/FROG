#!/usr/bin/env python3
"""Enrich the root Readme.md with the current Examples 01-10 executable closure.

This script intentionally patches the existing root Readme.md in place instead of
replacing it wholesale. It preserves the existing strategic content while
refreshing the repository-state, runtime-direction, and project-status sections.

Run from the repository root:

    python Implementations/Reference/RepositoryMaintenance/enrich_root_readme_examples01_10.py
"""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
README = ROOT / "Readme.md"


SECTIONS: dict[str, str] = {
    "published-repository-state": """<h2 id=\"published-repository-state\">Published repository state</h2>

<p>
At the current published state, the repository contains the six core architectural specification families:
<code>Expression/</code>,
<code>Language/</code>,
<code>IR/</code>,
<code>Libraries/</code>,
<code>Profiles/</code>,
and <code>IDE/</code>.
These remain the primary ownership layers of the published language specification.
</p>

<p>
The repository also contains repository-level support areas and repository-level framing / governance layers:
</p>

<ul>
  <li><strong><code>Examples/</code></strong> — illustrative named source slices, executable example dossiers, and bounded compiler-corridor mirrors,</li>
  <li><strong><code>Conformance/</code></strong> — public accept / reject / preserve expectations for the published repository state,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — a non-normative reference implementation workspace used to exercise disciplined execution paths,</li>
  <li><strong><code>Versioning/</code></strong> — centralized specification-governance and current-status reporting for the published specification corpus,</li>
  <li><strong><code>Strategy/</code></strong> — a non-normative strategic framing layer distinct from normative ownership,</li>
  <li><strong><code>Roadmap/</code></strong> — a non-normative closure-sequencing layer distinct from both strategy and specification.</li>
</ul>

<p>
The published numbered example surface now exposes a repository-visible executable progression under <code>Examples/01_*</code> through <code>Examples/10_*</code>.
Each numbered example has a canonical <code>.frog</code> source, a published FIR artifact, a published lowering artifact, a backend-contract path, runtime acceptance material, and an LLVM-oriented native proof surface.
</p>

<p>
The progression is intentionally staged:
</p>

<pre><code>01_pure_addition
  -> pure public-interface arithmetic

02_ui_value_roundtrip
  -> natural widget_value participation

03_ui_property_write
  -> widget_reference and frog.ui.property_write

04_stateful_feedback_delay
  -> explicit delay-backed feedback state

05_bounded_ui_accumulator
  -> combined UI + property write + bounded loop + explicit state corridor

06_boolean_value_roundtrip
  -> Boolean control / indicator value binding

07_string_value_roundtrip
  -> String control / indicator value binding

08_enum_value_roundtrip
  -> Enum control / indicator value binding

09_path_value_roundtrip
  -> Path control / indicator value binding

10_button_press_to_boolean
  -> momentary Button pressed state to Boolean indicator
</code></pre>

<p>
<code>Examples/05_bounded_ui_accumulator/</code> remains the primary applicative vertical-slice anchor because it combines source, front-panel package, FIR, lowering, backend contract, runtime-family acceptance, Python/Rust/C/C++ reference-consumer posture, and an LLVM-oriented native proof path.
Examples <code>01</code> through <code>04</code> are smaller executable proof slices used to keep individual concerns inspectable before they are combined in Example <code>05</code>.
Examples <code>06</code> through <code>10</code> extend the same runtime/widget discipline across Boolean, String, Enum, Path, and Button front-panel slices.
</p>

<p>
The correct current statement is therefore:
the repository materially exposes an executable Examples <code>01</code> through <code>10</code> corridor, while Example <code>05</code> remains the richest applicative UI/state/runtime/native reference corridor and Examples <code>06</code> through <code>10</code> validate the current scalar widget wave.
This does not claim full generalized symmetry across all future examples, all runtime families, or rendered-native front-panel closure.
</p>
""",
    "campaign-priority": """<h2 id=\"campaign-priority\">Campaign priority</h2>

<p>
The current campaign priority is explicit:
<strong>keep the published executable corridor green, then generalize cautiously.</strong>
</p>

<p>
A serious example is no longer considered finished merely because it is source-readable or architecturally plausible.
A serious example should progressively converge toward:
</p>

<ul>
  <li>one canonical <code>.frog</code> source,</li>
  <li>one explicit front-panel posture when applicable,</li>
  <li>one explicit FIR reading,</li>
  <li>one explicit lowering posture,</li>
  <li>one backend contract,</li>
  <li>one shared runtime-acceptance posture,</li>
  <li>and, where applicable, one LLVM-oriented native proof path.</li>
</ul>

<p>
The current published numbered examples provide this progression in bounded form:
</p>

<pre><code>.frog
  -> FIR
  -> lowering
  -> backend contract
  -> runtime acceptance
  -> LLVM proof
</code></pre>

<p>
Example <code>05_bounded_ui_accumulator</code> remains the primary applicative corridor for runtime-family and UI-facing work.
Examples <code>01</code> through <code>04</code> serve as smaller executable anchors for pure computation, widget value flow, object-style UI effects, and explicit state.
Examples <code>06</code> through <code>10</code> serve as bounded scalar widget anchors.
</p>

<p>
This campaign does <strong>not</strong> make one runtime the definition of FROG.
It makes the opposite point:
the language remains stable while downstream consumers remain modular and independently checkable.
</p>
""",
    "repository-runtime-and-native-execution-direction": """<h2 id=\"repository-runtime-and-native-execution-direction\">Runtime and native execution direction</h2>

<p>
The repository direction is intentionally explicit:
published examples should become consumable through runtime-family and compiler-family paths without making either path the definition of FROG.
</p>

<p>
The current executable reference reading is:
</p>

<pre><code>canonical .frog source
      |
      v
FIR
      |
      v
lowering
      |
      +----------------------------+----------------------------+
      |                            |
      v                            v
backend contract              LLVM-oriented module
      |                            |
      v                            v
runtime acceptance            native proof
</code></pre>

<p>
For Examples <code>01</code> through <code>04</code>, the runtime acceptance and LLVM proofs are intentionally narrow reference proofs.
For Example <code>05</code>, the repository carries the richer applicative path involving a front-panel package, widget values, widget references, UI property writes, explicit state, bounded iteration, runtime-family acceptance, and LLVM native proof material.
</p>

<p>
The reference implementation workspace remains stage-separated:
Deriver, Lowerer, ContractEmitter, Runtime, and LLVM are downstream consumers of the published source/FIR/lowering corridor rather than semantic owners of the language.
</p>
""",
    "project-status": """<h2 id=\"project-status\">Project status</h2>

<p>
FROG is currently under active design, cleanup, stabilization, and executable-corridor closure.
The repository already contains substantial material across canonical source representation, source-schema posture, language semantics, execution-facing IR architecture, intrinsic standardized primitive libraries, optional profile architecture, IDE architecture, governance surfaces, strategic framing, roadmap posture, examples, conformance material, and a non-normative reference implementation workspace.
</p>

<p>
At the current published state, the repository has reached a stronger closure milestone:
Examples <code>01</code> through <code>10</code> materially expose a repository-visible executable corridor across source, FIR, lowering, backend contracts, runtime acceptance, and LLVM-oriented proof material.
</p>

<p>
The Example <code>05_bounded_ui_accumulator</code> slice remains the primary applicative vertical-slice anchor because it combines front-panel package participation, widget values, widget references, UI property writes, bounded iteration, explicit state, public output, runtime-family acceptance, and LLVM-native proof posture.
Examples <code>01</code> through <code>04</code> provide smaller executable anchors for isolated concerns, while Examples <code>06</code> through <code>10</code> extend the bounded widget/runtime surface.
</p>

<p>
At the same time, the repository has not yet reached:
</p>

<ul>
  <li>full generalized multi-runtime symmetry across all serious examples,</li>
  <li>a generic contract executor that removes all example-specific runtime acceptance logic,</li>
  <li>a generic LLVM backend driven by lowered-unit kind rather than bounded example patterns,</li>
  <li>full native rendered front-panel closure,</li>
  <li>or final depth across all observability, debugging, and IDE-facing surfaces.</li>
</ul>

<p>
The current direction is therefore:
</p>

<ul>
  <li><strong>keep the Examples 01-10 executable corridor green,</strong></li>
  <li><strong>convert the reference runtime and LLVM proofs from example-specific code toward generic pattern-driven engines,</strong></li>
  <li><strong>then resume qualitative deepening of complex widget families and front-panel runtime behavior.</strong></li>
</ul>
"""
}


def replace_section(text: str, section_id: str, replacement: str) -> str:
    start = f'<h2 id="{section_id}">'
    start_index = text.find(start)
    if start_index < 0:
        raise RuntimeError(f"section not found: {section_id}")

    next_hr = text.find("\n<hr/>", start_index + len(start))
    if next_hr < 0:
        raise RuntimeError(f"section terminator not found after: {section_id}")

    return text[:start_index] + replacement.rstrip() + "\n" + text[next_hr:]


def main() -> int:
    text = README.read_text(encoding="utf-8")
    original = text

    for section_id, replacement in SECTIONS.items():
        text = replace_section(text, section_id, replacement)

    if text == original:
        print("Readme.md unchanged")
        return 0

    README.write_text(text, encoding="utf-8", newline="\n")
    print("Updated Readme.md with Examples 01-10 executable-corridor status")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
