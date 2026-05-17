<h1>Example 16 LLVM Native Boundary</h1>

<p>
This directory publishes the native-artifact boundary for
<code>Examples/16_picture_logo_jpeg</code>.
</p>

<p>
The lowered unit calls the standard
<code>frog.image.decode_file_rgba8</code> primitive. The LLVM IR models the
native entry point and provider call boundary. The manifest declares the ABI,
dynamic artifact filename, entry symbol, result release symbol, input, output,
source artifact, and required <code>frog.image</code> provider contract.
</p>

<p>
The runtime must consume this manifest-declared boundary. It must not treat
LLVM itself as the runtime identity, and it must not make the Picture widget
responsible for image decoding.
</p>

<p>
The runtime resolves the native image kernel from the manifest. It should not
hardcode the Example 16 entry symbol; the manifest-owned
<code>entry.symbol</code> and <code>entry.result_release_symbol</code> define
the callable surface.
</p>
