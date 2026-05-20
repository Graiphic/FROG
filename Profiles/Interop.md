<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Interop Profile</h1>

<p align="center">
  Optional standardized interoperability profile for <strong>FROG</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose of the Interop Profile</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#architectural-role">4. Architectural Role</a></li>
  <li><a href="#profile-identity-and-namespace">5. Profile Identity and Namespace</a></li>
  <li><a href="#scope-for-v01">6. Scope for v0.1</a></li>
  <li><a href="#interop-model">7. Interop Model</a></li>
  <li><a href="#payload-model">8. Payload Model</a></li>
  <li><a href="#python-primitives">9. Python Primitives</a></li>
  <li><a href="#native-primitives">10. Native and Shared Library Primitives</a></li>
  <li><a href="#dotnet-primitives">11. .NET Primitives</a></li>
  <li><a href="#sql-primitives">12. SQL Primitives</a></li>
  <li><a href="#diagram-representation">13. Diagram Representation</a></li>
  <li><a href="#validation-rules">14. Validation Rules</a></li>
  <li><a href="#support-and-claims">15. Support and Capability Claims</a></li>
  <li><a href="#out-of-scope">16. Out of Scope for v0.1</a></li>
  <li><a href="#summary">17. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the <strong>Interop profile</strong> for FROG v0.1.
</p>

<p>
The Interop profile standardizes an <strong>optional capability family</strong> for interoperability
with external execution environments and external data systems.
</p>

<p>
In v0.1, this profile standardizes a conservative first surface covering:
</p>

<ul>
  <li>Python request/response invocation,</li>
  <li>native or shared library request/response invocation,</li>
  <li>.NET request/response invocation,</li>
  <li>SQL query and statement execution.</li>
</ul>

<p>
The goal of this first version is to define a practical, bounded, and portable interop surface that
fits cleanly within the current FROG diagram and type model while remaining outside the minimal
intrinsic library core.
</p>

<p>
In v0.1, the standardized interop model is intentionally limited to synchronous request/response
primitives using explicit textual or byte payloads.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Interop Profile</h2>

<p>
The purpose of this profile is to define a standardized but optional interoperability surface for
implementations that choose to support interaction with foreign runtimes, native or managed
execution platforms, or SQL-capable data systems.
</p>

<p>
This profile exists to preserve the architectural distinction between:
</p>

<ul>
  <li>the minimal core language surface,</li>
  <li>portable intrinsic standardized libraries,</li>
  <li>optional standardized capability families,</li>
  <li>implementation-specific extensions.</li>
</ul>

<p>
Accordingly, the Interop profile is standardized but optional.
Support for this profile is <strong>not</strong> required for core FROG conformance.
</p>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><code>Profiles/Readme.md</code> — defines the architectural role of optional standardized capability families.</li>
  <li><code>Expression/Diagram.md</code> — defines how Interop profile primitives appear as executable diagram nodes.</li>
  <li><code>Expression/Type.md</code> — defines the ordinary scalar and array types used by these primitives.</li>
  <li><code>Language/</code> — defines the cross-cutting execution semantics that remain authoritative for validated programs.</li>
  <li><code>Libraries/Core.md</code> — defines the minimal always-available computational primitives, which remain distinct from interop capability.</li>
  <li><code>Libraries/IO.md</code> — defines file, path, byte, and related resource I/O primitives, which remain distinct from foreign-runtime and SQL interoperability.</li>
  <li><code>Libraries/UI.md</code> — defines widget interaction primitives, which remain distinct from interop primitives.</li>
  <li><code>Libraries/Connectivity.md</code> — retained only as a transition note and MUST NOT be treated as the primary normative source for this profile.</li>
  <li><code>GOVERNANCE.md</code> — defines governance-facing distinctions such as core support, profile support, conformance, certification, and branding-related claims.</li>
</ul>

<p>
This document defines the standardized primitive catalog for the Interop profile.
It does not redefine the graph model, the core type model, the general execution semantics,
or the widget interaction model.
</p>

<hr/>

<h2 id="architectural-role">4. Architectural Role</h2>

<p>
The Interop profile provides standardized primitives for invoking capabilities that are external to
the ordinary FROG function model and external to the intrinsic primitive core.
</p>

<p>
In serialized diagrams, calls to these primitives are represented as <code>primitive</code> nodes whose
<code>type</code> field uses the <code>frog.connectivity.*</code> namespace.
</p>

