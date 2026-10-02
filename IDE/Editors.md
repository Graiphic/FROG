<h1>FROG Editors and Authoring Tools</h1>

<p>
FROG is a public graphical dataflow language specification stewarded by Graiphic.
Graiphic Studio is the lead authoring implementation developed by the steward.
Portable syntax and semantics are documented publicly so that independent
editors, generators, validators, compilers and runtimes can implement the same
contracts. A FROG-compatible editor consumes those contracts and declares its
supported source versions and profiles.
</p>

<p>
This page tracks editor and authoring-tool efforts that are being developed
around FROG. Listing an editor here does not make that editor the language, nor
does it make the editor a normative implementation of FROG semantics.
</p>

<p>The <a href="../Expression/Frog%20source%20guide.md">source syntax guide</a>
explains canonical JSON. The
<a href="../docs/studio-source-compatibility.md">Studio integration record</a>
states the current draft/public-envelope difference and remaining qualification
gates. The Source window showing a file is not proof of canonical export,
complete profile validation or target execution.</p>

<hr/>

<h2 id="current-editor-efforts">Current editor efforts</h2>

<table>
  <thead>
    <tr>
      <th>Editor</th>
      <th>Status</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><a href="https://github.com/Graiphic/Graiphic-Studio">Graiphic Studio</a></td>
      <td>In development</td>
      <td>Graiphic's user-facing authoring environment with coordinated Front Panel, Block Diagram, and live Source views for creating and editing <code>.frog</code> documents.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="editor-boundaries">Editor boundaries</h2>

<p>
A FROG editor may provide:
</p>

<ul>
  <li>front-panel editing,</li>
  <li>diagram editing,</li>
  <li>widget placement and configuration,</li>
  <li>interface-map editing,</li>
  <li>icon editing,</li>
  <li>validation feedback,</li>
  <li>execution preparation,</li>
  <li>debugging and observability views.</li>
</ul>

<p>
Those conveniences remain tooling behavior. They must serialize to explicit
FROG source artifacts and consume specification-owned contracts rather than
creating hidden editor-only semantics.
</p>

<pre><code>Editor behavior
      |
      v
Program model
      |
      v
canonical .frog source
      |
      v
validation and execution-facing contracts
</code></pre>

<hr/>

<h2 id="relationship-to-the-public-specification">Relationship to the public specification</h2>

<p>
This repository remains the public home for the FROG language specification.
Editor documentation may live in separate repositories when it describes a
specific product or user interface. The public language specification keeps
owning source shape, semantics, validation expectations, interface boundaries,
widget contracts, and execution-facing handoff rules.
</p>

<p>
Graiphic Studio user documentation lives in
<a href="https://graiphic.github.io/Graiphic-Studio/">the Graiphic Studio documentation site</a>
and its source lives in
<a href="https://github.com/Graiphic/Graiphic-Studio">Graiphic/Graiphic-Studio</a>.
It should be read as product documentation for the editor, not as a replacement
for the FROG specification.
</p>

<p>
Current product documentation includes the
<a href="https://graiphic.github.io/Graiphic-Studio/#/docs/interface/window-workflow">coordinated window workflow</a>,
<a href="https://graiphic.github.io/Graiphic-Studio/#/docs/interface/widget-navigator">Widget Navigator</a>,
<a href="https://graiphic.github.io/Graiphic-Studio/#/docs/interface/function-navigator">Function Navigator</a>,
<a href="https://graiphic.github.io/Graiphic-Studio/#/docs/interface/icon-editor">Icon Editor</a>, and
<a href="https://graiphic.github.io/Graiphic-Studio/#/docs/interface/options">Studio Options</a>.
Product UI behavior remains editor documentation. Canonical source shape,
numeric representation identity, Array typing, interface-map bindings, and
other cross-tool contracts remain owned by this public specification.
</p>
