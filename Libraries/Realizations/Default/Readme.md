<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realizations</h1>

<p align="center">
  <strong>Official default realization family for standardized FROG widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Realizations index</a></li>
  <li><a href="../../Widgets/Readme.md">Standard widgets</a></li>
  <li><a href="./Package.md">Default realization package</a></li>
</ul>

<hr/>

<h2>Current Default Realization Families</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Manifest</th>
      <th>Category</th>
      <th>Runtime role posture</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md">Boolean</a></td><td><code>boolean.default.wfrog</code></td><td>scalar value</td><td>control / indicator</td></tr>
    <tr><td><a href="./String.md">String</a></td><td><code>string.default.wfrog</code></td><td>scalar value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Button.md">Button</a></td><td><code>button.default.wfrog</code></td><td>action</td><td>command</td></tr>
    <tr><td><a href="./Numeric.md">Numeric</a></td><td><code>numeric.default.wfrog</code></td><td>scalar value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Enum.md">Enum</a></td><td><code>enum.default.wfrog</code></td><td>scalar selection</td><td>control / indicator</td></tr>
    <tr><td><a href="./Path.md">Path</a></td><td><code>path.default.wfrog</code></td><td>path value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Picture.md">Picture</a></td><td><code>picture.default.wfrog</code></td><td>visual data</td><td>control / indicator</td></tr>
    <tr><td><a href="./Chart.md">Chart</a></td><td><code>chart.default.wfrog</code></td><td>visual data</td><td>indicator</td></tr>
    <tr><td><a href="./Listbox.md">Listbox</a></td><td><code>listbox.default.wfrog</code></td><td>selection / navigation</td><td>control / indicator</td></tr>
    <tr><td><a href="./Tab.md">Tab</a></td><td><code>tab.default.wfrog</code></td><td>selection / navigation</td><td>control / indicator</td></tr>
    <tr><td><a href="./Tree.md">Tree</a></td><td><code>tree.default.wfrog</code></td><td>selection / navigation</td><td>control / indicator</td></tr>
    <tr><td><a href="./Table.md">Table</a></td><td><code>table.default.wfrog</code></td><td>structured data</td><td>control / indicator</td></tr>
    <tr><td><a href="./Array.md">Array</a></td><td><code>array.default.wfrog</code></td><td>structured data</td><td>structured collection</td></tr>
    <tr><td><a href="./Cluster.md">Cluster</a></td><td><code>cluster.default.wfrog</code></td><td>structured data</td><td>structured aggregate</td></tr>
    <tr><td><a href="./Label.md">Label</a></td><td><code>label.default.wfrog</code></td><td>support</td><td>support</td></tr>
    <tr><td><a href="./Frame.md">Frame</a></td><td><code>frame.default.wfrog</code></td><td>support</td><td>support</td></tr>
    <tr><td><a href="./Decorations.md">Decorations</a></td><td><code>decorations.default.wfrog</code></td><td>support</td><td>decorative support</td></tr>
    <tr><td><a href="./Splitter.md">Splitter</a></td><td><code>splitter.default.wfrog</code></td><td>layout</td><td>layout</td></tr>
    <tr><td><a href="./Panel.md">Panel</a></td><td><code>panel.default.wfrog</code></td><td>layout / container</td><td>container</td></tr>
  </tbody>
</table>

<hr/>

<h2>Realization Boundary</h2>

<p>
The Default family is realization-side only.
It does not define FROG language semantics, widget class law, FIR, lowering, or runtime-private behavior.
</p>

<p>
A conforming host may render a Default realization using SVG templates, subobject composition, native widgets, or a mixed strategy, provided that the published widget class law and public part model remain preserved.
</p>

<hr/>

<h2>Simple and Composite Realizations</h2>

<ul>
  <li>Simple scalar or support widgets may use one primary SVG template.</li>
  <li>Composite widgets may use a shell plus subobject assets.</li>
  <li>Assets are templates or realization resources; they are not semantic owners of widget values.</li>
</ul>
