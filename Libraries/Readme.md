<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">🐸 FROG Libraries</h1>

<p align="center">
  Intrinsic standardized primitive library specifications for <strong>FROG</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-layer-exists">2. Why this Layer Exists</a></li>
  <li><a href="#scope-of-this-directory">3. Scope of this Directory</a></li>
  <li><a href="#architectural-position">4. Architectural Position</a></li>
  <li><a href="#how-a-primitive-gets-its-meaning">5. How a Primitive Gets its Meaning</a></li>
  <li><a href="#current-documents">6. Current Documents</a></li>
  <li><a href="#intrinsic-library-taxonomy">7. Intrinsic Library Taxonomy</a></li>
  <li><a href="#libraries-vs-profiles">8. Libraries vs Profiles</a></li>
  <li><a href="#naming-and-namespaces">9. Naming and Namespaces</a></li>
  <li><a href="#library-boundaries">10. Library Boundaries</a></li>
  <li><a href="#relation-with-other-specification-layers">11. Relation with Other Specification Layers</a></li>
  <li><a href="#relation-with-ir-lowering-and-backend-contract">12. Relation with IR, Lowering, and Backend Contract</a></li>
  <li><a href="#library-evolution">13. Library Evolution</a></li>
  <li><a href="#status">14. Status</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory contains the <strong>intrinsic standardized primitive library specifications</strong> used by FROG programs.
</p>

<p>
In FROG, executable diagrams may contain nodes of kind <code>primitive</code>.
The meaning of such nodes is not fully determined by their serialized presence alone.
Primitive identity and primitive-local contract must come from a stable normative source.
</p>

<p>
This directory is that normative home for the primitive vocabularies that belong to the
<strong>intrinsic language surface</strong>.
</p>

<p>
In the current repository architecture:
</p>

<ul>
  <li><code>Expression/</code> defines how primitive nodes appear in canonical source,</li>
  <li><code>Language/</code> defines cross-cutting execution semantics for validated programs,</li>
  <li><code>Libraries/</code> defines intrinsic standardized primitive vocabularies and primitive-local contracts,</li>
  <li><code>Profiles/</code> defines optional standardized capability families beyond the intrinsic core,</li>
  <li><code>IR/</code> defines the canonical execution-facing representation derived from validated meaning together with lowering and backend-facing boundaries,</li>
  <li><code>IDE/</code> may expose primitives in palettes and authoring flows without redefining them.</li>
</ul>

<p>
The purpose of <code>Libraries/</code> is therefore intentionally narrow:
it standardizes primitive families that are fundamental, portable, and intrinsic to the FROG language surface itself.
</p>

<p>
FROG distinguishes the minimal core primitive set, lightweight standard
library families, and optional external capability families. Lightweight
standard libraries are part of the base FROG language distribution when
published here, but they remain explicit diagram primitives rather than
hidden runtime features.
</p>

<pre><code>frog.core.*       -&gt; foundational operators and execution building blocks
frog.image.*      -&gt; lightweight standard library primitives
Profiles/ or
external packages -&gt; optional or environment-specific capability families
</code></pre>

<p>
For the widget corridor, this layer also becomes the normative home of the intrinsic executable interaction vocabulary, while widget instances, widget class law, widget behavior, widget realization, and widget-oriented packaging remain owned elsewhere.
</p>

<hr/>

<h2 id="why-this-layer-exists">2. Why this Layer Exists</h2>

<p>
A diagram can declare that a node is a primitive, but the diagram alone does not fully define what that primitive means.
That meaning must come from a stable normative source.
</p>

<p>
For example:
</p>

<pre><code>{
  "id": "add_1",
  "kind": "primitive",
  "type": "frog.core.add"
}</code></pre>

<p>
The diagram identifies the primitive node.
It does not by itself define the full primitive contract.
That contract comes from the relevant library specification together with the broader execution semantics defined elsewhere.
</p>

<p>
This layer exists so that intrinsic primitive identity remains:
</p>

<ul>
  <li>explicit,</li>
  <li>portable,</li>
  <li>tool-independent,</li>
  <li>stable across IDEs, validators, runtimes, compilers, and other implementations.</li>
</ul>

<p>
Without a dedicated library layer, primitive meaning would risk being scattered across:
</p>

<ul>
  <li>diagram examples,</li>
  <li>tooling behavior,</li>
  <li>runtime conventions,</li>
  <li>vendor-specific interpretations.</li>
</ul>

<p>
This directory prevents that drift by giving intrinsic primitive vocabularies a clear normative home.
</p>

<p>
That same discipline is now especially important for the widget corridor:
the source-level widget interaction model cannot remain credible if the executable primitive surface is left to runtime-private convention.
</p>

<hr/>

<h2 id="scope-of-this-directory">3. Scope of this Directory</h2>

<p>
This directory specifies intrinsic library-level primitive catalogs.
It defines which intrinsic standardized primitives exist, how they are named, what ports they expose,
what primitive-local metadata they require, and what primitive-local behavior belongs to them.
</p>

<p>
This directory is the normative home for questions such as:
</p>

<ul>
  <li>Which intrinsic primitive identifiers exist?</li>
  <li>Which intrinsic namespace owns a primitive?</li>
  <li>What ports does a primitive expose?</li>
  <li>What primitive-local metadata is required?</li>
  <li>What primitive-local behavior belongs to that primitive family?</li>
</ul>

