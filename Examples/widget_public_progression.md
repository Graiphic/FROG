<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Widget Example Public Progression Boundary</h1>

<p align="center">
  <strong>Repository-visible widget example boundary for the public FROG specification</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document records the public boundary for repository-visible widget examples.
It is not Graiphic's internal widget validation recipe, private roadmap, or
production runtime implementation plan.
</p>

<p>
Widget examples are useful because they make the relationship between
<code>.frog</code> source, FIR, lowering, backend contracts, <code>.wfrog</code>
realization packages, Default SVG assets, runtime snapshots, and diagnostics
inspectable. They do not replace the owning specification documents.
</p>

<hr/>

<h2>Current Public Runtime Closure</h2>

<p>
Examples <code>01</code> through <code>15</code> define the current public
reference runtime closure.
</p>

<ul>
  <li>Examples <code>01</code> through <code>04</code> cover small executable graph and UI/value participation slices.</li>
  <li>Example <code>05</code> is the bounded UI accumulator corridor.</li>
  <li>Examples <code>06</code> through <code>09</code> cover scalar widget value roundtrips.</li>
  <li>Examples <code>10</code> through <code>15</code> cover the published Button mechanical-action family.</li>
</ul>

<p>
Examples beyond Example <code>15</code> may remain public as
specification-facing, widget-facing, conformance-facing, or design-progression
material. They are not, by presence alone, part of the current public reference
runtime closure.
</p>

<hr/>

<h2>Public Widget Example Principles</h2>

<ul>
  <li>The public repository can publish source, FIR, lowering, contract, realization, asset, and conformance-facing evidence for a widget example.</li>
  <li>Public examples should preserve the ownership split between <code>.frog</code>, <code>.wfrog</code>, Default realizations, backend artifacts, and runtime consumers.</li>
  <li>Public examples should not imply that Graiphic production runtime code must be published for every future example.</li>
  <li>Graiphic production runtime work for post-boundary examples may continue in proprietary repositories unless a later public reference surface is explicitly promoted.</li>
</ul>

<hr/>

<h2>Current Post-Boundary Direction</h2>

<p>
Example <code>16_picture_logo_jpeg</code> starts the repository-visible
post-boundary widget progression. It publishes a Path control, the standard
<code>frog.image.decode_file_rgba8</code> library contract, and a Picture
indicator backed by Default Path/Picture realizations. It may publish source,
library, FIR/lowering, and realization material without expanding the public
runtime implementation beyond the Example <code>15</code> closure.
</p>

<p>
Future public widget examples should remain small, explicit, and inspectable.
Graiphic's internal sequencing, manual visual QA procedure, private runtime work,
and acceptance recipes are intentionally not published here.
</p>
