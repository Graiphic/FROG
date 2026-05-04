<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 03 — UI Property Write</h1>

<p align="center">
  <strong>Minimal object-style widget interaction example using widget_reference and frog.ui.property_write</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/03_ui_property_write/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>interface_input(status) --- value ---\
                                     frog.ui.property_write(label.text)
widget_reference(ctrl_gain) -- ref -/
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py --source Examples/03_ui_property_write/main.frog --expected Examples/03_ui_property_write/main.fir.json --check
python Implementations/Reference/Lowerer/lower_fir.py --fir Examples/03_ui_property_write/main.fir.json --expected Examples/03_ui_property_write/main.lowering.json --check
</code></pre>

<hr/>

<h2>Boundary</h2>

<p>This example is a UI effect derivation and lowering slice. It does not currently claim backend contract, runtime acceptance, or LLVM closure.</p>