<p>
This directory does <strong>not</strong> define:
</p>

<ul>
  <li>the canonical <code>.frog</code> source structure,</li>
  <li>the general executable graph model,</li>
  <li>the general type-expression model,</li>
  <li>the widget object model,</li>
  <li>front-panel serialization,</li>
  <li>cross-cutting execution semantics for validated programs,</li>
  <li>the canonical Execution IR object model,</li>
  <li>lowering strategy,</li>
  <li>backend contract content,</li>
  <li>IDE palette organization or authoring workflows,</li>
  <li>optional profile-owned capability families,</li>
  <li>implementation-specific extensions.</li>
</ul>

<p>
Those concerns are normatively owned elsewhere in the repository.
</p>

<p>
This directory also does not own the standardized primitive widget catalog itself.
The primitive UI interaction surface belongs in <code>Libraries/</code>, while the standardized primitive widget families themselves belong in their own normative library-facing widget specifications rather than being smuggled into the executable primitive corridor.
</p>

<hr/>

<h2 id="architectural-position">4. Architectural Position</h2>

<p>
The position of <code>Libraries/</code> inside the repository architecture is intentionally narrow and explicit:
</p>

<pre><code>Repository architecture around Libraries/

Expression/   -&gt; canonical source form
Language/     -&gt; normative execution meaning
Libraries/    -&gt; intrinsic primitive vocabularies
Profiles/     -&gt; optional standardized capability families
IR/           -&gt; canonical execution-facing representation and downstream boundaries
IDE/          -&gt; authoring, palette, observability, debugging, inspection

Libraries/ own intrinsic primitive definitions referenced by executable diagrams.
Libraries/ do not own source structure, cross-cutting execution semantics,
optional capability profiles, IR derivation, lowering, backend contracts,
or IDE behavior.
</code></pre>

<p>
This separation matters because the same primitive identity may be:
</p>

<ul>
  <li>serialized in source by <code>Expression/</code>,</li>
  <li>interpreted locally through <code>Libraries/</code>,</li>
  <li>executed under broader constraints described by <code>Language/</code>,</li>
  <li>represented later in execution-facing form by <code>IR/</code>,</li>
  <li>lowered or packaged as part of an explicit standard-library dependency closure,</li>
  <li>surfaced to users by <code>IDE/Palette.md</code>,</li>
  <li>constrained by optional capability contracts defined in <code>Profiles/</code>.</li>
</ul>

<p>
This also explains why <code>Libraries/Connectivity.md</code> is retained only as a transition note:
the authoritative normative home for <code>frog.connectivity.*</code> is now the Interop profile, not the intrinsic library core.
</p>

<p>
For the widget corridor, the same rule applies:
</p>

<pre><code>Expression/
    owns widget interaction source representation

Libraries/
    own frog.ui.* primitive-local contract

Language/
    owns validated semantic meaning

IR/
    owns execution-facing derivation and downstream transformation
</code></pre>

<hr/>

<h2 id="how-a-primitive-gets-its-meaning">5. How a Primitive Gets its Meaning</h2>

<p>
The following diagram summarizes how an intrinsic primitive node is interpreted:
</p>

<pre><code>Canonical source (.frog)
        |
        v
Expression/Diagram.md
declares a primitive node
        |
        v
"type": "frog.core.add"
        |
        v
Libraries/Core.md
defines the primitive-local contract
        |
        v
Language/
applies cross-cutting execution semantics
        |
        v
validated executable meaning
        |
        v
IR/
may derive canonical execution-facing representation
</code></pre>

<p>
Primitive meaning is therefore <strong>composed</strong>, not collapsed into one document:
</p>

<ul>
  <li><code>Expression/</code> owns representation,</li>
  <li><code>Libraries/</code> own intrinsic primitive identity and primitive-local contract,</li>
  <li><code>Language/</code> owns cross-cutting execution meaning,</li>
  <li><code>IR/</code> owns derived execution-facing representation after validated meaning already exists.</li>
</ul>

<p>
For <code>frog.ui.*</code>, this composition also depends on the widget-side source model:
</p>

<pre><code>Expression/Widget.md
         +
Expression/Widget interaction.md
         +
Expression/Widget class contract.md
         +
Libraries/UI.md
         +
Language/
         =
validated executable meaning of UI interaction
</code></pre>

<p>
This is exactly why the widget corridor must remain layered:
no one document and no one runtime should be allowed to absorb the full meaning alone.
</p>

<hr/>

<h2 id="current-documents">6. Current Documents</h2>

<p>
This directory currently contains the following documents:
</p>

<ul>
  <li><code>Readme.md</code> — architectural entry point for intrinsic standardized primitive libraries.</li>
  <li><code>Core.md</code> — foundational <code>frog.core</code> primitives.</li>
  <li><code>Math.md</code> — standard <code>frog.math</code> primitives.</li>
  <li><code>Collections.md</code> — standard <code>frog.collections</code> primitives.</li>
  <li><code>Text.md</code> — standard <code>frog.text</code> primitives.</li>
  <li><code>IO.md</code> — standard <code>frog.io</code> primitives.</li>
  <li><code>Image.md</code> — standard <code>frog.image</code> primitives, currently publishing <code>frog.image.decode_file_rgba8</code> and the portable <code>frog.image.buffer_rgba8</code> contract.</li>
  <li><code>Signal.md</code> — standard <code>frog.signal</code> primitives.</li>
  <li><code>System.md</code> — standard <code>frog.system</code> introspection primitives for platform, CPU, memory, process, environment, and capability reads.</li>
  <li><code>UI.md</code> — standard <code>frog.ui</code> executable widget interaction primitives.</li>
  <li><code>Connectivity.md</code> — transition note indicating that <code>frog.connectivity.*</code> is no longer normatively owned by the intrinsic library layer and is now owned by the Interop profile.</li>
  <li><code>Time.md</code> - standard <code>frog.time</code> primitives for wall-clock time, monotonic time, durations, waits, formatting/parsing, and profiling marks.</li>
