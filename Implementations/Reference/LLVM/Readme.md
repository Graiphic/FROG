<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference LLVM-Oriented Path</h1>

<p align="center">
  <strong>Downstream compiler-family posture for native executable closure in the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#architectural-boundary">2. Architectural Boundary</a></li>
  <li><a href="#current-published-directory-shape">3. Current Published Directory Shape</a></li>
  <li><a href="#current-reproducibility-check">4. Current Reproducibility Check</a></li>
  <li><a href="#build-check">5. Build Check</a></li>
  <li><a href="#scope">6. Scope</a></li>
  <li><a href="#summary">7. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines the downstream compiler-family posture for LLVM-oriented native executable closure in the non-normative FROG reference implementation.
LLVM remains downstream from FROG.
It is one possible compiler-family consumer of lowered FROG artifacts.
It is not the definition of the language, not the owner of FIR, and not the owner of widget semantics.
</p>

<p>
At the current published state, this directory exposes a first repository-visible LLVM-oriented closure dossier for the canonical Example 05 corridor, and a reference helper that can regenerate the published <code>module.ll</code> from the published lowered artifact.
</p>

<hr/>

<h2 id="architectural-boundary">2. Architectural Boundary</h2>

<pre><code>canonical .frog source
      |
      v
validated meaning
      |
      v
FIR / execution-facing representation
      |
      v
lowering
      |
      v
compiler-facing lowered kernel
      |
      v
LLVM-oriented downstream path
      |
      v
native executable
</code></pre>

<p>
The current LLVM path is intentionally a pure bounded-kernel proof.
It does not compile the rendered front panel natively.
UI and widget effects remain runtime-mediated in the current architecture.
</p>

<hr/>

<h2 id="current-published-directory-shape">3. Current Published Directory Shape</h2>

<pre><code>Implementations/Reference/LLVM/
├── Readme.md
├── examples/
│   └── 05_bounded_ui_accumulator/
│       ├── Readme.md
│       ├── main.fir.json
│       ├── main.lowering.json
│       ├── module.ll
│       ├── build.sh
│       └── expected-output.json
├── bridge/
│   ├── fir_to_lowering.md
│   └── lowering_to_llvm.md
├── tests/
│   └── test_example05_llvm_emission.py
└── tools/
    └── emit_llvm_module.py
</code></pre>

<hr/>

<h2 id="current-reproducibility-check">4. Current Reproducibility Check</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check
</code></pre>

<p>
This command reads:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.lowering.json</code></pre>

<p>
and checks that the emitted LLVM module matches:
</p>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll</code></pre>

<hr/>

<h2 id="build-check">5. Build Check</h2>

<p>
To also compile and run the published native proof when <code>clang</code> is available:
</p>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check --build</code></pre>

<p>
The build script itself remains:
</p>

<pre><code>cd Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator
bash build.sh
</code></pre>

<hr/>

<h2 id="scope">6. Scope</h2>

<p>
The current LLVM emitter supports only the frozen Example 05 lowered shape:
</p>

<ul>
  <li>one lowered unit named <code>main</code>,</li>
  <li><code>u16</code> state,</li>
  <li>initial state <code>0</code>,</li>
  <li>one add operation <code>state_next = state_current + input_value</code>,</li>
  <li>iteration count read from the lowered artifact,</li>
  <li>final publication parity with the published runtime corridor.</li>
</ul>

<p>
This is not a general LLVM backend.
</p>

<hr/>

<h2 id="summary">7. Summary</h2>

<p>
The current LLVM-oriented path now has a reproducibility check:
</p>

<pre><code>main.lowering.json -&gt; module.ll</code></pre>

<p>
The next depth step would be to replace the current closed-form multiply proof with a more explicit loop/state lowering if the project wants the native module to mirror the lowered kernel structure more literally.
</p>
