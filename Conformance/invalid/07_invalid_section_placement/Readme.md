<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Conformance Case 07</h1>

<p align="center">
  <strong>Invalid section placement must be rejected as canonical source structural failure</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case identity</h2>

<ul>
  <li><strong>Case ID:</strong> <code>invalid/07_invalid_section_placement</code></li>
  <li><strong>Kind:</strong> invalid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Primary boundary:</strong> canonical source structural validity</li>
  <li><strong>Stage of expected failure:</strong> structural validation</li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that canonical FROG source MUST reject source objects placed in the wrong top-level section family.
</p>

<p>
It exists to make one source-ownership rule publicly testable:
</p>

<pre><code>source object placement
belongs to canonical source structure
and must not be repaired
by semantic interpretation
or implementation convenience
</code></pre>

<p>
This is a structural-source failure.
It is not a later semantic rejection.
</p>

<hr/>

<h2>Why this case exists</h2>

<p>
The FROG Expression specification defines explicit top-level section families such as <code>metadata</code>, <code>interface</code>, <code>connector</code>, <code>diagram</code>, <code>front_panel</code>, <code>icon</code>, <code>ide</code>, and <code>cache</code>.
Section-local objects belong to their owning section family and MUST NOT be accepted when placed under the wrong top-level section.
</p>

<p>
Without this rejection rule, implementations could drift toward hidden source repair such as:
</p>

<ul>
  <li>accepting diagram-owned executable content under <code>front_panel</code>,</li>
  <li>accepting front-panel widget objects under <code>diagram</code> as if they were ordinary executable nodes,</li>
  <li>accepting connector-owned perimeter projection content under <code>interface</code> or <code>diagram</code>,</li>
  <li>accepting section-local objects under unrelated top-level containers and silently relocating them.</li>
</ul>

<p>
This case therefore protects the source-shape boundary of canonical <code>.frog</code> content.
</p>

<hr/>

<h2>What this case tests</h2>

<ul>
  <li>that section-local source objects remain owned by their proper top-level section family,</li>
  <li>that wrong-section placement is rejected during structural validation,</li>
  <li>that an implementation does not silently reinterpret misplaced source objects as valid canonical content,</li>
  <li>that canonical source structural validity remains distinct from later semantic validation.</li>
</ul>

<hr/>

<h2>Construct under test</h2>

<p>
The invalid construct is:
</p>

<pre><code>a source object that belongs to one section family
appears under a different top-level section family
and is still loadable JSON
but is not structurally valid canonical source
</code></pre>

<p>
A representative example is a widget declaration placed inside <code>diagram</code> as if front-panel widget composition were a diagram-owned executable source family.
</p>

<p>
Another representative example is an executable node-like object placed directly inside <code>front_panel</code> as if executable graph ownership belonged there.
</p>

<p>
The exact concrete fixture may vary, but the expected classification MUST remain the same:
</p>

<pre><code>loadable source
   -&gt;
structurally invalid canonical source
   -&gt;
meaning not established
</code></pre>

<hr/>

<h2>Expected outcome</h2>

<ul>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> not established</li>
  <li><strong>Expected rejection:</strong> source object placed in the wrong top-level section family</li>
</ul>

<p>
The implementation MUST reject this case before claiming validated program meaning.
</p>

<p>
It MUST NOT:
</p>

<ul>
  <li>silently move the misplaced object to another section,</li>
  <li>accept the object because it “looks understandable”,</li>
  <li>defer the problem as if it were only a semantic error,</li>
  <li>treat the misplaced object as implementation-defined tolerated input while still claiming conformance to the published case.</li>
</ul>

<hr/>

<h2>Architectural boundary being protected</h2>

<pre><code>parseable JSON
    !=
structurally valid canonical source

section presence
    !=
section-local placement correctness

front_panel content
    !=
diagram content

diagram content
    !=
front_panel content

connector projection
    !=
logical interface definition

implementation tolerance
    !=
published conformance truth
</code></pre>

<hr/>

<h2>Why the failure is structural, not semantic</h2>

<p>
This case is not about whether a valid source object would have meaningful execution behavior once correctly placed.
It is about the fact that canonical source ownership has already been violated before semantic interpretation is entitled to proceed.
</p>

<p>
The failure therefore belongs to the structural-validation corridor owned by <code>Expression/</code>, including source-shape posture and section-local placement rules.
</p>

<p>
This case should be read together with the published distinction between:
</p>

<ul>
  <li>loadability,</li>
  <li>structural validity,</li>
  <li>semantic acceptance,</li>
  <li>later preservation expectations.</li>
</ul>

<hr/>

<h2>Conformance reading rule</h2>

<p>
A conforming implementation should classify this case as:
</p>

<pre><code>Loadability:
  yes

Structural validity:
  no

Validated meaning:
  not established

IR derivation:
  must not be claimed as conforming

Reason:
  wrong top-level section placement for a section-owned source object
</code></pre>

<hr/>

<h2>Minimal fixture guidance</h2>

<p>
A minimal fixture for this case SHOULD stay small and isolate section-placement error only.
It SHOULD avoid introducing additional unrelated failures that would obscure the reason for rejection.
</p>

<p>
Good fixture design:
</p>

<ul>
  <li>keep all required top-level sections present,</li>
  <li>keep JSON syntax valid,</li>
  <li>introduce one clearly misplaced section-owned object,</li>
  <li>avoid stacking multiple unrelated structural violations in the same fixture.</li>
</ul>

<p>
The goal is not to create a noisy invalid file.
The goal is to expose one sharply-owned structural rejection boundary.
</p>

<hr/>

<h2>Repository role</h2>

<p>
This case belongs in <code>Conformance/invalid/</code> because it publishes a source-shape rejection that implementations must classify consistently.
</p>

<p>
It does not redefine the source specification.
It makes the published structural boundary testable.
</p>

<pre><code>Expression/    -&gt; owns the source-shape rule
Conformance/   -&gt; exposes the rejection publicly
Implementation -&gt; must reject accordingly
</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case states a simple but important public rule:
</p>

<pre><code>wrong section placement
must be rejected
as structural canonical-source invalidity
</code></pre>

<p>
It exists to prevent silent source repair and to keep section ownership explicit in canonical FROG source.
</p>