</ul>

<p>
The presence of <code>Connectivity.md</code> in this directory does <strong>not</strong> mean that
<code>frog.connectivity.*</code> remains part of the intrinsic standardized library core.
It is retained only for repository continuity, navigation stability, and explicit architectural redirection.
</p>

<p>
In practice:
</p>

<pre><code>Libraries/Connectivity.md   -&gt; transition note only
Profiles/Interop.md         -&gt; authoritative normative home
</code></pre>

<p>
As the widget corridor closes further, this directory is also expected to become the home of the standardized primitive widget baseline, but only in documents whose ownership clearly targets reusable intrinsic widget families rather than source serialization or runtime realization.
</p>

<hr/>

<h2 id="intrinsic-library-taxonomy">7. Intrinsic Library Taxonomy</h2>

<p>
At the current repository stage, the intrinsic standardized primitive taxonomy is organized as follows:
</p>

<ul>
  <li><strong><code>frog.core.*</code></strong> — foundational language primitives</li>
  <li><strong><code>frog.math.*</code></strong> — numeric scalar primitives</li>
  <li><strong><code>frog.collections.*</code></strong> — collection primitives</li>
  <li><strong><code>frog.text.*</code></strong> — text-processing primitives</li>
  <li><strong><code>frog.io.*</code></strong> — file, path, resource, and byte-oriented I/O primitives</li>
  <li><strong><code>frog.image.*</code></strong> — basic image-data decoding and portable image-buffer primitives; current published primitive: <code>frog.image.decode_file_rgba8</code></li>
  <li><strong><code>frog.signal.*</code></strong> — signal-processing primitives</li>
  <li><strong><code>frog.system.*</code></strong> — bounded system introspection primitives</li>
  <li><strong><code>frog.time.*</code></strong> - wall-clock, monotonic, duration, wait, formatting/parsing, and profiling primitives</li>
  <li><strong><code>frog.ui.*</code></strong> — executable widget interaction primitives</li>
</ul>

<p>
The taxonomy can also be read as a simple mental map:
</p>

<pre><code>frog.core.*         -&gt; foundational execution building blocks
frog.math.*         -&gt; scalar numeric operations
frog.collections.*  -&gt; collection manipulation
frog.text.*         -&gt; text processing
frog.io.*           -&gt; file/path/resource/byte I/O
frog.image.*        -&gt; image decoding and image-buffer values
frog.signal.*       -&gt; signal-oriented operations
frog.system.*       -&gt; bounded system introspection
frog.time.*         -&gt; time and timing operations
frog.ui.*           -&gt; object-style widget interaction in execution
</code></pre>

<p>
The first lightweight-standard taxonomy also classifies each family by its
current publication status and by the kind of implementation boundary it
implies:
</p>

<ul>
  <li><strong>value-only</strong>: ordinary deterministic value transforms with no host resource dependency,</li>
  <li><strong>provider-backed</strong>: explicit primitives whose implementation may come from a standard-library provider or packaged dependency,</li>
  <li><strong>runtime-hosted</strong>: explicit primitives that need host/runtime services such as UI objects, scheduling, time sources, or managed resources,</li>
  <li><strong>OS-dependent</strong>: explicit primitives whose result depends on platform, filesystem, process, or environment capabilities,</li>
  <li><strong>profile-owned</strong>: standardized optional capability families that are not intrinsic lightweight libraries.</li>
