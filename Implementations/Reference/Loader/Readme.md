<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Loader</h1>

<p align="center">
  Canonical source intake for the non-normative FROG reference implementation<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains source-intake logic for the reference implementation.
Its role is to load canonical <code>.frog</code> artifacts into a structured in-memory form suitable for later validation.
</p>

<p>
The loader is responsible for intake.
It is not responsible for defining language truth.
</p>

<hr/>

<h2>What this Directory Owns</h2>

<ul>
  <li>file reading,</li>
  <li>basic decoding of canonical source artifacts,</li>
  <li>basic structural intake into implementation data structures,</li>
  <li>early error reporting for unreadable or malformed source payloads.</li>
</ul>

<hr/>

<h2>What this Directory Does Not Own</h2>

<ul>
  <li>canonical source specification ownership,</li>
  <li>semantic validation,</li>
  <li>primitive validation,</li>
  <li>type validation,</li>
  <li>Execution IR derivation.</li>
</ul>

<p>
If the loader needs to know what a valid <code>.frog</code> file means,
that rule belongs upstream in the published specification.
</p>

<hr/>

<h2>Expected Output</h2>

<p>
The loader should produce a structured source representation suitable for the validator.
It may preserve source locations, raw records, and ordering details where useful for diagnostics.
</p>

<p>
That structured representation is still a source-side intake form.
It is not yet validated program meaning and it is not yet Execution IR.
</p>

<hr/>

<h2>Design Rules</h2>

<ul>
  <li>Preserve source identity where possible.</li>
  <li>Do not silently repair malformed source.</li>
  <li>Report malformed source explicitly.</li>
  <li>Keep loader output close to canonical source structure.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The loader turns canonical <code>.frog</code> artifacts into structured intake data for the reference pipeline.
It is a source-ingestion layer,
not a semantic owner.
</p>
