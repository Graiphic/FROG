<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Example Reference Snapshot Standard</h1>

<p align="center">
  <strong>Public accepted-state evidence for repository-visible examples</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
An example reference snapshot records the public accepted state of a visible
example at a specific validation checkpoint. It helps readers inspect what was
accepted without relying on chat memory, private runtime code, or local browser
state.
</p>

<p>
The snapshot is evidence, not source truth. The source remains the owning
<code>.frog</code>, directly referenced <code>.wfrog</code> packages, FIR,
lowering, backend manifest material, Default realization assets, and public
library contracts.
</p>

<hr/>

<h2>Directory Shape</h2>

<p>
Validated examples may include a <code>reference/</code> directory:
</p>

<pre><code>Examples/&lt;NN_example_name&gt;/
  reference/
    README.md
    screenshot.png
    state.accepted.json
    visual-contract.md
    visual-contract.json
    artifact-index.json
</code></pre>

<p>
The directory is intentionally small. It should not duplicate source artifacts
that already exist in the example dossier.
</p>

<hr/>

<h2>Required Files</h2>

<table>
  <thead>
    <tr>
      <th>File</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>README.md</code></td>
      <td>Explains the snapshot boundary, embeds the accepted screenshot directly, and links the accepted artifacts.</td>
    </tr>
    <tr>
      <td><code>screenshot.png</code></td>
      <td>Browser-host capture of the accepted visible state.</td>
    </tr>
    <tr>
      <td><code>state.accepted.json</code></td>
      <td>Accepted public runtime snapshot, including visible values, consumed assets, diagnostics, and artifact state exposed by the example.</td>
    </tr>
    <tr>
      <td><code>visual-contract.md</code></td>
      <td>Human-readable appearance contract: widget posture, important dimensions, visible states, labels, borders, spacing, and source-owned customization surfaces.</td>
    </tr>
    <tr>
      <td><code>visual-contract.json</code></td>
      <td>Machine-readable summary of the same visible contract, suitable for future regression tooling.</td>
    </tr>
    <tr>
      <td><code>artifact-index.json</code></td>
      <td>Relative paths and hashes for the source, realization, FIR, lowering, manifest, and fixture artifacts that define the accepted example.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Public Boundary</h2>

<p>
Reference snapshots are public example evidence. They must not contain
Graiphic proprietary runtime source, internal validation recipes, private IDE
implementation plans, private task notes, or private runtime roadmap details.
</p>

<p>
For post-public-runtime-boundary examples, a snapshot may show an accepted
browser-host surface produced by Graiphic private runtime work. That does not
promote the private runtime implementation into the public repository and does
not make the private runtime mandatory for independent FROG implementations.
</p>

<hr/>

<h2>Artifact Rules</h2>

<ul>
  <li>Reference files link to existing source artifacts instead of copying them.</li>
  <li>The snapshot README should show the accepted screenshot inline so a GitHub reader can see the visual state without browsing for the image file.</li>
  <li>The snapshot README should provide clear named links to the screenshot, accepted state JSON, visual contract, machine-readable visual contract, and artifact index.</li>
  <li><code>artifact-index.json</code> records relative paths and stable hashes for traceability.</li>
  <li>Generated screenshots and accepted state files are refreshed only after the example has been visually accepted at its stated validation level.</li>
  <li>If the example changes later, the snapshot must either be refreshed or clearly marked as historical.</li>
  <li>A snapshot must not overclaim generalized runtime completeness beyond the example's README boundary.</li>
</ul>

<hr/>

<h2>Visual Contract Rules</h2>

<p>
The visual contract describes the accepted appearance in engineering terms:
which widget classes appear, which Default realization assets are consumed,
which parts are visible, which state surfaces matter, and which properties are
source-owned or realization-published.
</p>

<p>
It should include the surfaces that would make a regression obvious: labels,
index displays, frames, padding, repeated cell posture, scrollbars, selections,
disabled cells, hover or pressed states, and control/indicator differences when
those surfaces are relevant to the example.
</p>

<hr/>

<h2>Acceptance Note</h2>

<p>
The presence of a reference snapshot means only that the visible example state
was accepted at the boundary documented by that example. It does not redefine
FROG semantics, widget class law, or runtime architecture.
</p>
