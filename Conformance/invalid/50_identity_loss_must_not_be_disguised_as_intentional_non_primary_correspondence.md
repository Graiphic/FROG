<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Conformance Case — Invalid: Identity Loss Must Not Be Disguised as Intentional Non-Primary Correspondence</h1>

<p align="center">
  <strong>Invalid conformance case for silent source-identity loss presented as though it were a valid non-primary IR outcome in FROG v0.1</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr />

<h2>Contents</h2>
<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-ir-reading-is-forbidden">7. Why IR Reading Is Forbidden</a></li>
  <li><a href="#why-silent-omission-is-wrong">8. Why Silent Omission Is Wrong</a></li>
  <li><a href="#illustrative-invalid-ir-shape">9. Illustrative Invalid IR Shape</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr />

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers a program, tool behavior, derivation behavior, or IR emission behavior that loses source-established identity or declaration linkage, then attempts to justify that loss by claiming that the missing contributor was simply “non-primary”.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>omitting a declaration-side contributor from recoverable IR correspondence even though recoverability is still required,</li>
  <li>deriving a primary execution object but dropping its declaration-side source relation,</li>
  <li>claiming that no correspondence record is needed because the declaration did not become a primary execution object,</li>
  <li>using silence to hide whether the contributor was intentionally non-primary or accidentally lost.</li>
</ul>

<p>
Conceptually, the forbidden collapse is:
</p>

<pre><code>identity lost
      ==
intentional non-primary outcome
</code></pre>

<p>
Base FROG v0.1 does not allow that equivalence.
</p>

<hr />

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected loadability:</strong> loadable</p>

<p><strong>Expected structural validity:</strong> valid</p>

<p><strong>Expected meaning:</strong> established or otherwise assumed by the invalid claim</p>

<p><strong>Expected IR result:</strong> not derivable as conforming canonical IR</p>

<p><strong>Expected IR schema result:</strong> not sufficient even if schema-valid shape is claimed</p>

<p><strong>Expected IR architectural result:</strong> invalid</p>

<p>
<strong>Expected rejection:</strong>
identity loss has been disguised as intentional non-primary correspondence.
</p>

<hr />

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
The invalid intent is any source shape, validator behavior, derivation behavior, or canonical IR emission behavior that relies on omission where explicit recoverability is still required.
</p>

<p>
A minimal conceptual anti-pattern is:
</p>

<pre><code>source:
  interface.inputs.gain exists
  diagram contains explicit interface_input participation

tool interpretation:
  derive one boundary object
  drop declaration linkage
  emit no correspondence carrier
  claim:
    "the declaration was just non-primary"
</code></pre>

<p>
Another invalid interpretation is:
</p>

<pre><code>widget declaration referenced by widget_reference participation
      |
      +-- declaration identity disappears in IR
      |
      +-- tool claims that no explicit linkage is needed
          because the declaration is "not executable"
</code></pre>

<p>
That is not a valid architectural reading of base FROG v0.1 where recoverability still requires explicit declaration-side correspondence.
</p>

<hr />

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>source attribution versus source-identity loss,</li>
  <li>intentional non-primary outcome versus accidental omission,</li>
  <li>declaration-reference correspondence versus disappearance,</li>
  <li>canonical IR recoverability versus implementation-private guesswork,</li>
  <li>published IR carrier discipline versus undocumented implementation folklore.</li>
</ul>

<hr />

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because base FROG v0.1 allows non-primary outcomes only when the relevant recoverability obligations remain satisfied.
</p>

<p>
More precisely, it is invalid because:
</p>

<ul>
  <li>non-primary does not mean unrecoverable,</li>
  <li>declaration-side contributors may remain outside the primary execution-object set, but their relation must still be explicit when required,</li>
  <li>canonical IR must not leave tools unable to distinguish intentional omission from accidental source-identity loss,</li>
  <li>the published IR posture includes explicit recoverability carriers such as <code>source_map[]</code> and <code>correspondence[]</code> when that boundary is in scope,</li>
  <li>schema-visible carrier presence alone would still not rescue an architecturally invalid omission.</li>