</ul>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Status</th>
      <th>Purpose</th>
      <th>Initial scope</th>
      <th>Out of scope / boundary</th>
      <th>Implementation posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.core</code></td>
      <td>published core</td>
      <td>Minimal foundational operators and execution building blocks.</td>
      <td>Arithmetic, comparison, boolean logic, value selection, and explicit local memory through <code>frog.core.delay</code>.</td>
      <td>Broader math, arrays, text, I/O, UI interaction, profiles, and runtime services.</td>
      <td>Mostly value-only; <code>delay</code> is explicit local memory governed with the language execution model.</td>
    </tr>
    <tr>
      <td><code>frog.math</code></td>
      <td>published lightweight standard library</td>
      <td>Portable scalar mathematics beyond the minimal core.</td>
      <td>Utility, rounding, powers, roots, logarithms, trigonometric, and hyperbolic scalar functions.</td>
      <td>Linear algebra, statistics, tensors, symbolic math, optimization, and signal processing.</td>
      <td>Value-only, with numeric edge behavior constrained by the active execution profile.</td>
    </tr>
    <tr>
      <td><code>frog.collections</code></td>
      <td>published lightweight standard library</td>
      <td>Portable collection manipulation.</td>
      <td>Array length, empty check, element get/set, append, concat, and slice.</td>
      <td>Maps, sets, dictionaries, higher-order collection functions, advanced reshaping, and streaming collections.</td>
      <td>Value-only. The earlier <code>frog.array</code> candidate is represented by this broader namespace in the current public surface.</td>
    </tr>
    <tr>
      <td><code>frog.text</code></td>
      <td>published lightweight standard library</td>
      <td>Portable text processing over ordinary string values.</td>
      <td>Concat, length, substring, contains, prefix/suffix predicates, first find, split, and join.</td>
      <td>Locale collation, regular expressions, rich formatting, parsing, encodings, JSON/XML/HTML, and file/network text I/O.</td>
      <td>Value-only. The earlier <code>frog.string</code> candidate is represented by <code>frog.text</code> in the current public surface.</td>
    </tr>
    <tr>
      <td><code>frog.io</code></td>
      <td>published lightweight standard library</td>
      <td>Foundational file, path, resource, and byte-oriented access.</td>
      <td>Path joining/normalization/parent/name, filesystem existence and kind queries, directory listing, whole text reads, and whole byte reads.</td>
      <td>File mutation, streams, partial reads, networking, hardware I/O, foreign runtime calls, database access, and structured error objects.</td>
      <td>OS-dependent and runtime-hosted. The earlier <code>frog.path</code> and <code>frog.file</code> candidates are represented by this namespace for v0.1.</td>
    </tr>
    <tr>
      <td><code>frog.image</code></td>
      <td>published lightweight standard library</td>
      <td>Portable image-data values and baseline graphics file decoding.</td>
      <td><code>decode_file_rgba8</code>, PNG/JPEG/BMP reads, and the <code>frog.image.buffer_rgba8</code> value contract.</td>
      <td>Picture widget realization, camera acquisition, GPU surfaces, advanced computer vision, and non-promoted write/drawing functions.</td>
      <td>Provider-backed. Codec choice and packaging are implementation details behind the public primitive contract.</td>
    </tr>
    <tr>
      <td><code>frog.signal</code></td>
      <td>published lightweight standard library</td>
      <td>Portable one-dimensional sampled-signal transforms.</td>
      <td>Basic analysis, deterministic filtering, decimation, and simple resampling over numeric sample arrays.</td>
      <td>Waveform records, hardware acquisition, streaming, FFT/spectral analysis, adaptive filters, and multi-channel containers.</td>
      <td>Value-only for the published v0.1 surface.</td>
    </tr>
    <tr>
      <td><code>frog.ui</code></td>
      <td>published intrinsic interaction library</td>
      <td>Executable widget interaction through object-style and event-oriented primitives.</td>
      <td>Property read/write, method invoke, event observe, widget-reference consumption, and optional UI sequencing ports.</td>
      <td>Widget class catalog, front-panel serialization, <code>.wfrog</code> package content, rendering, host realization, and a general async event framework.</td>
      <td>Runtime-hosted and UI-host mediated, while primitive identity remains public and portable.</td>
    </tr>
    <tr>
      <td><code>frog.time</code></td>
      <td>published lightweight standard library</td>
      <td>Explicit wall-clock, monotonic, duration, wait, formatting/parsing, and diagnostic timing primitives.</td>
      <td><code>now</code>, <code>monotonic_now</code>, elapsed helpers, duration conversion/arithmetic, ISO 8601 UTC format/parse, scheduler wait boundaries, periodic target helper, and profiling marks.</td>
      <td>Hard real-time guarantees, scheduler internals, timer handles, timed loops, wall-clock alarms, locale-heavy formatting, private profiling transports, and OS-specific clock APIs as public law.</td>
      <td>Mixed value-only and runtime-hosted. Clock reads, waits, and profiling hooks require explicit host capability metadata and local <code>success</code>/<code>error_code</code> status outputs where failure is possible.</td>
    </tr>    <tr>
      <td><code>frog.waveform</code></td>
      <td>candidate lightweight standard library</td>
      <td>Portable waveform value helpers around sampled data with timing metadata.</td>
      <td>Build/unbundle waveform values, access samples, access or update <code>t0</code> and <code>dt</code>, and append compatible waveforms.</td>
      <td>Hardware acquisition, streaming buffers, chart history widgets, and device timing.</td>
      <td>Value-only if limited to waveform records; provider/runtime dependencies start only when acquisition or streaming is introduced.</td>
    </tr>
    <tr>
      <td><code>frog.table</code></td>
      <td>candidate lightweight standard library</td>
      <td>Portable tabular value construction and access.</td>
      <td>Build table values, row/column counts, cell access, row/column extraction, and simple table formatting.</td>
      <td>Database connectivity, spreadsheet automation, table widgets, and external data-frame engines.</td>
      <td>Value-only for data-shape helpers; provider-backed only if later formatting/import/export contracts require it.</td>
    </tr>
    <tr>
      <td><code>frog.picture</code></td>
      <td>candidate lightweight standard library</td>
      <td>Picture-value construction and simple 2D drawing.</td>
      <td>Empty picture, points, lines, rectangles, ovals, arcs, text, pixmap composition, and rasterization candidates.</td>
      <td>Picture widget realization, advanced graphics engines, GPU drawing, font-engine mandates, and computer vision.</td>
      <td>Provider-backed where rasterization, text shaping, or image composition needs an implementation module.</td>
    </tr>
    <tr>
      <td><code>frog.plot</code></td>
      <td>candidate lightweight standard library</td>
      <td>Simple plot rendering helpers that produce picture-like values.</td>
      <td>Waveform, XY, multi-XY, and legend rendering candidates.</td>
      <td>Chart widgets, live chart history, acquisition, advanced signal analysis, and plotting backends as mandatory dependencies.</td>
      <td>Provider-backed or value-to-picture depending on the accepted picture model.</td>
    </tr>
    <tr>
      <td><code>frog.boolean</code></td>
      <td>deferred candidate</td>
      <td>Boolean helpers not already covered by <code>frog.core</code>.</td>
      <td>Possible edge detection, selection helpers, and latch candidates.</td>
      <td>Core boolean logic already belongs to <code>frog.core</code>; stateful latch behavior must not bypass explicit state and execution semantics.</td>
      <td>Value-only for stateless helpers; runtime/semantic review required for edge or latch behavior.</td>
    </tr>
    <tr>
      <td><code>frog.sync</code></td>
      <td>deferred candidate</td>
      <td>Communication and synchronization primitives.</td>
      <td>Queues, notifiers, semaphores, occurrences, and related coordination primitives after execution semantics are settled.</td>
      <td>Private scheduler internals, unsafe handles, unbounded shared mutable state, and implementation-specific thread APIs.</td>
      <td>Runtime-hosted and capability-managed; not a value-only library.</td>
    </tr>
    <tr>
      <td><code>frog.ref</code></td>
      <td>deferred high-risk candidate</td>
      <td>Reference-like or protected-value capabilities if accepted later.</td>
      <td>DVR-like protected values and carefully bounded reference semantics.</td>
      <td>Raw pointers, arbitrary process memory, hidden aliasing, and unsafe implementation handles as public values.</td>
      <td>Runtime-hosted and capability-managed; requires source, FIR, lowering, and safety guardrails before publication.</td>
    </tr>
    <tr>
      <td><code>frog.system</code></td>
      <td>published lightweight standard library</td>
      <td>Bounded, read-only system introspection.</td>
      <td>Platform identifier, architecture, CPU count/load, memory totals/availability, process id, executable path, current directory, environment reads, and FROG capability availability checks.</td>
      <td>Process execution, process enumeration, OS-specific APIs, permission enumeration, time/timing, hardware inventory, secrets, and making system introspection mandatory for minimal embedded profiles.</td>
      <td>OS-dependent and runtime-hosted; each call has explicit capability requirements plus local <code>success</code>/<code>error_code</code> status outputs.</td>
    </tr>
    <tr>
      <td><code>frog.process</code>, <code>frog.ffi</code>, <code>frog.net</code>, <code>frog.http</code>, <code>frog.db</code>, <code>frog.registry</code>, <code>frog.com</code>, <code>frog.dotnet</code></td>
      <td>profile-owned or future-profile candidates</td>
      <td>External integration with commands, shared libraries, network services, databases, registries, COM, ActiveX, .NET, and similar ecosystems.</td>
      <td>Only the surfaces explicitly published by a profile, such as the current Interop profile's <code>frog.connectivity.*</code> family.</td>
      <td>Intrinsic lightweight-library core. These capabilities depend on host policy, foreign runtimes, external protocols, platform APIs, or security-sensitive boundaries.</td>
      <td>Profile-owned, OS-dependent, provider-backed, or implementation-specific unless a future public profile standardizes them.</td>
    </tr>
  </tbody>
