<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG FIR Library Call Model</h1>

<p align="center">
  <strong>Public execution-facing representation model for intrinsic, standard-library, provider-backed, and external calls in FIR</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines how canonical Execution IR / FIR represents validated
calls without turning one compiler, provider, runtime, dynamic library, native
symbol, or host loader into the identity of the FROG program.
</p>

<p>
It aligns the public IR layer with the standard-library and provider doctrine
used by FROG:
</p>

<pre><code>validated source meaning
   -&gt;
explicit FIR call object
   -&gt;
lowering / backend contract / manifest
   -&gt;
chosen implementation mechanism</code></pre>

<p>
The call object is public and portable. Concrete lowering, ABI selection,
native symbol names, provider package filenames, loader behavior, and runtime
integration are downstream implementation concerns.
</p>

<hr/>

<h2>Boundary Rule</h2>

<pre><code>FIR owns the public execution-facing call identity.
Lowering owns target-oriented callable form.
Backend contracts own consumer-facing artifact requirements.
Implementations own concrete providers and runtime integration.</code></pre>

<p>
FIR must preserve enough information for a later backend to know what public
operation is required. It must not encode a private implementation recipe as
language truth.
</p>

<hr/>

<h2>Call Object Posture</h2>

<p>
In base v0.1, intrinsic operators and library functions may both appear as
FIR <code>primitive</code> objects. The distinction between a core intrinsic,
a standard-library function, a provider-backed call, a host capability, or an
external provider call is carried by:
</p>

<ul>
  <li>the namespace-qualified <code>kind</code>,</li>
  <li>the owning <code>library_id</code> or <code>profile_id</code>,</li>
  <li>explicit typed ports,</li>
  <li>effect metadata,</li>
  <li>status metadata,</li>
  <li>abstract provider or capability requirements where needed,</li>
  <li>source attribution and identity mapping.</li>
</ul>

<p>
The exact JSON shape may become first-class in a future schema revision. Until
then, bounded FIR examples may carry the call model in structured attributes
as long as the required facts remain explicit and machine-readable.
</p>

<hr/>

<h2>Minimal Public Shape</h2>

<p>
A FIR call object should make at least the following facts explicit:
</p>

<table>
  <thead>
    <tr>
      <th>Field family</th>
      <th>Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>family</code></td>
      <td>Execution-object family, commonly <code>primitive</code> for intrinsic and standard-library calls in base v0.1.</td>
    </tr>
    <tr>
      <td><code>kind</code></td>
      <td>Stable namespace-qualified public call identity, such as <code>frog.core.add</code> or <code>frog.image.decode_file_rgba8</code>.</td>
    </tr>
    <tr>
      <td><code>classification.library_id</code></td>
      <td>Owning public library family, such as <code>frog.core</code>, <code>frog.image</code>, or another published <code>frog.xxx</code> family.</td>
    </tr>
    <tr>
      <td><code>classification.profile_id</code></td>
      <td>Owning optional profile when a call belongs to a profile rather than the lightweight standard surface.</td>
    </tr>
    <tr>
      <td><code>ports</code></td>
      <td>Explicit typed input, output, state, status, or capability ports that participate in execution.</td>
    </tr>
    <tr>
      <td><code>call_model.call_class</code></td>
      <td>Public category describing the call's execution posture.</td>
    </tr>
    <tr>
      <td><code>call_model.effect</code></td>
      <td>Effect classification used by validation, lowering, packaging, and diagnostics.</td>
    </tr>
    <tr>
      <td><code>call_model.status_model</code></td>
      <td>Failure/status posture for the call.</td>
    </tr>
    <tr>
      <td><code>call_model.provider_requirement</code></td>
      <td>Abstract public capability requirement, when the call requires a provider, host service, or external implementation.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Call Classes</h2>