<p>
Examples:
</p>

<pre>
frog.connectivity.python.call_text
frog.connectivity.native.call_bytes
frog.connectivity.dotnet.call_text
frog.connectivity.sql.query_text
frog.connectivity.sql.execute
</pre>

<p>
The Interop profile is distinct from:
</p>

<ul>
  <li><code>frog.core</code>, which contains the minimal always-available computational primitives,</li>
  <li><code>frog.io</code>, which covers file, path, byte, and related resource I/O only,</li>
  <li><code>frog.ui</code>, which covers widget interaction only,</li>
  <li>future Interop subprofiles or separate optional profiles for process execution, networking, HTTP, host input devices, hardware access, runtime coordination, deployment, or richer external capability families.</li>
</ul>

<p>
This profile standardizes an optional capability family.
It does not require that every FROG implementation support foreign runtimes, native ABI invocation,
managed-platform execution, or SQL integration.
</p>

<hr/>

<h2 id="profile-identity-and-namespace">5. Profile Identity and Namespace</h2>

<p>
The stable identity of this profile is:
</p>

<pre>
Interop profile
</pre>

<p>
This profile owns the standardized optional primitive namespace family:
</p>

<pre>
frog.connectivity.*
</pre>

<p>
For this profile, the general naming pattern is:
</p>

<pre>
frog.connectivity.&lt;family&gt;.&lt;primitive&gt;
</pre>

<p>
Examples:
</p>

<ul>
  <li><code>frog.connectivity.python.call_text</code></li>
  <li><code>frog.connectivity.python.call_bytes</code></li>
  <li><code>frog.connectivity.native.call_bytes</code></li>
  <li><code>frog.connectivity.dotnet.call_text</code></li>
  <li><code>frog.connectivity.sql.query_text</code></li>
  <li><code>frog.connectivity.sql.execute</code></li>
</ul>

<p>
Primitive and family names in <code>frog.connectivity</code> SHOULD use lowercase snake_case where
multiple words are needed.
</p>

<p>
The namespace remains stable even though its architectural ownership belongs to the Interop profile
rather than to the intrinsic standard library core.
</p>

<hr/>

<h2 id="scope-for-v01">6. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following Interop profile primitive families:
</p>

<ul>
  <li>Python request/response invocation primitives,</li>
  <li>native or shared library request/response invocation primitives,</li>
  <li>.NET request/response invocation primitives,</li>
  <li>SQL query and statement execution primitives.</li>
</ul>

<p>
For v0.1, these families remain in a single Interop profile document. The
profile owns <code>frog.connectivity.*</code> as an optional capability
namespace, while each family remains claimable only to the extent explicitly
defined here or by a later profile revision.
</p>

<p>
The following table records the public routing posture for current and future
connectivity families:
</p>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Namespace posture</th>
      <th>v0.1 status</th>
      <th>Boundary</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Python request/response</td>
      <td><code>frog.connectivity.python.*</code></td>
      <td>Published in this profile.</td>
      <td>Source-visible session handles, interpreter lifecycle objects, and environment selection contracts are deferred.</td>
    </tr>
    <tr>
      <td>Native or shared-library request/response</td>
      <td><code>frog.connectivity.native.*</code></td>
      <td>Published in this profile as byte request/response.</td>
      <td>Typed FFI, calling-convention catalogs, structured ABI parameters, and memory ownership contracts are deferred.</td>
    </tr>
    <tr>
      <td>.NET request/response</td>
      <td><code>frog.connectivity.dotnet.*</code></td>
      <td>Published in this profile as textual request/response.</td>
      <td>Managed object handles, reflection surfaces, COM, ActiveX, and arbitrary object-graph interop are deferred.</td>
    </tr>
    <tr>
      <td>SQL query and execute</td>
      <td><code>frog.connectivity.sql.*</code></td>
      <td>Published in this profile as minimal query and execute primitives.</td>
      <td>Connection handles, prepared statements, transactions, cursors, pooling, and row streaming are deferred.</td>
    </tr>
    <tr>
      <td>Process and command execution</td>
      <td><code>frog.connectivity.process.*</code> or a later process subprofile</td>
      <td>Not published in v0.1.</td>
      <td>Requires explicit argv versus shell execution, stdin/stdout/stderr, working directory, environment, timeout, exit status, security policy, and async-spawn boundaries.</td>
    </tr>
    <tr>
      <td>Network and HTTP</td>
      <td><code>frog.connectivity.net.*</code> and <code>frog.connectivity.http.*</code> or later subprofiles</td>
      <td>Not published in v0.1.</td>
      <td>Requires explicit socket/session lifetimes, DNS, TLS, headers, redirects, authentication, secrets, timeouts, cancellation, streaming, and policy gates.</td>
    </tr>
    <tr>
      <td>Host input devices</td>
      <td><code>frog.connectivity.input_device.*</code> or a later host-input subprofile</td>
      <td>Not published in v0.1.</td>
      <td>Requires platform gates, device discovery, open/acquire/close lifecycle, polling semantics, status behavior, and distinction from FROG UI widget events.</td>
    </tr>
    <tr>
      <td>Windows-specific system interop</td>
      <td><code>frog.connectivity.windows.*</code> or narrower Windows subprofiles</td>
      <td>Not published in v0.1, except the existing bounded .NET request/response family.</td>
      <td>Registry, COM, ActiveX, and broader managed-runtime integration require explicit platform, trust, threading, object lifetime, and capability policy.</td>
    </tr>
    <tr>
      <td>Service hosting, callbacks, and live data</td>
      <td>Later profile or profile revision only</td>
      <td>Not published in v0.1.</td>
      <td>Requires lifecycle, routing, concurrency, subscription, buffering, reconnection, security, and async/session semantics.</td>
    </tr>
  </tbody>
