<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Conformance Case — Invalid: Region Without Recoverable Structure Owner Is Invalid</h1>

<p align="center">
  <strong>Invalid conformance case for a canonical Execution IR region whose owning structure relation is missing, ambiguous, or unrecoverable in FROG v0.1</strong><br/>
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
  <li><a href="#why-region-without-owner-is-forbidden">7. Why Region Without Owner Is Forbidden</a></li>
  <li><a href="#illustrative-invalid-ir-shape">8. Illustrative Invalid IR Shape</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr />

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers any derivation, construction, emission, or conformance claim in which a region appears in canonical Execution IR but its owning structure relation is absent, ambiguous, contradictory, or recoverable only through private implementation knowledge.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>emitting a region record with no owner reference,</li>
  <li>emitting a structure record whose region references do not match the region-side owner relation,</li>
  <li>claiming that ownership can be reconstructed from ordering or containment convention alone,</li>
  <li>flattening structure ownership away while still keeping region-shaped records for convenience.</li>
</ul>

<hr />

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected loadability:</strong> loadable</p>

<p><strong>Expected structural validity:</strong> valid or apparently valid at a superficial carrier level</p>

<p><strong>Expected meaning:</strong> established or otherwise assumed by the invalid claim</p>

<p><strong>Expected IR result:</strong> not derivable as conforming canonical IR</p>

<p><strong>Expected IR schema result:</strong> not sufficient even if some structural shape appears acceptable</p>

<p><strong>Expected IR architectural result:</strong> invalid</p>

<p>
<strong>Expected rejection:</strong>
a region has been left without a recoverable owning structure relation.
</p>

<hr />

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
The anti-pattern under test is:
</p>

<pre><code>region exists
      therefore
ownership may remain implicit
</code></pre>

<p>
A minimal conceptual example is:
</p>

<pre><code>tool behavior:
  derive one structure
  derive one region-like record
  drop explicit owner relation
  claim:
    "the owner is obvious from local grouping"
</code></pre>

<p>
Another invalid variation is:
</p>

<pre><code>structure object says region_ids = ["region.a"]
region.a says owner_object_id = "obj.other"

tool still claims canonical IR validity
</code></pre>

<p>
That is not a valid open-IR reading in base FROG v0.1.
</p>

<hr />

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>region identity versus recoverable region ownership,</li>
  <li>structure-preserving IR versus structure-like but ownerless grouping,</li>
  <li>explicit open-IR structure relation versus private nesting convention,</li>
  <li>carrier presence versus true structural recoverability.</li>
</ul>

<hr />

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because base FROG v0.1 keeps structured control explicit in canonical Execution IR.
A region that remains explicit must also remain explicitly owned.
</p>

<p>
It is invalid because:
</p>

<ul>
  <li>region identity alone is not sufficient,</li>
  <li>structured control requires recoverable region ownership,</li>
  <li>private hidden nesting is not a substitute for explicit ownership,</li>
  <li>region-local content must remain attached to its structure-owned scope,</li>
  <li>schema-like carrier presence would still be insufficient if ownership is not recoverable.</li>
</ul>

<p>
The required rule is:
</p>

<pre><code>explicit region
      !=
recoverable region ownership by itself
</code></pre>

<hr />

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator, IR checker, or conformance reader should reject the case when the region-side ownership relation is missing, contradictory, or unrecoverable.
</p>

<p>
It should report that at least one of the following boundaries has failed:
</p>

<ul>
  <li>required structure ownership recoverability,</li>
  <li>required coherence between structure-side and region-side ownership carriers,</li>
  <li>required attachment of region-local content to the owning structure scope,</li>
  <li>required architectural validity of explicit structured control in canonical IR.</li>
</ul>

<hr />

<h2 id="why-region-without-owner-is-forbidden">7. Why Region Without Owner Is Forbidden</h2>

<p>
A conforming toolchain must not rationalize this case by:
</p>

<ul>
  <li>keeping ownership only in private nesting metadata,</li>
  <li>assuming later tools can recover ownership from region ordering,</li>
  <li>pretending that a region record can remain open-IR valid while its owner is only “obvious”,</li>
  <li>using region-shaped records as loose grouping artifacts after flattening away structure identity.</li>
</ul>

<p>
Such behavior would keep the appearance of structured IR while dropping one of the key architectural guarantees of structured control.
</p>

<p>
The forbidden repair pattern is:
</p>

<pre><code>region is explicit
        -/-> therefore ownership may remain implicit
</code></pre>

<hr />

<h2 id="illustrative-invalid-ir-shape">8. Illustrative Invalid IR Shape</h2>

<p>
A simplified invalid canonical IR reading of this anti-pattern would look like:
</p>

<pre><code>{
  "unit": {
    "objects": [
      {
        "id": "obj.loop.main",
        "family": "structure",
        "role": "primary",
        "structure_family": "while_loop"
      }
    ],
    "regions": [
      {
        "id": "region.loop.body",
        "member_object_ids": ["obj.loop.body.add"]
      }
    ]
  }
}
</code></pre>

<p>
This shape is invalid for the case under test because the region remains explicit but its owning structure relation is not recoverable.
</p>

<p>
A contradictory ownership reading would also be invalid:
</p>

<pre><code>structure says one owner relation
region says another
</code></pre>

<p>
The point is not merely that a field is missing.
The point is that the structural IR boundary has lost one of its required recoverable ownership relations.
</p>

<hr />

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because FROG v0.1 does not allow an explicit region in canonical Execution IR to remain without a recoverable owning structure relation.
</p>

<p>
A conforming implementation must reject region-shaped IR that preserves the appearance of structured control while losing explicit structure ownership.
</p>

<p>
The essential rule is:
</p>

<pre><code>explicit region
    is not valid
without recoverable structure owner
in canonical Execution IR
</code></pre>
