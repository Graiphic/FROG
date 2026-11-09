<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference UI Host</h1>

<p align="center">
  Host-side UI binding layer for the non-normative FROG reference implementation<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains the host-side UI binding layer used by the first reference runtime family.
Its role is to connect runtime execution with front-panel-oriented interaction when the accepted backend contract declares UI participation support.
</p>

<hr/>

<h2>Scope of the First Slice</h2>

<p>
The first slice should remain intentionally narrow.
It should support:
</p>

<ul>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li><code>frog.ui.property_read</code>,</li>
  <li><code>frog.ui.property_write</code>,</li>
  <li><code>frog.ui.method_invoke</code>.</li>
</ul>

<p>
It should not silently expand that into a first-class standardized event execution model.
</p>

<hr/>

<h2>What this Directory Owns</h2>

<ul>
  <li>runtime-to-UI binding mechanics for the reference family,</li>
  <li>widget handle resolution for the reference runtime,</li>
  <li>host-side property and method dispatch for standardized UI primitives,</li>
  <li>family-specific refresh and commit mechanics.</li>
</ul>

<hr/>

<h2>What this Directory Does Not Own</h2>

<ul>
  <li>the normative widget model,</li>
  <li>the normative distinction between <code>widget_value</code> and <code>widget_reference</code>,</li>
  <li>a universal UI runtime architecture,</li>
  <li>a standardized first-class event model for all backends.</li>
</ul>

<hr/>

<h2>Design Rules</h2>

<ul>
  <li>Preserve the distinction between natural value participation and object-style interaction.</li>
  <li>Preserve the distinction between widget-reference participation and standardized UI-object primitive operations.</li>
  <li>Keep host-side UI mechanics private to the reference family.</li>
  <li>Do not let toolkit-specific behavior redefine FROG meaning.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The UI host is the bridge between the first reference runtime and host-side widget behavior.
It is intentionally narrow,
family-specific,
and downstream from the published UI and backend-contract boundaries.
</p>
