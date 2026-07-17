<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Numeric Representation Contract</h1>

<p align="center">
  <strong>Canonical source tokens, display names, validation, and visual type-family guidance</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#source-shape">2. Canonical Source Shape</a></li>
  <li><a href="#main-representations">3. Main Representations</a></li>
  <li><a href="#advanced-representations">4. Advanced Representations</a></li>
  <li><a href="#parameterized-representations">5. Parameterized Representations</a></li>
  <li><a href="#defaults">6. Defaults</a></li>
  <li><a href="#arrays-and-bindings">7. Arrays, Terminals, and Bindings</a></li>
  <li><a href="#visual-guidance">8. Visual Type-Family Guidance</a></li>
  <li><a href="#compatibility">9. Compatibility and Migration</a></li>
  <li><a href="#validation">10. Validation Rules</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the canonical numeric representation vocabulary used by
FROG source objects, widget values, diagram terminals, array element types,
interface ports, validation, and execution-facing derivation.
</p>

<p>
The representation token is semantic. Palette grouping, menu layout, icon
artwork, header abbreviations, and colors are discoverability and presentation
surfaces. They MUST NOT replace the canonical representation token.
</p>

<hr/>

<h2 id="source-shape">2. Canonical Source Shape</h2>

<pre><code>"data_type": {
  "representation": "f64",
  "named_numeric_size": "Float64"
}</code></pre>

<ul>
  <li><code>data_type.representation</code> is the canonical compact machine-readable token.</li>
  <li><code>data_type.named_numeric_size</code> is the canonical human-readable name.</li>
  <li><code>representation.kind</code> MAY be accepted as a compatibility alias for <code>data_type.representation</code>.</li>
  <li>When more than one of these fields is present, every field MUST identify the same representation.</li>
</ul>

<p>
An IDE MAY show a short header such as <code>F64</code> or <code>U16</code> in a
menu or terminal icon. The short header is not a source token.
</p>

<hr/>

<h2 id="main-representations">3. Main Representations</h2>

<table>
  <thead>
    <tr><th>Canonical name</th><th>Token</th><th>Short header</th><th>Meaning</th></tr>
  </thead>
  <tbody>
    <tr><td><code>Float16</code></td><td><code>f16</code></td><td><code>F16</code></td><td>IEEE 754 binary16 floating point</td></tr>
    <tr><td><code>BFloat16</code></td><td><code>bf16</code></td><td><code>BF16</code></td><td>bfloat16 floating point</td></tr>
    <tr><td><code>Float32</code></td><td><code>f32</code></td><td><code>F32</code></td><td>IEEE 754 binary32 floating point</td></tr>
    <tr><td><code>Float64</code></td><td><code>f64</code></td><td><code>F64</code></td><td>IEEE 754 binary64 floating point</td></tr>
    <tr><td><code>Int8</code></td><td><code>i8</code></td><td><code>I8</code></td><td>signed 8-bit integer</td></tr>
    <tr><td><code>Int16</code></td><td><code>i16</code></td><td><code>I16</code></td><td>signed 16-bit integer</td></tr>
    <tr><td><code>Int32</code></td><td><code>i32</code></td><td><code>I32</code></td><td>signed 32-bit integer</td></tr>
    <tr><td><code>Int64</code></td><td><code>i64</code></td><td><code>I64</code></td><td>signed 64-bit integer</td></tr>
    <tr><td><code>UInt8</code></td><td><code>u8</code></td><td><code>U8</code></td><td>unsigned 8-bit integer</td></tr>
    <tr><td><code>UInt16</code></td><td><code>u16</code></td><td><code>U16</code></td><td>unsigned 16-bit integer</td></tr>
    <tr><td><code>UInt32</code></td><td><code>u32</code></td><td><code>U32</code></td><td>unsigned 32-bit integer</td></tr>
    <tr><td><code>UInt64</code></td><td><code>u64</code></td><td><code>U64</code></td><td>unsigned 64-bit integer</td></tr>
    <tr><td><code>Complex&lt;Float32&gt;</code></td><td><code>c32</code></td><td><code>CF32</code></td><td>complex number with Float32 components</td></tr>
    <tr><td><code>Complex&lt;Float64&gt;</code></td><td><code>c64</code></td><td><code>CF64</code></td><td>complex number with Float64 components</td></tr>
    <tr><td><code>FixedPoint&lt;...&gt;</code></td><td><code>fxp</code></td><td><code>FXP</code></td><td>parameterized binary fixed-point value</td></tr>
    <tr><td><code>Decimal&lt;precision,scale&gt;</code></td><td><code>decimal</code></td><td><code>DEC</code></td><td>parameterized exact base-10 decimal value</td></tr>
  </tbody>
</table>

<hr/>

<h2 id="advanced-representations">4. Advanced Representations</h2>

<p>
The <em>advanced</em> grouping is an authoring-discoverability tier. It does not
change the semantic status of a representation.
</p>

