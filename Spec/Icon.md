<h1 align="center">🐸 FROG Icon Specification</h1>

<p align="center">
Icon definition for <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose</a></li>
  <li><a href="#location">3. Location in a .frog file</a></li>
  <li><a href="#structure">4. Icon object structure</a></li>
  <li><a href="#svg-requirements">5. SVG requirements</a></li>
  <li><a href="#coordinate-system">6. Coordinate system and sizing</a></li>
  <li><a href="#embedding">7. Embedding rules</a></li>
  <li><a href="#optional-fields">8. Optional fields</a></li>
  <li><a href="#examples">9. Examples</a></li>
  <li><a href="#validation">10. Validation rules</a></li>
  <li><a href="#summary">11. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>icon</strong> section defines the graphical icon used to represent a Frog when displayed as a node in a diagram.
</p>

<p>
Icons are intended for development environments and visualization tools.
They MUST NOT affect execution semantics.
</p>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The icon provides a compact, recognizable representation of a Frog.
</p>

<ul>
  <li>Improves readability in diagrams</li>
  <li>Provides consistent visual identity</li>
  <li>Enables library browsing and search previews</li>
</ul>

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

<p>
The icon object is an optional top-level section in a <code>.frog</code> file.
</p>

<pre>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {},
  "icon": { }
}</pre>

<p>
The <code>icon</code> object MAY be omitted.
If omitted, tools MAY render a default icon.
</p>

<hr/>

<h2 id="structure">4. Icon object structure</h2>

<p>
Minimal icon structure:
</p>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg&gt;...&lt;/svg&gt;"
}</pre>

<p>
Fields:
</p>

<ul>
  <li><code>size</code> (integer) — logical icon size in pixels (v0.1 default is 40)</li>
  <li><code>svg</code> (string) — SVG content</li>
</ul>

<hr/>

<h2 id="svg-requirements">5. SVG requirements</h2>

<p>
The <code>svg</code> field MUST contain valid SVG markup.
</p>

<ul>
  <li>The SVG MUST be self-contained.</li>
  <li>The SVG MUST NOT reference external assets (no external images, no remote fonts).</li>
  <li>The SVG SHOULD avoid scripts and animations.</li>
  <li>The SVG MUST be safe to render in a sandboxed environment.</li>
</ul>

<p>
For interoperability, icons SHOULD use simple shapes and paths:
</p>

<ul>
  <li><code>&lt;path&gt;</code></li>
  <li><code>&lt;rect&gt;</code></li>
  <li><code>&lt;circle&gt;</code></li>
  <li><code>&lt;text&gt;</code> (optional)</li>
</ul>

<hr/>

<h2 id="coordinate-system">6. Coordinate system and sizing</h2>

<p>
In v0.1, icons are expected to be designed on a <strong>40×40 logical grid</strong>.
</p>

<p>
The SVG SHOULD declare:
</p>

<ul>
  <li><code>width="40"</code> and <code>height="40"</code>, or</li>
  <li>a <code>viewBox="0 0 40 40"</code></li>
</ul>

<p>
Recommended:
</p>

<pre>&lt;svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40"&gt;...&lt;/svg&gt;</pre>

<p>
Tools MAY scale icons for high-DPI rendering.
The logical coordinate system MUST remain consistent.
</p>

<hr/>

<h2 id="embedding">7. Embedding rules</h2>

<p>
The SVG is embedded as a JSON string.
</p>

<p>
Implementations MUST preserve SVG content exactly, except for JSON escaping.
</p>

<ul>
  <li>Newlines MAY be preserved.</li>
  <li>Whitespace MAY be normalized by formatting tools.</li>
  <li>Tools SHOULD provide a stable formatting strategy to avoid noisy diffs.</li>
</ul>

<hr/>

<h2 id="optional-fields">8. Optional fields</h2>

<p>
Tools MAY add optional icon fields for editing and rendering.
</p>

Example:

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg&gt;...&lt;/svg&gt;",
  "editable_layers": true,
  "hint": {
    "category": "math",
    "preferred_bg": "dark"
  }
}</pre>

Rules:

<ul>
  <li>Unknown fields MUST be ignored by runtimes.</li>
  <li>Unknown fields SHOULD be preserved by formatting tools.</li>
</ul>

<hr/>

<h2 id="examples">9. Examples</h2>

<h3>9.1 Minimal icon</h3>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 40 40'&gt;&lt;rect x='2' y='2' width='36' height='36' rx='6'/&gt;&lt;/svg&gt;"
}</pre>

<h3>9.2 Icon with simple symbol</h3>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 40 40'&gt;
            &lt;rect x='2' y='2' width='36' height='36' rx='6' fill='#1f2937'/&gt;
            &lt;path d='M12 20h16' stroke='#ffffff' stroke-width='3' stroke-linecap='round'/&gt;
            &lt;path d='M20 12v16' stroke='#ffffff' stroke-width='3' stroke-linecap='round'/&gt;
          &lt;/svg&gt;"
}</pre>

<hr/>

<h2 id="validation">10. Validation rules</h2>

Implementations MUST enforce:

<ul>
  <li>If <code>icon</code> exists, <code>icon.size</code> MUST be an integer.</li>
  <li>If <code>icon</code> exists, <code>icon.svg</code> MUST be a string containing valid SVG.</li>
  <li>The SVG SHOULD include a 40×40 viewBox or equivalent sizing strategy.</li>
  <li>Icons MUST NOT affect execution semantics.</li>
</ul>

Tools MAY provide stricter validation for safety (e.g., rejecting scripts).

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
The icon section provides an optional SVG-based icon for a Frog.
</p>

<ul>
  <li>Icons are embedded as SVG strings in JSON.</li>
  <li>v0.1 targets a 40×40 logical grid.</li>
  <li>Icons must be safe, self-contained, and tool-agnostic.</li>
  <li>Icons do not affect execution.</li>
</ul>
