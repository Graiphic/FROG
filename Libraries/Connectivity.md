<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Connectivity Transition</h1>

<p align="center">
  Transition and routing note for <strong>frog.connectivity.*</strong> in FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#status">1. Status</a></li>
  <li><a href="#why-this-file-exists">2. Why this File Exists</a></li>
  <li><a href="#normative-source">3. Normative Source</a></li>
  <li><a href="#what-changed">4. What Changed</a></li>
  <li><a href="#what-did-not-change">5. What Did Not Change</a></li>
  <li><a href="#how-to-read-this-file">6. How to Read this File</a></li>
  <li><a href="#future-family-routing">7. Future Family Routing</a></li>
  <li><a href="#summary">8. Summary</a></li>
</ul>

<hr/>

<h2 id="status">1. Status</h2>

<p>
This file is a <strong>transition stub</strong>.
</p>

<p>
It exists only to preserve continuity for readers, links, and repository navigation
for the <code>frog.connectivity.*</code> namespace.
</p>

<p>
It is <strong>not</strong> the authoritative normative specification.
</p>

<hr/>

<h2 id="why-this-file-exists">2. Why this File Exists</h2>

<p>
Earlier repository organization exposed connectivity-related material under <code>Libraries/</code>.
The repository architecture has since been clarified.
</p>

<p>
The intrinsic library layer and the optional profile layer are now separated more explicitly:
</p>

<pre>
Libraries/  -> intrinsic standardized primitive vocabularies
Profiles/   -> optional standardized capability families
</pre>

<p>
Because older expectations, bookmarks, or inbound links may still point to
<code>Libraries/Connectivity.md</code>, this file remains as a routing note.
</p>

<hr/>

<h2 id="normative-source">3. Normative Source</h2>

<p>
The authoritative normative specification for <code>frog.connectivity.*</code> is:
</p>

<pre><code>Profiles/Interop.md</code></pre>

<p>
That document MUST be treated as the primary normative source for:
</p>

<ul>
  <li>the standardized connectivity primitive surface,</li>
  <li>primitive contracts, ports, and type expectations,</li>
  <li>validation and capability boundaries,</li>
  <li>support claims related to standardized interop capabilities.</li>
</ul>

<p>
This file MUST NOT be treated as a competing normative source.
</p>

<hr/>

<h2 id="what-changed">4. What Changed</h2>

<p>
What changed is the <strong>architectural ownership</strong> of the namespace.
</p>

<p>
The <code>frog.connectivity.*</code> family is no longer classified as part of the
minimal intrinsic standardized library core.
It is now classified as an <strong>optional standardized capability family</strong>
owned by the Interop profile.
</p>

<p>
This is an architectural reclassification, not a removal from the standardized FROG surface.
</p>

<hr/>

<h2 id="what-did-not-change">5. What Did Not Change</h2>

<p>
The namespace identity remains stable:
</p>

<pre><code>frog.connectivity.*</code></pre>

<p>
This transition does not by itself rename the namespace or invalidate the existence
of the standardized connectivity capability family.
</p>

<p>
In short:
</p>

<pre>
Changed:
- architectural ownership
- repository classification

Unchanged:
- namespace prefix
- capability-family identity
</pre>

<hr/>

<h2 id="how-to-read-this-file">6. How to Read this File</h2>

<p>
Use this file only when you need to understand <strong>why</strong> connectivity still
appears under <code>Libraries/</code>.
</p>

<p>
Do <strong>not</strong> use this file for detailed primitive definitions or normative behavior.
For those questions, read:
</p>

<pre><code>Profiles/Interop.md</code></pre>

<p>
This file SHOULD remain short.
It SHOULD NOT duplicate:
</p>

<ul>
  <li>primitive catalogs,</li>
  <li>primitive-local behavioral rules,</li>
  <li>validation rules,</li>
  <li>support-level definitions,</li>
  <li>examples already owned by the Interop profile specification.</li>
</ul>

<p>
If this transition note becomes unnecessary in a later repository phase, it MAY be
reduced further or removed.
</p>

<hr/>

<h2 id="future-family-routing">7. Future Family Routing</h2>

<p>
<code>Profiles/Interop.md</code> keeps the v0.1 public
<code>frog.connectivity.*</code> posture in one normative profile document.
That document publishes the current conservative request/response families and
records where future optional families may route.
</p>

<p>
Future connectivity work may add or split profile-owned families for process
execution, typed native FFI, HTTP or network clients, advanced database
sessions, host input devices, Windows-specific integration, service hosting,
callbacks, streaming, or live data. Those families remain outside the
intrinsic lightweight library core unless a later public decision explicitly
promotes a narrow portable subset.
</p>

<p>
This transition note does not define those future primitives. It only points
readers back to the Interop profile as the public authority for claimable
connectivity behavior.
</p>

<hr/>

<h2 id="summary">8. Summary</h2>

<p>
<code>frog.connectivity.*</code> remains part of the standardized FROG specification surface,
but its normative home is now <strong>Profiles/Interop.md</strong>, not the intrinsic
<code>Libraries/</code> core.
</p>

<p>
This file remains only for continuity and routing.
</p>

<pre>
Need the actual connectivity specification?
        |
        v
Profiles/Interop.md

Need to know why this file still exists?
        |
        v
This transition stub explains that
</pre>