</table>

<p>
This table is a taxonomy and routing rule. A candidate row does not by itself
publish a primitive. A family becomes part of the public standard surface only
when its corresponding specification defines concrete primitive identifiers,
ports, typing, validation rules, and downstream representation posture.
</p>

<p>
Additional intrinsic library families MAY be standardized later, but they are not part of the intrinsic standardized surface unless a corresponding specification exists in this directory and is published as such.
</p>

<p>
The taxonomy is therefore open to disciplined growth, but closed against architectural drift.
Intrinsic libraries are added by explicit normative publication, not by convention or by implementation popularity.
</p>

<hr/>

<h2 id="libraries-vs-profiles">8. Libraries vs Profiles</h2>

<p>
One of the most important architectural rules in the repository is the separation between:
</p>

<ul>
  <li><strong>intrinsic standardized libraries</strong>, and</li>
  <li><strong>optional standardized profiles</strong>.</li>
</ul>

<p>
The distinction is simple:
</p>

<pre><code>If a capability is:
- generic,
- portable,
- intrinsic to the language surface,
- not dependent on a particular external runtime or ecosystem,

then it belongs in Libraries/.

If a capability is:
- optional,
- environment-dependent,
- tied to foreign runtimes, host ABIs, managed platforms, databases,
  protocols, services, target-profile classes, deployment-mode classes,
  or comparable external assumptions,
- standardized but not intrinsic to the minimal core,

then it belongs in Profiles/.
</code></pre>

<p>
Decision sketch:
</p>

<pre><code>                    New capability
                          |
                          v
         +--------------------------------------+
         | Is it intrinsic, generic, portable, |
         | and broadly language-level?         |
         +-------------------+------------------+
                             |
                   yes       |       no
                             |
                             v
                       Libraries/   -----> evaluate as Profiles/
                                            or implementation-specific extension
</code></pre>

<p>
Examples:
</p>