</table>

<p>
These future families are reserved only as routing posture. Their names,
ports, capability tokens, and conformance claims are not standardized until a
future profile document or profile revision publishes them explicitly.
</p>

<p>
These primitives provide a first standardized interoperability layer for:
</p>

<ul>
  <li>Python modules and functions,</li>
  <li>native or shared library entry points, including C-compatible or C++-compatible host-call surfaces exposed by a supporting implementation,</li>
  <li>.NET assemblies, types, and methods,</li>
  <li>SQL-capable data sources addressed by a supporting implementation.</li>
</ul>

<p>
FROG v0.1 does <strong>not</strong> attempt to define:
</p>

<ul>
  <li>persistent source-level handles or sessions,</li>
  <li>process or command execution primitives,</li>
  <li>source-visible Python session lifecycle primitives,</li>
  <li>typed foreign-function or ABI call nodes beyond the byte request/response native primitive,</li>
  <li>callbacks, event subscriptions, or bidirectional host integration,</li>
  <li>async or streaming interop,</li>
  <li>keyboard, mouse, joystick, game controller, or other host input-device acquisition primitives,</li>
  <li>object-graph reflection as a standard source-level mechanism,</li>
  <li>prepared statements, transactions, cursors, or connection pooling as source-level standardized objects,</li>
  <li>automatic marshaling of arbitrary FROG values into foreign object systems,</li>
  <li>COM, ActiveX, Java, gRPC, REST, message-bus, or general network transport primitives,</li>
  <li>generic external-runtime or external-service bindings beyond the explicit primitive families standardized here.</li>
</ul>

<hr/>

<h2 id="interop-model">7. Interop Model</h2>

<p>
In v0.1, standardized interop primitives are synchronous request/response operations.
</p>

<p>
An interop primitive consumes explicit inputs, performs one external interaction, and then produces
ordinary FROG outputs.
</p>

<p>
The standardized source-level model does not expose persistent foreign references, runtime sessions,
connection handles, or foreign object identities as first-class FROG values.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>each standardized call in this document is self-contained at source level,</li>
  <li>implementations MAY internally reuse connections, runtimes, processes, drivers, or pools,</li>
  <li>such reuse remains implementation-defined unless another applicable specification standardizes it,</li>
  <li>such reuse is not part of the canonical source contract defined here.</li>
</ul>

<p>
Binding resolution, host process model, security policy, timeout policy, foreign runtime availability,
driver availability, platform availability, and concrete execution environment are implementation-defined
unless standardized elsewhere.
</p>

<p>
This document standardizes the source-visible primitive contract only.
Implementation-defined binding does not make an unsupported family valid: a
program that references a <code>frog.connectivity.*</code> primitive outside
the supported profile or subset must be rejected or reported through the
implementation's unsupported-profile or unsupported-subset diagnostic model.
</p>

<hr/>

<h2 id="payload-model">8. Payload Model</h2>

<p>
All Interop profile primitives are typed according to <code>Expression/Type.md</code>.
</p>

