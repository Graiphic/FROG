<a id="json-et-sha-pour-les-auteurs-de-bridges"></a>

# JSON and SHA for bridge authors

`frog/bridge_json.hpp` provides a C++20 source API in `frog::bridge::json`. The third-party binary contract remains the Bridge IPC protocol. The public header contains no private Frontend, Studio, Runtime or LLVM type or include. Its implementation delegates to the same `FrogJson` in `PreserveLexeme` mode, encoder and SHA-256 as the production Frontend; it introduces no parallel parser or semantic catalog.

```cpp
#include <frog/bridge_json.hpp>

frog::bridge::Error error;
auto root = frog::bridge::json::parse(payloadBytes, error);
if (!root) { /* rejection attributed through error.code/message/pointer */ }
else if (root->kind() == frog::bridge::json::Kind::Object) {
    auto count = root->member("count");
    if (count && count->kind() == frog::bridge::json::Kind::Number) {
        auto exact = count->numberLexeme(); // no conversion to double
        // The bridge profile owns numerical conversion and its bounds.
    }
}
```

`Value` is a copyable handle to an immutable tree. A child obtained through `member`, `elements` or `members` retains the complete tree after the root handle and input bytes are destroyed. A returned `string_view` remains valid while any handle to the same tree survives. `members()` copies keys and enumerates the encoder's deterministic order; `elements()` preserves array order.

An absent `member` returns `nullopt`. A present member containing `null` returns a `Kind::Null` `Value`. Accessors require their exact type: a wrong type or moved-from handle produces `std::logic_error`, with no fabricated value. `numberLexeme()` preserves I64/U64, `-0`, exponents and decimal precision. It admits no language or target conversion; a JSON number such as `1e400` may be readable, then rejected by a finite f64 profile.

`parse(bytes, error, maxBytes)` uses an effective limit of `min(maxBytes, 16 MiB)`. The common parser rejects duplicate keys, malformed UTF-8, isolated surrogates, incomplete JSON, depth over 128 and more than one million values. Size is checked before parsing. Success clears `Error`; rejection returns `nullopt`, never a partial tree. `Error.pointer` may contain the actual `@byte:N` cursor; it is not a Source JSON Pointer.

`encode` calls the common deterministic encoder and preserves number lexemes. It does not preserve the input document's formatting or member order. Artifact hashes must therefore be computed from **original bytes**, not this re-encoding. `quote` escapes through the same owner, checks UTF-8 with the same parser and limits its result to 16 MiB; it throws `invalid_argument` for an invalid string and `length_error` on overflow. `sha256` hashes exact bytes, including NUL, without interpreting text.

JSON reading and SHA prove neither FIR validity, profile compatibility, trust nor execution admission. Those stages retain their owners. Handles retain the complete tree; their size is not a measure of memory independent of that tree. This API adds no transport or thread and reads no file, network, registry or environment variable.

# Exact spans

`Value::originalUtf8()` returns the shared parser's exact token span, excluding surrounding whitespace. Child handles retain both the parsed tree and original input bytes. Use this accessor when binding a receipt to provenance bytes; `encode()` deliberately produces deterministic JSON and can change ordering or whitespace.