<ul>
  <li><code>frog.core.add</code> belongs naturally to <code>Libraries/</code>.</li>
  <li><code>frog.image.decode_file_rgba8</code> belongs to <code>Libraries/</code> when treated as a small, portable, base-distribution image primitive.</li>
  <li><code>frog.ui.property_read</code> belongs to <code>Libraries/</code> because it is part of the intrinsic executable UI interaction surface of the language model.</li>
  <li><code>frog.connectivity.*</code> does <strong>not</strong> belong to the intrinsic library core because it represents an optional interoperability capability surface and is normatively owned by the Interop profile.</li>
</ul>

<p>
This separation keeps the intrinsic language taxonomy durable, predictable, portable, and easier to implement consistently.
</p>

<p>
The same rule should guide the next widget step:
the standardized primitive widget baseline belongs in <code>Libraries/</code> only insofar as it is intrinsic, portable, and language-level.
Anything environment-specific or host-dependent remains outside this core.
</p>

<hr/>

<h2 id="naming-and-namespaces">9. Naming and Namespaces</h2>

<p>
FROG primitive identifiers use stable namespace-qualified names.
For intrinsic library primitives, the general naming patterns are:
</p>

<pre><code>frog.&lt;library&gt;.&lt;primitive&gt;
frog.&lt;library&gt;.&lt;family&gt;.&lt;primitive&gt;</code></pre>

<p>
Examples:
</p>

<ul>
  <li><code>frog.core.add</code></li>
  <li><code>frog.math.sqrt</code></li>
  <li><code>frog.collections.length</code></li>
  <li><code>frog.text.concat</code></li>
  <li><code>frog.io.read_text</code></li>
  <li><code>frog.image.decode_file_rgba8</code></li>
  <li><code>frog.signal.moving_average</code></li>
  <li><code>frog.system.platform_info</code></li>
  <li><code>frog.time.monotonic_now</code></li>
  <li><code>frog.ui.property_read</code></li>
</ul>

<p>
This naming model keeps primitive identity explicit and portable across tools, runtimes, and implementations.
</p>

<p>
Deeper hierarchical namespaces MAY be used where an intrinsic library family requires explicit sub-namespace ownership.
</p>

<p>
Other repository layers, including optional profiles, MAY define additional namespace families.
Such namespaces are not automatically intrinsic library namespaces merely because they begin with <code>frog.</code>.
Architectural ownership is defined by the specification layer that normatively owns them.
</p>

<p>
In practice:
</p>

<pre><code>Namespace prefix alone is not enough.

"frog.something.*"
does not automatically mean
"intrinsic library namespace".

Normative ownership still matters.
</code></pre>

<p>
This is especially important in the widget corridor, where:
</p>

<ul>
  <li><code>frog.widgets.*</code> identifies widget classes,</li>
  <li><code>frog.ui.*</code> identifies executable widget interaction primitives,</li>
  <li>the two namespaces are related, but not interchangeable.</li>
</ul>

<hr/>

<h2 id="library-boundaries">10. Library Boundaries</h2>

<p>
The intrinsic standardized library families in this directory are intentionally separated by responsibility.
</p>

<ul>
  <li><strong><code>frog.core.*</code></strong> owns only the foundational primitive baseline.</li>
  <li><strong><code>frog.math.*</code></strong> owns numeric scalar primitives beyond the core.</li>
  <li><strong><code>frog.collections.*</code></strong> owns collection manipulation primitives.</li>
  <li><strong><code>frog.text.*</code></strong> owns text-processing primitives.</li>
  <li><strong><code>frog.io.*</code></strong> owns file, path, resource, and byte-oriented I/O primitives.</li>
  <li><strong><code>frog.image.*</code></strong> owns portable image-data primitives such as decode-to-buffer operations.</li>
  <li><strong><code>frog.signal.*</code></strong> owns signal-processing primitives.</li>
  <li><strong><code>frog.system.*</code></strong> owns bounded, read-only system introspection primitives.</li>
  <li><strong><code>frog.time.*</code></strong> owns explicit time and timing primitives.</li>
  <li><strong><code>frog.time.*</code></strong> - wall-clock, monotonic, duration, wait, formatting/parsing, and profiling primitives</li>
  <li><strong><code>frog.ui.*</code></strong> owns executable widget interaction primitives only.</li>
</ul>

<p>
Boundary sketch:
</p>

<pre><code>frog.core.*         -&gt; foundational primitives only
frog.math.*         -&gt; math only
frog.collections.*  -&gt; collections only
frog.text.*         -&gt; text only
frog.io.*           -&gt; I/O only
frog.image.*        -&gt; image data only
frog.signal.*       -&gt; signal only
frog.system.*       -&gt; system introspection only
frog.time.*         -&gt; time and timing only
frog.ui.*           -&gt; executable UI interaction only
</code></pre>

<p>
Therefore:
</p>

