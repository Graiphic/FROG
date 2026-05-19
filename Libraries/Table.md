<h1 align="center">FROG Table Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.table</strong> value and primitive surface for FROG v0.1<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary">2. Boundary</a></li>
  <li><a href="#value-shape">3. Value Shape</a></li>
  <li><a href="#published-surface">4. Published Surface</a></li>
  <li><a href="#primitive-contracts">5. Primitive Contracts</a></li>
  <li><a href="#effects-status-and-fir">6. Effects, Status, and FIR</a></li>
  <li><a href="#candidate-deferrals">7. Candidate Deferrals</a></li>
  <li><a href="#non-goals">8. Non-goals</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
<code>frog.table</code> defines a portable tabular value contract and the
first value-only primitives for constructing, inspecting, extracting, updating,
and formatting table data. It is a data library, not a table-widget helper
library and not a database or spreadsheet engine.
</p>

<p>
Table widgets may display or edit <code>frog.table</code> values, but the
computation that builds or transforms those values remains in ordinary diagram
nodes.
</p>

<hr/>

<h2 id="boundary">2. Boundary</h2>

<pre><code>frog.table
  owns portable rectangular table values and pure table transforms

table widgets
  display/edit table values and expose widget interaction

frog.text
  owns general text operations

profiles
  own database, spreadsheet, CSV file I/O, and external data-frame integration</code></pre>

<p>
The v0.1 table surface is intentionally flat and rectangular. It does not
define hierarchical tables, pivot tables, formula cells, database result-set
handles, or host-native grid virtualization.
</p>

<hr/>

<h2 id="value-shape">3. Value Shape</h2>

<p>
The minimal table value shape is:
</p>

