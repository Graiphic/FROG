<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime Bridge</h1>

<p align="center">
  <strong>Compiler-agnostic bridge posture for the non-normative FROG reference runtime</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document records the boundary between the FROG reference runtime and backend-produced compiled artifacts.
The runtime hosts execution and UI. Backends compile lowered units. Explicit manifests and stable call surfaces connect both worlds.
</p>

<p>
This document is non-normative. Version governance remains centralized in <code>Versioning/Readme.md</code> and repository history.
</p>

<hr/>

<h2>Current Status</h2>

<p>
The C++ reference runtime currently hosts the Example 05 UI and executes the backend contract. The LLVM proof path separately validates native-oriented compiler output from the lowered unit. The browser renderer is functional but not yet a faithful <code>.wfrog</code> renderer.
</p>

<p>
The bridge described here is a target milestone, not an already completed runtime property.
</p>

<hr/>

<h2>Core Rule</h2>

<pre><code>FROG runtime hosts execution and UI.
FROG backends compile lowered units.
Explicit manifests and stable ABI surfaces connect both worlds.
</code></pre>

<hr/>

<h2>Example 05 Target</h2>

<p>
The first implementation target should remain <code>Examples/05_bounded_ui_accumulator/</code>.
The runtime should consume a manifest-declared compiled artifact instead of owning the Example 05 algorithm internally.
</p>

<p>
The first success case remains <code>input_value = 3</code> and <code>result = 15</code>. Overflow should be reported through a machine-readable status that the runtime maps to diagnostics.
</p>

<hr/>

<h2>Non-Goals</h2>

<ul>
  <li>The runtime does not compile diagrams.</li>
  <li>The runtime does not depend on LLVM.</li>
  <li>This bridge does not claim a complete production runtime.</li>
  <li>This bridge does not introduce Example 06 or new widget classes.</li>
</ul>