<ul>
  <li><code>frog.core.*</code> MUST NOT become a generic bucket for unrelated future functionality.</li>
  <li><code>frog.io.*</code> MUST remain distinct from foreign-runtime interoperability, deployment, database access, and broader external integration concerns unless those are explicitly standardized as intrinsic libraries.</li>
  <li><code>frog.image.*</code> MUST remain distinct from Picture widget realization, camera acquisition, advanced computer vision, and implementation-specific codec stacks.</li>
  <li><code>frog.ui.*</code> MUST remain distinct from front-panel serialization, widget catalog definition, broader IDE UI editing concerns, target-profile classes, deployment-mode classes, and backend-family-specific UI binding contracts.</li>
  <li><code>frog.system.*</code> MUST remain distinct from process execution, networking, file mutation, time/timing, hardware access, permission enumeration, secret access, and host-private runtime service discovery.</li>
  <li><code>frog.time.*</code> MUST remain distinct from hard real-time profiles, private scheduler internals, UI event pumps, OS-specific clock APIs, profiling transports, system introspection, and synchronization primitives.</li>
  <li><code>frog.text.*</code> MUST remain distinct from file, path, and external-service semantics.</li>
  <li><code>frog.collections.*</code> MUST remain distinct from future specialized families unless those are explicitly standardized.</li>
  <li><code>frog.signal.*</code> MUST remain distinct from broader acquisition, streaming, tensor, or specialized domain families unless those are explicitly standardized.</li>
  <li><code>Libraries/</code> MUST NOT become a catch-all container for ecosystem-specific capability growth.</li>
  <li>Optional capability families that depend on foreign runtimes, host ABIs, managed platforms, databases, protocols, services, target-profile classes, deployment-mode classes, or comparable environment assumptions SHOULD be specified in <code>Profiles/</code> rather than in <code>Libraries/</code>.</li>
</ul>

<p>
The main anti-pattern to avoid is this:
</p>

<pre><code>Useful capability
      -&gt;
"put it in Libraries/"
      -&gt;
Libraries becomes a catch-all ecosystem bucket
      -&gt;
intrinsic language surface loses clarity
</code></pre>

<p>
This repository explicitly rejects that drift.
</p>

<p>
The same warning applies to widget growth:
the intrinsic library layer may standardize a primitive reusable widget baseline,
but MUST NOT become a dump site for every host-specific front-panel feature or runtime convenience surface.
</p>

<hr/>

<h2 id="relation-with-other-specification-layers">11. Relation with Other Specification Layers</h2>

<p>
Library specifications are used together with the rest of the FROG specification.
In particular:
</p>

<ul>
  <li><code>Expression/Diagram.md</code> defines how primitive nodes appear in executable graphs,</li>
  <li><code>Expression/Type.md</code> defines the ordinary type model used by primitive ports,</li>
  <li><code>Language/</code> defines cross-cutting execution semantics that remain distinct from library-local primitive behavior,</li>
  <li><code>Expression/Widget.md</code> and <code>Expression/Widget interaction.md</code> define the widget model and source-facing interaction model used by <code>frog.ui.*</code>,</li>
  <li><code>Expression/Widget class contract.md</code> defines the class-side legality targeted by <code>frog.ui.*</code>,</li>
  <li><code>Profiles/</code> defines optional standardized capability families that are not part of the intrinsic library core,</li>
  <li><code>IDE/Palette.md</code> may organize discovery and presentation of primitives, but does not replace library specifications.</li>
</ul>

<p>
These relationships can be visualized as follows:
</p>

<pre><code>Expression/Diagram.md
        |
        |  references primitive nodes
        v
    Libraries/
        |
        |  provide primitive-local normative contracts
        v
    Language/
        |
        |  provides cross-cutting execution semantics
        v
validated executable meaning

Profiles/
        |
        |  add optional standardized capability families
        v

IDE/Palette.md
        |
        |  exposes discoverability and insertion
        v
does not redefine primitive semantics
</code></pre>

<p>
Accordingly, a library specification is one normative input used to interpret intrinsic primitive nodes inside a validated executable graph.
It is not a replacement for the diagram specification, the language semantics, the profile layer, the widget model, or the IDE model.
</p>

<hr/>

<h2 id="relation-with-ir-lowering-and-backend-contract">12. Relation with IR, Lowering, and Backend Contract</h2>

<p>
Intrinsic primitive catalogs remain upstream from the execution-facing and backend-facing corridor.
</p>

<p>
In particular:
</p>

<ul>
  <li><code>IR/</code> MAY carry normalized references to intrinsic primitive identities,</li>
  <li><code>IR/Derivation rules.md</code> and <code>IR/Construction rules.md</code> MAY materialize primitive execution objects or support objects that refer to intrinsic primitive contracts,</li>
  <li><code>IR/Lowering.md</code> MAY later specialize those primitive-facing execution objects for a backend family,</li>
  <li><code>IR/Backend contract.md</code> MAY later declare backend-facing assumptions about primitive consumption,</li>
  <li>but none of those downstream layers becomes the normative owner of the primitive catalog itself.</li>
</ul>

<p>
The governing rule is:
</p>

<pre><code>Libraries/
    own intrinsic primitive identity and primitive-local contract

IR/
    may represent those primitives in canonical execution-facing derived form

Lowering/
    may specialize execution-facing primitive representation

Backend contract/
    may declare consumable backend-facing primitive assumptions

Implementations/
    may realize those primitives privately
</code></pre>

<p>
Therefore:
</p>

<ul>
  <li>a lowered or backend-facing form MUST NOT redefine what <code>frog.core.add</code> or <code>frog.ui.property_write</code> intrinsically are,</li>
  <li>a compiler, launcher, or runtime MAY realize only the standard-library primitives actually used by a validated program, but that optimization MUST preserve the published primitive contracts,</li>
  <li>a backend-family-specific consumption model MUST NOT become the normative source of primitive truth,</li>
  <li>a runtime-private realization of a primitive MUST NOT replace the published primitive-local contract.</li>
</ul>

<p>
This separation is essential because primitive identity must remain stable even when:
</p>