<pre><code>{
  "kind": "frog.table",
  "columns": array&lt;string&gt;,
  "rows": array&lt;array&lt;string&gt;&gt;
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>columns</code> contains display headers and stable column order.</li>
  <li><code>rows</code> contains row-major cell text.</li>
  <li>Every row length MUST equal <code>column_count</code>.</li>
  <li>Column count MAY be zero only when row count is also zero.</li>
</ul>

<p>
The v0.1 table value is text-cell oriented on purpose. Typed cells, column
schemas, variants, row identifiers, metadata, and validation states remain
deferred until the record/cluster and status corridors are settled. This keeps
the first <code>frog.table</code> value compatible with the current public type
system's built-in arrays and strings.
</p>

<hr/>

<h2 id="published-surface">4. Published Surface</h2>

<table>
  <thead>
    <tr>
      <th>Primitive</th>
      <th>Inputs</th>
      <th>Outputs</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.table.build</code></td>
      <td><code>columns</code>, <code>rows</code></td>
      <td><code>table</code></td>
      <td>Create a table value.</td>
    </tr>
    <tr>
      <td><code>frog.table.empty</code></td>
      <td><code>columns</code></td>
      <td><code>table</code></td>
      <td>Create an empty table with headers.</td>
    </tr>
    <tr>
      <td><code>frog.table.row_count</code></td>
      <td><code>table</code></td>
      <td><code>count</code></td>
      <td>Return the number of rows.</td>
    </tr>
    <tr>
      <td><code>frog.table.column_count</code></td>
      <td><code>table</code></td>
      <td><code>count</code></td>
      <td>Return the number of columns.</td>
    </tr>
    <tr>
      <td><code>frog.table.columns</code></td>
      <td><code>table</code></td>
      <td><code>columns</code></td>
      <td>Return column headers.</td>
    </tr>
    <tr>
      <td><code>frog.table.rows</code></td>
      <td><code>table</code></td>
      <td><code>rows</code></td>
      <td>Return row-major cell text.</td>
    </tr>
    <tr>
      <td><code>frog.table.cell</code></td>
      <td><code>table</code>, <code>row</code>, <code>column</code></td>
      <td><code>value</code></td>
      <td>Return one cell.</td>
    </tr>
    <tr>
      <td><code>frog.table.with_cell</code></td>
      <td><code>table</code>, <code>row</code>, <code>column</code>, <code>value</code></td>
      <td><code>result</code></td>
      <td>Return a table with one replaced cell.</td>
    </tr>
    <tr>
      <td><code>frog.table.row</code></td>
      <td><code>table</code>, <code>row</code></td>
      <td><code>values</code></td>
      <td>Return one row.</td>
    </tr>
    <tr>
      <td><code>frog.table.column</code></td>
      <td><code>table</code>, <code>column</code></td>
      <td><code>values</code></td>
      <td>Return one column.</td>
    </tr>
    <tr>
      <td><code>frog.table.append_row</code></td>
      <td><code>table</code>, <code>values</code></td>
      <td><code>result</code></td>
      <td>Return a table with one row appended.</td>
    </tr>
    <tr>
      <td><code>frog.table.with_columns</code></td>
      <td><code>table</code>, <code>columns</code></td>
      <td><code>result</code></td>
      <td>Return a table with replacement headers.</td>
    </tr>
    <tr>
      <td><code>frog.table.to_delimited_text</code></td>
      <td><code>table</code>, <code>delimiter</code>, <code>include_header</code></td>
      <td><code>text</code></td>
      <td>Format a table as simple delimited text.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="primitive-contracts">5. Primitive Contracts</h2>

<h3><code>frog.table.build</code></h3>

<ul>
  <li><code>columns</code>: <code>array&lt;string&gt;</code></li>
  <li><code>rows</code>: <code>array&lt;array&lt;string&gt;&gt;</code></li>
  <li>Every row length MUST equal the number of columns.</li>
  <li>If <code>columns</code> is empty, <code>rows</code> MUST also be empty.</li>
</ul>

<h3><code>frog.table.empty</code></h3>

<ul>
  <li>Creates a table with the supplied columns and zero rows.</li>
  <li>Duplicate column names are allowed as display text in v0.1; stable typed column ids are deferred.</li>
</ul>

<h3>Structural queries</h3>

<ul>
  <li><code>frog.table.row_count</code> returns <code>u64</code>.</li>
  <li><code>frog.table.column_count</code> returns <code>u64</code>.</li>
  <li><code>frog.table.columns</code> returns <code>array&lt;string&gt;</code>.</li>
  <li><code>frog.table.rows</code> returns <code>array&lt;array&lt;string&gt;&gt;</code>.</li>
</ul>

<h3>Cell, row, and column access</h3>

<ul>
  <li><code>row</code> and <code>column</code> index inputs have type <code>u64</code>.</li>
  <li>Indexes MUST be in range.</li>
  <li><code>frog.table.cell</code> returns <code>string</code>.</li>
  <li><code>frog.table.row</code> returns <code>array&lt;string&gt;</code> with length <code>column_count</code>.</li>
  <li><code>frog.table.column</code> returns <code>array&lt;string&gt;</code> with length <code>row_count</code>.</li>
</ul>

<h3>Pure updates</h3>

<ul>
  <li><code>frog.table.with_cell</code> returns a new table value; it does not mutate the input table in place.</li>
  <li><code>frog.table.append_row</code> requires the new row length to equal <code>column_count</code>.</li>
  <li><code>frog.table.with_columns</code> requires the replacement header count to equal the current <code>column_count</code>.</li>
</ul>

<h3><code>frog.table.to_delimited_text</code></h3>

<ul>
  <li><code>delimiter</code> has type <code>string</code> and MUST be non-empty.</li>
  <li><code>include_header</code> has type <code>bool</code>.</li>
  <li>The result has type <code>string</code>.</li>
  <li>Cells containing the delimiter, quotes, or line breaks are quoted with double quotes. Embedded double quotes are escaped by doubling them.</li>
  <li>This is value formatting only; it does not read or write files.</li>
</ul>

<hr/>

<h2 id="effects-status-and-fir">6. Effects, Status, and FIR</h2>

<p>
Every published <code>frog.table</code> primitive has:
</p>

<ul>
  <li><code>call_class = standard_library_value</code></li>
  <li><code>effect = pure</code></li>
  <li><code>status_model = none</code></li>
  <li>no provider requirement</li>
</ul>

<p>
The library uses validation preconditions rather than local status outputs.
Non-rectangular row data, out-of-range indexes, invalid replacement row length,
invalid replacement column length, and empty delimiters are validation or
execution-profile failures until the uniform status corridor is standardized.
</p>

<p>
FIR SHOULD preserve table calls as named public library calls with
<code>library_id = "frog.table"</code>. The table value may be represented as
a library value object with explicit <code>columns</code> and <code>rows</code>
fields, or as an equivalent FIR support value. FIR MUST NOT replace a table
value with a table-widget instance, a host grid handle, a database cursor, or a
private runtime cache.
</p>

<hr/>

<h2 id="candidate-deferrals">7. Candidate Deferrals</h2>

<ul>
  <li>Typed cells, column schemas, and cell variants.</li>
  <li>Row ids, metadata, validation state, formatting state, and cell-level presentation state.</li>
  <li>Sorting and filtering as value transforms distinct from widget view state.</li>
  <li>CSV, TSV, spreadsheet, database, and data-frame import/export.</li>
  <li>Streaming table rows and lazy result sets.</li>
  <li>Record/cluster-backed rows after the base record/cluster value model is decided.</li>
</ul>

<hr/>

<h2 id="non-goals">8. Non-goals</h2>

<ul>
  <li>Do not make <code>frog.table</code> a wrapper for one host grid control.</li>
  <li>Do not hide database or spreadsheet connectivity inside the intrinsic table library.</li>
  <li>Do not treat Table widget viewport, selection, sorting, or editing state as the table value itself.</li>
  <li>Do not standardize arbitrary heterogeneous records through table rows in v0.1.</li>
</ul>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
<code>frog.table</code> publishes a small text-cell rectangular table value and
pure table primitives for construction, inspection, extraction, replacement,
row append, header replacement, and delimited-text formatting. It keeps table
data separate from widgets, databases, spreadsheets, and private host grid
implementations.
</p>

<hr/>

<p align="center">
End of FROG Table Library Specification
</p>
