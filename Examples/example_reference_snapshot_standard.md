<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Example Screenshot Publication Rule</h1>

<p align="center">
  <strong>Public screenshot-only evidence for repository-visible examples</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Status</h2>

<p>
This file keeps its historical name so older repository links continue to work.
As of 2026-06-17, the former public reference snapshot package standard is
deprecated for new work.
</p>

<p>
New public example publication is screenshot-only. A public example may expose
one or more readable browser-host screenshots, but it must not add public
accepted-state JSON, visual-contract files, artifact hash indexes, private
runtime evidence, or internal validation recipes.
</p>

<hr/>

<h2>Public Shape</h2>

<p>
When a visual example needs public evidence, publish the smallest useful image
artifact:
</p>

<pre><code>Examples/&lt;NN_example_name&gt;/
  reference/
    screenshot.png
</code></pre>

<p>
PNG is the preferred public format. A text-safe SVG wrapper may be used only
when it exists already or when the image publication route requires it, but the
visible result must still be inspected as an image through the public
repository path.
</p>

<p>
Older <code>reference/</code> directories may still contain historical
<code>state.accepted.json</code>, visual-contract, or artifact-index files.
They are historical archives and are not the publication model for new or
refreshed examples.
</p>

<hr/>

<h2>Screenshot Rules</h2>

<ul>
  <li>The screenshot must come from the accepted browser-host state after the latest relevant build or launch.</li>
  <li>The screenshot must be opened and visually inspected through the public repository path before closure.</li>
  <li>The screenshot must be readable, representative, and useful for later human comparison.</li>
  <li>The screenshot must not show stale hosts, error pages, raw CSS/JS leakage, clipped widget surfaces, or degraded compression.</li>
  <li>If the example changes later, refresh the screenshot or clearly leave the old image as historical.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
The screenshot is evidence, not source truth. The source remains the owning
<code>.frog</code>, directly referenced <code>.wfrog</code> packages, FIR,
lowering, backend manifest material, Default realization assets, and public
library contracts.
</p>

<p>
Screenshots must not publish Graiphic proprietary runtime source, private IDE
implementation plans, private task notes, internal validation recipes, or
runtime roadmap details.
</p>
