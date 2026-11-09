<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 62</h1>

<p align="center">
  <strong>Structure terminal role loss must not be treated as valid canonical execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#why-terminal-presence-is-not-enough">7. Why Terminal Presence Is Not Enough</a></li>
  <li><a href="#what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</a></li>
  <li><a href="#expected-conformance-result">9. Expected Conformance Result</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>62_structure_terminal_role_loss_must_not_be_treated_as_valid_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> a canonical execution IR representation loses recoverable structure terminal role information even though the corresponding terminal carrier still exists.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case exists to reject a specific IR collapse:</p>

<pre><code>recoverable structure terminal
+
recoverable structure terminal role
                -&gt;
terminal still present
+
role no longer recoverable
</code></pre>

<p>Presence is not enough.</p>

<p>If the canonical execution IR still contains a terminal-shaped carrier but no longer allows a conforming reader to recover the terminal role that the structure boundary depends on, then the architectural distinction has been lost.</p>

<p>This case makes explicit that terminal survival and role survival are different preservation requirements.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>recoverable structure terminal identity, and</li>
  <li>recoverable structure terminal role.</li>
</ul>

<p>The rejected collapse is:</p>

<pre><code>recoverable terminal carrier
            !=
recoverable terminal role semantics
</code></pre>

<p>A terminal that remains present but whose role is no longer recoverable is not architecturally preserved.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains a structure boundary with explicit terminals whose roles are architecturally meaningful.</p>

<p>Those roles may distinguish, for example:</p>

<ul>
  <li>entry-side versus exit-side participation,</li>
  <li>input-side versus output-side boundary behavior,</li>
  <li>selector, condition, tunnel, iteration, carry, state-related, or other structure-owned role categories where the architecture requires them,</li>
  <li>distinct boundary semantics that cannot be recovered from mere terminal existence alone.</li>
</ul>

<p>A canonical execution IR is then produced in which:</p>

<ul>
  <li>the terminal carriers still appear,</li>
  <li>the structure boundary may still appear,</li>
  <li>some adjacency or connectivity may still appear,</li>
  <li>but the role information needed to interpret the structure terminal correctly is no longer recoverable.</li>
</ul>

<p>The conformance failure occurs when that IR is still treated as valid merely because the terminal-like objects did not disappear completely.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid because canonical execution IR validity requires recoverability of structure terminal roles where those roles are part of the preserved architectural meaning.</p>

<p>More precisely, the case is invalid when at least one of the following occurs:</p>

<ul>
  <li>a structure terminal remains present but its role classification is absent,</li>
  <li>a structure terminal role is replaced by a generic undifferentiated terminal category,</li>
  <li>role meaning is left to inference from layout, incidental connectivity, or implementation folklore,</li>
  <li>distinct structure-terminal roles are collapsed into one carrier shape with no recoverable architectural distinction,</li>
  <li>role reconstruction depends on downstream runtime behavior rather than canonical IR content.</li>
</ul>

<p>Any of those situations breaks preservation even if the IR still looks superficially complete.</p>

<hr/>

<h2 id="why-terminal-presence-is-not-enough">7. Why Terminal Presence Is Not Enough</h2>
<p>Terminal presence is not enough because a structure boundary depends on more than object survival.</p>

<p>Once role information is removed, the remaining carrier may still be:</p>

<ul>
  <li>syntactically present,</li>
  <li>schema-shaped like a valid terminal object,</li>
  <li>connected to nearby IR objects,</li>
  <li>sufficient for one implementation’s private interpretation.</li>
</ul>

<p>But it is still architecturally insufficient if the published canonical IR no longer tells a conforming reader what role that terminal plays at the structure boundary.</p>

<p>The rejected shortcut is:</p>

<pre><code>terminal object exists
        therefore
terminal role is preserved
</code></pre>

<p>That inference is invalid.</p>

<hr/>

<h2 id="what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</h2>
<p>The following must not be preserved as acceptable public truth:</p>

<ul>
  <li>“the terminal is still there, so its role can be guessed later,”</li>
  <li>“role recovery can be delegated to one runtime’s structure interpreter,”</li>
  <li>“schema-valid terminal shape is sufficient even if terminal role is gone,”</li>
  <li>“connectivity around the terminal is enough to recover its architectural role,”</li>
  <li>“role metadata may be dropped because implementations already know what to do.”</li>
</ul>

<p>The rejected preservation claim is:</p>

<pre><code>carrier presence
      -/-> role recoverability
</code></pre>

<hr/>

<h2 id="expected-conformance-result">9. Expected Conformance Result</h2>
<p>This case must be rejected.</p>

<p>The rejection is not based on source invalidity and not necessarily on schema-shape invalidity.</p>

<p>The rejection is specifically based on canonical execution IR architectural invalidity caused by role-loss across derivation or construction.</p>

<p>The expected conformance reading is therefore:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may be partially or even fully constructed in schema-shaped form,</li>
  <li>but the canonical execution IR is still invalid because structure terminal role recoverability has been lost.</li>
</ul>

<hr/>

<h2 id="rationale">10. Rationale</h2>
<p>This rejection matters because the repository has already distinguished several nearby preservation layers:</p>

<pre><code>validated meaning
        -&gt;
canonical execution IR
        -&gt;
recoverable region ownership
        -&gt;
recoverable structure boundary terminals
        -&gt;
recoverable structure terminal roles
</code></pre>

<p>If terminal roles can be lost while the IR is still accepted as valid, then canonical IR preservation becomes too weak to support consistent downstream reading.</p>

<p>The result would be architectural drift such as:</p>

<ul>
  <li>terminal role meaning reconstructed differently by different implementations,</li>
  <li>boundary semantics inferred from non-canonical clues,</li>
  <li>role-sensitive structure behavior hidden behind runtime-specific interpretation,</li>
  <li>false confidence created by terminal carrier survival alone.</li>
</ul>

<p>The preserved rule remains:</p>

<pre><code>structure terminal roles
must remain recoverable
in canonical execution IR
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is not the failure point,</li>
  <li>semantic meaning may already be established,</li>
  <li>terminal carriers may still exist in canonical IR,</li>
  <li>their required structure-terminal roles are no longer recoverable,</li>
  <li>the IR is therefore architecturally invalid,</li>
  <li>the case must be rejected.</li>
</ul>

<p>The public truth asserted by this rejection is:</p>

<pre><code>structure terminal role loss
must not be treated as
valid canonical execution IR
</code></pre>