<p>
Because FROG v0.1 does not yet standardize records, structs, enums, class types, or variant types,
the minimal standardized interop payload model uses only built-in scalar and array types.
</p>

<p>
In v0.1, the following built-in types are especially relevant to this profile:
</p>

<ul>
  <li><code>string</code> — used for identifiers, textual requests, textual responses, SQL text, and serialized parameter documents,</li>
  <li><code>bool</code> — used for success signaling,</li>
  <li><code>i64</code> — used for affected-row counts,</li>
  <li><code>array&lt;u8&gt;</code> — used for raw binary requests and responses.</li>
</ul>

<p>
Unless stated otherwise:
</p>

<ul>
  <li>all required inputs MUST exist and be type-compatible,</li>
  <li>all outputs MUST always be present,</li>
  <li>primitives that can fail MUST expose a <code>success</code> output of type <code>bool</code>,</li>
  <li>when <code>success = false</code>, every data output MUST still be well-typed and MUST take its defined fallback value.</li>
</ul>

<p>
This profile does not standardize an error-object output, exception payload, or diagnostic record as part
of the source-visible primitive contract in v0.1.
Failure is exposed only through <code>success</code> together with the standardized fallback value of each data output.
</p>

<p>
Fallback values in this document are standardized as follows:
</p>

<ul>
  <li>empty string: <code>""</code>,</li>
  <li>zero affected rows: <code>0</code>,</li>
  <li>empty dynamic byte array: <code>array&lt;u8&gt;</code> with zero elements.</li>
</ul>

<p>
When a primitive uses a textual request or textual response payload, the semantic meaning of that payload
is defined by the supporting implementation or by another applicable specification.
For example, an implementation MAY use JSON text, SQL parameter text, or another stable textual interchange form.
</p>

<p>
When a primitive uses a byte payload, the semantic meaning of that payload is likewise defined by the
supporting implementation or by another applicable specification.
</p>

<hr/>

<h2 id="python-primitives">9. Python Primitives</h2>

<h3>9.1 <code>frog.connectivity.python.call_text</code></h3>

<p>
Invokes a Python function using a textual request payload and returns a textual response payload.
</p>

<ul>
  <li>input ports: <code>module</code>, <code>function</code>, <code>request</code></li>
  <li>output ports: <code>response</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>module: string</code></li>
  <li><code>function: string</code></li>
  <li><code>request: string</code></li>
  <li><code>response: string</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>response</code> contains the textual result produced by the addressed Python function,</li>
  <li>if <code>success = false</code>, <code>response</code> MUST be the empty string,</li>
  <li>Python environment resolution, import rules, and function dispatch are implementation-defined unless standardized elsewhere.</li>
</ul>

<h3>9.2 <code>frog.connectivity.python.call_bytes</code></h3>

<p>
Invokes a Python function using a byte request payload and returns a byte response payload.
</p>

<ul>
  <li>input ports: <code>module</code>, <code>function</code>, <code>request</code></li>
  <li>output ports: <code>response</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>module: string</code></li>
  <li><code>function: string</code></li>
  <li><code>request: array&lt;u8&gt;</code></li>
  <li><code>response: array&lt;u8&gt;</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>response</code> contains the byte result produced by the addressed Python function,</li>
  <li>if <code>success = false</code>, <code>response</code> MUST be the empty byte array,</li>
  <li>Python environment resolution, import rules, and function dispatch are implementation-defined unless standardized elsewhere.</li>
</ul>

<hr/>

<h2 id="native-primitives">10. Native and Shared Library Primitives</h2>

<h3>10.1 <code>frog.connectivity.native.call_bytes</code></h3>

<p>
Invokes an entry point from a native or shared library using a byte request payload and returns a
byte response payload.
</p>

<ul>
  <li>input ports: <code>library</code>, <code>symbol</code>, <code>request</code></li>
  <li>output ports: <code>response</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>library: string</code></li>
  <li><code>symbol: string</code></li>
  <li><code>request: array&lt;u8&gt;</code></li>
  <li><code>response: array&lt;u8&gt;</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>response</code> contains the byte result produced by the addressed native or shared-library entry point,</li>
  <li>if <code>success = false</code>, <code>response</code> MUST be the empty byte array,</li>
  <li>library resolution, calling convention, symbol visibility, ABI compatibility, and memory ownership rules are implementation-defined unless standardized elsewhere.</li>