<ul>
  <li>the canonical IR introduces execution-facing explicitness,</li>
  <li>lowering changes storage, scheduling, or control realization,</li>
  <li>backend contracts declare family-specific assumptions,</li>
  <li>private runtimes choose different internal layouts.</li>
</ul>

<hr/>

<h2 id="library-evolution">13. Library Evolution</h2>

<p>
FROG begins with a compact but extensible intrinsic library taxonomy.
This is intentional.
</p>

<p>
The current intrinsic standardized library surface already covers:
</p>

<ul>
  <li>foundational computation,</li>
  <li>numeric functions,</li>
  <li>collections,</li>
  <li>text processing,</li>
  <li>I/O,</li>
  <li>signal processing,</li>
  <li>system introspection,</li>
  <li>time and timing,</li>
  <li>widget interaction.</li>
</ul>

<p>
Additional intrinsic library families MAY be added later where they remain:
</p>

<ul>
  <li>generic,</li>
  <li>portable,</li>
  <li>intrinsic to the language surface,</li>
  <li>not dependent on one specific foreign runtime, managed platform, database stack, host ABI, protocol family, target-profile family, deployment-mode family, backend-family consumption model, or vendor technology.</li>
</ul>

<p>
A lightweight standard library family MAY be shipped with a base FROG
distribution while still remaining explicit in source, FIR, lowering, backend
contracts, and deployment packaging. The fact that a base distribution provides
an implementation does not turn the capability into widget behavior, runtime
magic, or vendor-owned semantics.
</p>

<p>
Capability areas that are useful but environment-dependent SHOULD be specified as profiles or as implementation-specific extensions rather than being folded into the intrinsic library core.
</p>

<p>
When a new intrinsic library family is introduced:
</p>

<ul>
  <li>it SHOULD be added as a sibling specification in this directory,</li>
  <li>it SHOULD define clear namespace ownership,</li>
  <li>it SHOULD state its relation with <code>Language/</code>, <code>Profiles/</code>, and relevant <code>Expression/</code> documents,</li>
  <li>it SHOULD remain compatible with later execution-facing derivation, lowering, and backend-facing consumption without moving those downstream ownerships into <code>Libraries/</code>,</li>
  <li>it SHOULD be reflected in repository-level architecture and relevant IDE palette documentation where relevant.</li>
</ul>

<p>
For the current widget closure campaign, the next coherent evolution of this directory is to publish a minimal standardized primitive widget baseline whose law remains explicit, whose interaction remains aligned with <code>frog.ui.*</code>, and whose realization remains subordinate rather than normative.
</p>

<hr/>

<h2 id="status">14. Status</h2>

<p>
At the current repository stage, <code>Libraries/</code> defines a growing but intentionally controlled intrinsic standardized primitive taxonomy used by the rest of the specification.
</p>

<p>
Its role is to anchor the normative primitive vocabulary consumed by executable diagrams while remaining cleanly separated from:
</p>

<ul>
  <li>source-format structure in <code>Expression/</code>,</li>
  <li>cross-cutting execution semantics in <code>Language/</code>,</li>
  <li>optional standardized capability families in <code>Profiles/</code>,</li>
  <li>derived execution-facing representation in <code>IR/</code>,</li>
  <li>lowering and backend-facing handoff boundaries,</li>
  <li>widget and front-panel source models,</li>
  <li>IDE palette organization and authoring workflows,</li>
  <li>implementation-specific extensions.</li>
</ul>

<p>
The current direction can be summarized simply:
</p>

<pre><code>Libraries/ should stay:
- intrinsic
- portable
- explicit
- bounded
- stable

Libraries/ should not become:
- a profile bucket
- an IR bucket
- a lowering bucket
- a backend-contract bucket
- an IDE bucket
- a runtime bucket
- a vendor bucket
- a generic ecosystem dumping ground
</code></pre>

<p>
This directory is expected to evolve as the language matures, but that evolution MUST preserve explicit namespace ownership, clear architectural boundaries, and conservative integration with the rest of the specification.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
<code>Libraries/</code> is the architectural home of intrinsic standardized primitive vocabularies in FROG.
</p>

<p>
It exists to make primitive identity, ports, primitive-local metadata, and primitive-local behavior explicit and portable without corrupting the ownership boundaries of:
</p>

<ul>
  <li><code>Expression/</code>,</li>
  <li><code>Language/</code>,</li>
  <li><code>Profiles/</code>,</li>
  <li><code>IR/</code>,</li>
  <li><code>IDE/</code>.</li>
</ul>

<p>
Intrinsic libraries stay intrinsic.
Optional capability growth stays in profiles.
Execution-facing representation stays in IR.
Runtime realization stays implementation-private.
</p>

<pre><code>Libraries/
    own intrinsic primitive truth

Profiles/
    own optional capability families

IR/
    owns the canonical execution-facing bridge

Implementations/
    own private realization
</code></pre>

<p>
For the widget corridor, this means:
</p>

<ul>
  <li>widget instances remain source-owned in <code>.frog</code>,</li>
  <li>widget class law remains explicit and inspectable,</li>
  <li><code>.wfrog</code> remains the widget-oriented package publication family,</li>
  <li><code>frog.ui.*</code> remains the intrinsic executable interaction vocabulary,</li>
  <li>future primitive widget baseline publication can grow in <code>Libraries/</code> without collapsing source, behavior, realization, and runtime ownership into one layer.</li>
</ul>
