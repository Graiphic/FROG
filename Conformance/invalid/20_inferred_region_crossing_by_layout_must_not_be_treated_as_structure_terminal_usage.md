<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Inferred Region Crossing by Layout Must Not Be Treated as Structure Terminal Usage</h1>

<p><strong>Invalid conformance case for collapsing apparent region crossing by layout into explicit structure-terminal usage in FROG v0.1</strong><br>
FROG — Free Open Graphical Language</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-derivation-is-forbidden">7. Why Derivation Is Forbidden</a></li>
  <li><a href="#why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr>

<h2 id="case-overview">1. Case Overview</h2>

<p>This invalid case covers a source shape or tool behavior that treats apparent region crossing, frame contact, geometric overlap, or visual line placement as though they were sufficient to create explicit structure-terminal usage.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>treating a wire that appears to cross a structure frame as though it used a structure terminal,</li>
  <li>treating geometric contact with a boundary as though it created legal cross-region dataflow,</li>
  <li>treating line routing through a framed area as though it created structure input or output semantics,</li>
  <li>promoting a visually suggestive edge into a structure terminal without explicit declaration,</li>
  <li>using apparent region crossing as a substitute for explicit terminal law.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>inferred region crossing by layout
                 ==
explicit structure-terminal usage
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires apparent region crossing to act as structure-terminal law.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>diagram layout

outside value ---- line visually crossing a frame ---- inside node

tool interpretation:
  "the line crosses the structure boundary,
   therefore a structure terminal exists"

tool interpretation:
  "the apparent crossing is enough to authorize
   inside/outside value transfer"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>visual boundary crossing exists
    therefore
terminal usage exists
</code></pre>

<p>or:</p>

<pre><code>line passes through a framed region
    therefore
legal structure-boundary dataflow exists
</code></pre>

<p>without an explicit published rule establishing such terminal semantics.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>explicit structure-terminal declaration,</li>
  <li>explicit cross-boundary terminal usage,</li>
  <li>structure-boundary dataflow versus line geometry,</li>
  <li>legal region crossing versus apparent frame crossing,</li>
  <li>rejection of semantic collapse across structure-terminal law and visual routing.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li>structure-boundary crossing arises from explicit structure-terminal declaration and usage,</li>
  <li>line routing belongs to source presentation and recoverability,</li>
  <li>frame contact does not create terminal semantics,</li>
  <li>apparent region crossing does not create legal inside/outside value transfer,</li>
  <li>layout geometry does not become structure-terminal law.</li>
</ul>

<p>Treating apparent crossing as terminal usage therefore contradicts the published architecture.</p>

<p>The required separation is:</p>

<pre><code>explicit structure-terminal law
          !=
apparent boundary crossing by layout
</code></pre>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires visual or geometric region crossing to be accepted as structure-terminal usage.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li>explicit terminal declaration versus inferred terminal declaration,</li>
  <li>legal cross-boundary dataflow versus visual frame crossing,</li>
  <li>terminal semantics versus line routing,</li>
  <li>structure law versus layout geometry.</li>
</ul>

<p>If the case attempts to justify the collapse through editor convenience, readability heuristics, automatic routing, frame recovery, or backend preference, that should still be rejected. None of those creates language law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate structure-terminal crossing.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>a structure input or output terminal from a line crossing alone,</li>
  <li>a legal internal/external value transfer from apparent frame contact alone,</li>
  <li>a structure-boundary dependency from geometry alone,</li>
  <li>or a backend-facing contract that pretends visually crossing wires were explicit structure terminals all along.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>creating synthetic structure terminals from line geometry,</li>
  <li>rewriting apparent frame crossing into terminal usage,</li>
  <li>using routing overlap as justification for cross-boundary dataflow,</li>
  <li>treating a line that appears to pass through a structure frame as though it had legal terminal semantics,</li>
  <li>resolving ambiguous layout by inventing hidden structure-terminal usage while still claiming conformance.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>apparent frame crossing exists
    -/-> structure terminal exists

line routing exists
    -/-> legal region crossing exists

layout geometry
    -/-> structure-terminal law
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats explicit structure-terminal usage and inferred region crossing by layout as distinct published categories.</p>

<p>A conforming implementation must reject any source or tool behavior that depends on treating apparent region crossing as though it created structure-terminal usage.</p>

<p>The essential rule is:</p>

<pre><code>inferred region crossing by layout
    does not create
structure-terminal usage
</code></pre>