</ul>

<p>
The required distinction is:
</p>

<pre><code>intentional non-primary correspondence
              !=
silent attribution loss
</code></pre>

<hr />

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator, IR checker, or conformance reader should reject the case when the emitted or claimed IR result depends on that collapse.
</p>

<p>
It should report that at least one of the following boundaries has been violated:
</p>

<ul>
  <li>required source attribution or correspondence has been lost,</li>
  <li>non-primary outcome has not been made explicitly recoverable,</li>
  <li>declaration linkage has been replaced by silence,</li>
  <li>canonical IR carrier expectations have been violated where the published IR schema posture is in scope,</li>
  <li>the resulting IR is architecturally invalid even if some structural shape is still present.</li>
</ul>

<p>
If the omission is justified through implementation convenience, runtime economy, or “obviousness”, that should still be rejected.
None of those replaces explicit recoverability where the published architecture requires it.
</p>

<hr />

<h2 id="why-ir-reading-is-forbidden">7. Why IR Reading Is Forbidden</h2>

<p>
A conforming canonical IR reading is forbidden for this invalid case because the claimed IR result fails to preserve the recoverability required by the published IR layer.
</p>

<p>
There is no conforming basis for claiming all of the following at once:
</p>

<ul>
  <li>the source-side contributor mattered to validation or architectural explanation,</li>
  <li>the contributor did not become a primary execution object,</li>
  <li>the contributor therefore needed no explicit recoverable carrier,</li>
  <li>the resulting omission is still conforming.</li>
</ul>

<p>
That last step is exactly what base v0.1 rejects.
</p>

<hr />

<h2 id="why-silent-omission-is-wrong">8. Why Silent Omission Is Wrong</h2>

<p>
A conforming toolchain must not silently “repair” or rationalize this case by:
</p>

<ul>
  <li>dropping the contributor and calling the result “non-primary”,</li>
  <li>relying on hidden internal tables while emitting no open-IR recoverability carrier,</li>
  <li>assuming that later tools can reconstruct the missing relation from naming coincidence,</li>
  <li>treating omission as architecturally equivalent to explicit correspondence.</li>
</ul>

<p>
Such behavior would erase distinctions that the published source-to-IR boundary requires to remain explicit and reviewable.
</p>

<p>
The forbidden repair pattern is:
</p>

<pre><code>source-visible contributor exists
        -/-> therefore recoverability may be omitted

no primary object
        -/-> therefore no explicit correspondence is needed
</code></pre>

<hr />

<h2 id="illustrative-invalid-ir-shape">9. Illustrative Invalid IR Shape</h2>

<p>
A simplified invalid canonical IR reading of this anti-pattern would look like:
</p>

<pre><code>{
  "unit": {
    "objects": [
      {
        "id": "obj.boundary.in.main",
        "family": "boundary",
        "role": "primary"
      }
    ],
    "source_map": [],
    "correspondence": []
  }
}
</code></pre>

<p>
This shape is invalid for the case under test when the architectural story still depends on explicit declaration linkage or explicit non-primary recoverability.
</p>

<p>
The point is not that every case must always populate every carrier.
The point is that omission is invalid when it erases a distinction the published IR boundary requires to remain recoverable.
</p>

<hr />

<h2 id="summary">10. Summary</h2>

<p>
This case must be rejected because FROG v0.1 does not allow accidental attribution loss to masquerade as intentional non-primary correspondence.
</p>

<p>
A conforming implementation must reject source-to-IR behavior that depends on silent disappearance where explicit recoverability is still required.
</p>

<p>
The essential rule is:
</p>

<pre><code>identity loss
    does not become valid
by being renamed
"non-primary"
</code></pre>