</ul>

<hr/>

<h2 id="dotnet-primitives">11. .NET Primitives</h2>

<h3>11.1 <code>frog.connectivity.dotnet.call_text</code></h3>

<p>
Invokes a .NET method using a textual request payload and returns a textual response payload.
</p>

<ul>
  <li>input ports: <code>assembly</code>, <code>type_name</code>, <code>method</code>, <code>request</code></li>
  <li>output ports: <code>response</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>assembly: string</code></li>
  <li><code>type_name: string</code></li>
  <li><code>method: string</code></li>
  <li><code>request: string</code></li>
  <li><code>response: string</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>response</code> contains the textual result produced by the addressed .NET method,</li>
  <li>if <code>success = false</code>, <code>response</code> MUST be the empty string,</li>
  <li>assembly resolution, type lookup, method binding, runtime loading, and instance-versus-static dispatch policy are implementation-defined unless standardized elsewhere.</li>
</ul>

<hr/>

<h2 id="sql-primitives">12. SQL Primitives</h2>

<h3>12.1 <code>frog.connectivity.sql.query_text</code></h3>

<p>
Executes a SQL query and returns a textual result document.
</p>

<ul>
  <li>input ports: <code>connection</code>, <code>query</code>, <code>parameters</code></li>
  <li>output ports: <code>rows</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>connection: string</code></li>
  <li><code>query: string</code></li>
  <li><code>parameters: string</code></li>
  <li><code>rows: string</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>rows</code> contains the query result encoded as a textual result document,</li>
  <li>if <code>success = false</code>, <code>rows</code> MUST be the empty string,</li>
  <li>the textual form of <code>parameters</code> is implementation-defined unless standardized elsewhere,</li>
  <li>connection-string interpretation, driver selection, dialect behavior, and result encoding are implementation-defined unless standardized elsewhere.</li>
</ul>

<h3>12.2 <code>frog.connectivity.sql.execute</code></h3>

<p>
Executes a non-query SQL statement and returns an affected-row count.
</p>

<ul>
  <li>input ports: <code>connection</code>, <code>query</code>, <code>parameters</code></li>
  <li>output ports: <code>affected_rows</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>connection: string</code></li>
  <li><code>query: string</code></li>
  <li><code>parameters: string</code></li>
  <li><code>affected_rows: i64</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>affected_rows</code> contains the affected-row count produced by the supporting implementation,</li>
  <li>if <code>success = false</code>, <code>affected_rows</code> MUST be <code>0</code>,</li>
  <li>the textual form of <code>parameters</code> is implementation-defined unless standardized elsewhere,</li>
  <li>connection-string interpretation, driver selection, dialect behavior, and affected-row semantics are implementation-defined unless standardized elsewhere.</li>
</ul>

<p>
In v0.1, SQL primitives do not standardize source-level transactions, prepared-statement handles,
cursor iteration, or connection lifetime control.
</p>

<hr/>

<h2 id="diagram-representation">13. Diagram Representation</h2>

<p>
Calls to Interop profile primitives are serialized as <code>primitive</code> nodes in the diagram.
</p>

<p>
Examples:
</p>

<pre>
{
  "id": "py_eval_1",
  "kind": "primitive",
  "type": "frog.connectivity.python.call_text"
}

{
  "id": "native_call_1",
  "kind": "primitive",
  "type": "frog.connectivity.native.call_bytes"
}

{
  "id": "sql_query_1",
  "kind": "primitive",
  "type": "frog.connectivity.sql.query_text"
}
</pre>

<p>
The exact port existence, direction, and typing of these nodes are resolved from this specification
together with the type model and the graph rules.
</p>

<hr/>

<h2 id="validation-rules">14. Validation Rules</h2>

<p>
Implementations that claim support for the Interop profile MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.connectivity</code> primitive reference MUST identify a valid standardized Interop profile primitive name,</li>
  <li>all required input ports MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>all identifier-like selector ports in this document MUST use type <code>string</code> in v0.1,</li>
  <li>all byte payload ports in this document MUST use type <code>array&lt;u8&gt;</code>,</li>
  <li>all <code>success</code> outputs MUST use type <code>bool</code>.</li>
</ul>

<p>
For result-bearing primitives:
</p>

<ul>
  <li>a <code>success</code> output MUST exist,</li>
  <li>fallback values for unsuccessful results MUST match the definitions in this specification.</li>
</ul>

