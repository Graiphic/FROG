# Search And Replace String — casefold-unavailable

The existing UTF-8 literal-search target reports replacement count and the offset in the resulting String. Omitted replacement deletes a match. This version rejects empty searches and invalid UTF-8 offsets with ordinary error 1; Unicode case-folding is an explicit unavailable-provider diagnostic.

The existing UTF-8 literal-search target reports replacement count and the offset in the resulting String. Omitted replacement deletes a match. This version rejects empty searches and invalid UTF-8 offsets with ordinary error 1; Unicode case-folding is an explicit unavailable-provider diagnostic.

The existing UTF-8 literal-search target reports replacement count and the offset in the resulting String. Omitted replacement deletes a match. This version rejects empty searches and invalid UTF-8 offsets with ordinary error 1; Unicode case-folding is an explicit unavailable-provider diagnostic.

Mode: expected-diagnostic. Profile: studio-draft. Capability: frog.studio.string.search_replace.v1.
Expected:
Diagnostic: native.string.casefold_unavailable

[Open the actual FROG document](main.frog).