<table>
  <thead>
    <tr>
      <th>Class</th>
      <th>Meaning</th>
      <th>Provider posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>intrinsic_core_operator</code></td>
      <td>Core language operation with a published primitive contract.</td>
      <td>No provider requirement. Every conforming core implementation must understand the public primitive contract.</td>
    </tr>
    <tr>
      <td><code>intrinsic_core_state</code></td>
      <td>Core state or feedback operation with explicit state participation.</td>
      <td>No external provider requirement, but the operation is not pure.</td>
    </tr>
    <tr>
      <td><code>standard_library_value</code></td>
      <td>Value-only lightweight standard-library function.</td>
      <td>No mandatory native provider when the operation is purely value-level.</td>
    </tr>
    <tr>
      <td><code>standard_library_provider_backed</code></td>
      <td>Published <code>frog.xxx</code> standard-library function whose implementation may require a provider capability.</td>
      <td>Requires an abstract provider/capability requirement, not a fixed binary.</td>
    </tr>
    <tr>
      <td><code>host_capability</code></td>
      <td>Call whose meaning depends on a declared host capability such as file access, clock access, process access, or UI host access.</td>
      <td>Requires an explicit host capability requirement.</td>
    </tr>
    <tr>
      <td><code>external_provider</code></td>
      <td>Profile-owned, package-owned, or externally provided callable surface.</td>
      <td>Requires a declared provider, policy, and dependency contract.</td>
    </tr>
    <tr>
      <td><code>runtime_service</code></td>
      <td>Execution support supplied by a host, runtime, debugger, launcher, or IDE.</td>
      <td>Not an ordinary FIR program call unless exposed by a public source primitive or profile.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Effects</h2>

<p>
Each FIR call should classify its effect so later stages do not infer behavior
from private runtime habits.
</p>

<table>
  <thead>
    <tr>
      <th>Effect</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>pure</code></td>
      <td>Deterministic value computation with no host or resource effect after validation.</td>
    </tr>
    <tr>
      <td><code>stateful</code></td>
      <td>Uses explicit FIR state participation, such as delay or local memory.</td>
    </tr>
    <tr>
      <td><code>host_capability_read</code></td>
      <td>Reads through a declared host or provider capability.</td>
    </tr>
    <tr>
      <td><code>host_capability_write</code></td>
      <td>Writes through a declared host or provider capability.</td>
    </tr>
    <tr>
      <td><code>provider_native</code></td>
      <td>May be realized by a provider or native dependency while preserving the public primitive contract.</td>
    </tr>
    <tr>
      <td><code>runtime_orchestration</code></td>
      <td>Downstream host/runtime support, not source-level library semantics unless exposed by a public primitive or profile.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Status Models</h2>

<p>
FIR must not infer failure behavior from runtime folklore. Each call declares
its status posture.
</p>

<table>
  <thead>
    <tr>
      <th>Status model</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>none</code></td>
      <td>No value-level status channel. Suitable for total pure operators after type validation.</td>
    </tr>
    <tr>
      <td><code>local_result_tuple</code></td>
      <td>The primitive publishes ordinary data outputs such as <code>success</code> and <code>error_code</code>.</td>
    </tr>
    <tr>
      <td><code>status_value</code></td>
      <td>Reserved for a future uniform status-value or error-cluster-like corridor.</td>
    </tr>
  </tbody>
</table>

<p>
Provider or host-capability unavailability is a dependency or capability
resolution failure unless the published library contract defines it as a
normal output value.
</p>

<hr/>

<h2>Provider Requirements</h2>

<p>
Provider requirements answer this public question:
</p>

<pre><code>What public capability must be available for this validated call?</code></pre>

<p>
They do not answer:
</p>

<pre><code>Which private binary, loader, ABI, symbol, codec stack, or runtime service
must this implementation use?</code></pre>

<p>
A provider requirement may name:
</p>

<ul>
  <li>a public requirement id,</li>
  <li>the owning library family or profile,</li>
  <li>capability tokens,</li>
  <li>a semantic version posture,</li>
  <li>portability assumptions,</li>
  <li>whether the requirement is mandatory for lowering or optional for a host execution mode.</li>
</ul>

<p>
Concrete artifacts, callable symbols, shared-library names, static-link
strategy, generated wrappers, and packaged dependency closure belong to
lowering and backend contracts.
</p>

<hr/>

<h2>Example: Image Decode</h2>