<table>
  <thead>
    <tr><th>Canonical name</th><th>Token</th><th>Short header</th><th>Meaning</th></tr>
  </thead>
  <tbody>
    <tr><td><code>Int4</code></td><td><code>i4</code></td><td><code>I4</code></td><td>signed 4-bit integer</td></tr>
    <tr><td><code>UInt4</code></td><td><code>u4</code></td><td><code>U4</code></td><td>unsigned 4-bit integer</td></tr>
    <tr><td><code>Float8E4M3</code></td><td><code>f8e4m3</code></td><td><code>F8P</code></td><td>8-bit float favoring precision through E4M3</td></tr>
    <tr><td><code>Float8E5M2</code></td><td><code>f8e5m2</code></td><td><code>F8R</code></td><td>8-bit float favoring range through E5M2</td></tr>
    <tr><td><code>Int128</code></td><td><code>i128</code></td><td><code>I128</code></td><td>signed 128-bit integer</td></tr>
    <tr><td><code>UInt128</code></td><td><code>u128</code></td><td><code>U128</code></td><td>unsigned 128-bit integer</td></tr>
    <tr><td><code>Float80</code></td><td><code>f80</code></td><td><code>F80</code></td><td>80-bit extended floating point</td></tr>
    <tr><td><code>Float128</code></td><td><code>f128</code></td><td><code>F128</code></td><td>128-bit floating point</td></tr>
    <tr><td><code>BigUInt</code></td><td><code>biguint</code></td><td><code>BIGU</code></td><td>arbitrary-precision unsigned integer</td></tr>
  </tbody>
</table>

<p>
<code>BigInt</code> and <code>BigDecimal</code> are not part of this baseline.
A tool MUST NOT serialize them as standardized FROG representations unless a
later specification revision adds them.
</p>

<hr/>

<h2 id="parameterized-representations">5. Parameterized Representations</h2>

<p>
<code>fxp</code> and <code>decimal</code> require explicit parameters. A display
header alone is insufficient to reconstruct their semantic type.
</p>

<pre><code>"data_type": {
  "representation": "fxp",
  "named_numeric_size": "FixedPoint&lt;signed,32,16,nearest,saturate&gt;"
}

"data_type": {
  "representation": "decimal",
  "named_numeric_size": "Decimal&lt;18,4&gt;"
}</code></pre>

<p>
Implementations MAY expose structured parameter members in addition to the
canonical name. If both forms are present, they MUST agree.
</p>

<hr/>

<h2 id="defaults">6. Defaults</h2>

<p>
The standard Numeric widget default is <code>Float64</code> / <code>f64</code>.
Its default editor accepts negative and fractional values. A different
representation MUST be explicit in source.
</p>

<p>
Ring and Enum families define their own default carrier separately. Their
standard compact default is <code>UInt16</code> / <code>u16</code>.
</p>

<hr/>

<h2 id="arrays-and-bindings">7. Arrays, Terminals, and Bindings</h2>

<ul>
  <li>A homogeneous array inherits its element representation.</li>
  <li>An untyped empty array has no typed terminal and MUST NOT be bound as a typed value.</li>
  <li>After an element type is assigned, array terminal identity, type text, and type-family color derive from that element type.</li>
  <li>Changing a scalar widget representation MUST update its diagram terminal and any compatible interface-map binding without requiring a separate refresh action.</li>
  <li>Encapsulating a bound scalar widget in an array transfers the value relationship to the typed array when the operation is valid; the obsolete scalar terminal and binding MUST NOT remain as competing live identities.</li>
</ul>

<hr/>

<h2 id="visual-guidance">8. Visual Type-Family Guidance</h2>

<p>
Editors SHOULD use one consistent visual family across representation menus,
diagram terminals, array terminals, and interface-map bindings. The following
palette is the reference guidance for the baseline:
</p>

<table>
  <thead><tr><th>Family</th><th>Reference color</th></tr></thead>
  <tbody>
    <tr><td>floating point</td><td><code>#D97706</code></td></tr>
    <tr><td>signed integer</td><td><code>#2563EB</code></td></tr>
    <tr><td>unsigned integer</td><td><code>#1191B2</code></td></tr>
    <tr><td>complex</td><td><code>#E11D48</code></td></tr>
    <tr><td>fixed point</td><td><code>#7C3AED</code></td></tr>
    <tr><td>decimal</td><td><code>#B45309</code></td></tr>
  </tbody>
</table>

<p>
Color is a redundant visual cue only. Type identity MUST remain available from
canonical source and MUST NOT depend on color perception.
</p>

<hr/>

<h2 id="compatibility">9. Compatibility and Migration</h2>

<ul>
  <li><code>dbl</code> MAY be migrated to <code>f64</code> / <code>Float64</code>.</li>
  <li><code>sgl</code> MAY be migrated to <code>f32</code> / <code>Float32</code>.</li>
  <li><code>cdb</code> MAY be migrated to <code>c64</code> / <code>Complex&lt;Float64&gt;</code>.</li>
  <li><code>csg</code> MAY be migrated to <code>c32</code> / <code>Complex&lt;Float32&gt;</code>.</li>
  <li>Legacy integer tokens already equal to canonical tokens require no migration.</li>
  <li>Legacy <code>ext</code> and <code>cxt</code> are ambiguous. A tool MUST request or derive an explicit supported type and MUST NOT guess silently.</li>
</ul>

<hr/>

<h2 id="validation">10. Validation Rules</h2>

<ul>
  <li>Unknown representation tokens MUST be diagnosed.</li>
  <li>A known representation unsupported by the selected execution target MUST be diagnosed; it MUST NOT be silently coerced.</li>
  <li>Representation aliases present together MUST agree.</li>
  <li>Parameterized representations MUST include complete, valid parameters.</li>
  <li>Array value shape and every materialized element MUST agree with the declared element representation.</li>
  <li>Visual header text, icon color, or palette tier MUST NOT override source type identity.</li>
</ul>