<p>
For <code>frog.connectivity.sql.execute</code>:
</p>

<ul>
  <li><code>affected_rows</code> MUST use type <code>i64</code>.</li>
</ul>

<p>
An implementation that does not support this profile MUST NOT silently reinterpret
<code>frog.connectivity.*</code> primitives as intrinsic-library primitives.
Unsupported profile usage MUST instead be rejected or reported as unsupported according to the
implementation's validation or capability-reporting model.
</p>

<p>
An implementation that supports only a subset of this profile MUST likewise
reject or report primitives outside its claimed subset. For example, support
for <code>frog.connectivity.python.*</code> does not imply support for
<code>frog.connectivity.sql.*</code>, future process execution primitives,
future HTTP primitives, or future host input-device primitives.
</p>

<hr/>

<h2 id="support-and-claims">15. Support and Capability Claims</h2>

<p>
Support for the Interop profile is optional.
</p>

<p>
A core-conforming FROG implementation MAY support none of the primitives defined here.
</p>

<p>
An implementation claiming support for the <strong>Interop profile</strong> SHOULD support the full set
of primitives defined by this document unless a future narrower subprofile, profile revision, or
explicit subset-claim model is standardized.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><code>core FROG support only</code> is a valid claim,</li>
  <li><code>core FROG + Interop profile</code> is a stronger claim,</li>
  <li>a partial implementation MUST NOT present itself as full Interop profile support unless the supported subset is made explicit.</li>
</ul>

<p>
Examples of explicit non-full claims may include implementation wording such as:
</p>

<ul>
  <li><code>core FROG + Python interop subset</code></li>
  <li><code>core FROG + SQL interop subset</code></li>
  <li><code>core FROG + partial frog.connectivity support</code></li>
</ul>

<p>
Future subprofiles may make narrower claims first-class, such as a process
execution subset, an HTTP client subset, a typed native FFI subset, an
input-device subset, or a Windows-specific subset. Until such documents are
published, those claims are descriptive implementation claims rather than
full standardized Interop profile claims.
</p>

<p>
Capability claims under this document do not automatically imply certification, trademark permission,
official endorsement, or branding authorization.
</p>

<hr/>

<h2 id="out-of-scope">16. Out of Scope for v0.1</h2>

<ul>
  <li>persistent foreign handles or object references as source-level standardized values,</li>
  <li>process launch, shell execution, background process management, stdout/stderr streaming, or OS command nodes,</li>
  <li>standardized reflection over Python objects, .NET objects, or native ABI structures,</li>
  <li>standardized Python interpreter sessions, virtual-environment selection, or reusable scripting-session objects,</li>
  <li>automatic bidirectional mapping between arbitrary FROG values and foreign structured types,</li>
  <li>typed ABI configuration for native/shared-library calls beyond the byte request/response primitive,</li>
  <li>callbacks and re-entrant invocation into FROG graphs,</li>
  <li>transaction control primitives such as begin, commit, and rollback,</li>
  <li>prepared statements, cursors, and row-by-row iteration,</li>
  <li>network protocols such as HTTP, TCP, UDP, WebSocket, MQTT, or gRPC,</li>
  <li>host input-device discovery or acquisition for keyboards, mice, joysticks, game controllers, or comparable devices,</li>
  <li>COM, ActiveX, Java, or other non-listed foreign platforms,</li>
  <li>generic external-runtime or external-service primitives beyond the explicit families standardized here,</li>
  <li>async execution, futures, promises, channels, or runtime scheduling primitives,</li>
  <li>source-level error objects, exception payload channels, or standardized diagnostic record transport.</li>
</ul>

<hr/>

<h2 id="summary">17. Summary</h2>

<p>
The Interop profile defines a minimal optional standardized interoperability surface for FROG v0.1.
</p>

<p>
It standardizes conservative request/response primitives for Python, native or shared libraries,
.NET, and SQL while remaining compatible with the current FROG source and type model.
</p>

<p>
In short:
</p>

<ul>
  <li><code>frog.io</code> covers file, path, byte, and related resource I/O,</li>
  <li><code>frog.ui</code> covers widget interaction,</li>
  <li><code>frog.connectivity</code> covers optional standardized interoperability with foreign runtimes, native or shared libraries, managed platforms, and SQL-capable data sources,</li>
  <li>richer sessions, handles, callbacks, broader service bindings, and async coordination remain for later revisions.</li>
</ul>