<p>
A validated path-to-picture program using the standard image library can be
represented as:
</p>

<pre><code>widget_value(image_path)
   -&gt;
frog.image.decode_file_rgba8
   -&gt;
widget_value(preview_picture)</code></pre>

<p>
The path widget supplies a path value. The Picture widget displays an image
buffer value. The decode operation is the standard-library call between them.
</p>

<pre><code>{
  "id": "decode_image",
  "family": "primitive",
  "kind": "frog.image.decode_file_rgba8",
  "classification": {
    "primary_or_support": "primary",
    "library_id": "frog.image"
  },
  "ports": [
    { "id": "path", "direction": "in", "role": "data", "type_ref": "path" },
    { "id": "image", "direction": "out", "role": "data", "type_ref": "frog.image.buffer_rgba8" },
    { "id": "success", "direction": "out", "role": "data", "type_ref": "bool" },
    { "id": "error_code", "direction": "out", "role": "data", "type_ref": "string" }
  ],
  "attributes": {
    "call_model": {
      "call_class": "standard_library_provider_backed",
      "effect": "host_capability_read",
      "status_model": "local_result_tuple",
      "status_outputs": ["success", "error_code"],
      "provider_requirement": {
        "requirement_id": "frog.image.decode_file_rgba8",
        "capabilities": ["image.decode.rgba8"],
        "binding": "abstract_provider",
        "version": "v0.1-compatible"
      }
    }
  }
}</code></pre>

<p>
This FIR states the public requirement. It does not state which concrete image
decoder, shared library, static library, generated artifact, host service, or
runtime mechanism must satisfy that requirement.
</p>

<hr/>

<h2>FIR Versus Lowering</h2>

<table>
  <thead>
    <tr>
      <th>FIR owns</th>
      <th>Lowering/backend owns</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Namespaced call identity.</td>
      <td>Backend-specific callable form.</td>
    </tr>
    <tr>
      <td>Library or profile ownership.</td>
      <td>Native artifact kind, object layout, and ABI.</td>
    </tr>
    <tr>
      <td>Typed ports and connections.</td>
      <td>Storage layout, pointer shape, calling convention, and symbol mapping.</td>
    </tr>
    <tr>
      <td>Effect and status model.</td>
      <td>Diagnostic mapping for provider resolution and backend failure.</td>
    </tr>
    <tr>
      <td>Abstract provider or capability requirement.</td>
      <td>Concrete provider binding, linking, packaging, or host resolution.</td>
    </tr>
    <tr>
      <td>Source attribution and correspondence.</td>
      <td>Optimization, generated launcher details, manifest details, and deployment closure.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Validation Obligations</h2>

<p>
Before a call can be derived to FIR, semantic validation must establish:
</p>

<ul>
  <li>the call identity is known in the relevant library or profile surface,</li>
  <li>all required ports are present and directionally legal,</li>
  <li>input and output types are compatible with the published signature,</li>
  <li>status outputs are represented according to the published status model,</li>
  <li>effect classification is compatible with surrounding structure and state rules,</li>
  <li>provider or capability requirements are explicit when the call needs them,</li>
  <li>unsupported-but-valid call families are reported as unsupported by the implementation subset rather than guessed into private FIR.</li>
</ul>

<hr/>

<h2>Open Items</h2>

<p>
This model closes the public FIR-level call boundary. It does not close every
downstream or cross-cutting issue. Remaining work includes:
</p>

<ul>
  <li>first-class schema support for call metadata and document-level requirements,</li>
  <li>uniform error/status corridor beyond local result tuples,</li>
  <li>lowering and backend manifest rules for provider-backed calls,</li>
  <li>conformance cases for valid calls, invalid calls, unsupported calls, and missing-provider diagnostics,</li>
  <li>public library-specific signatures as additional <code>frog.xxx</code> families are promoted.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
FIR library calls are explicit execution-facing objects. They preserve the
public call identity, typed ports, effects, status posture, abstract provider
requirements, and attribution needed for later lowering. They do not bind FROG
semantics to a private runtime, a specific compiler, a concrete DLL, a native
symbol, or one provider implementation.
</p>
