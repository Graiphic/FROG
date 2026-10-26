# Search And Replace String — all

The existing UTF-8 literal-search target reports replacement count and the offset in the resulting String. Omitted replacement deletes a match. This version rejects empty searches and invalid UTF-8 offsets with ordinary error 1; Unicode case-folding is an explicit unavailable-provider diagnostic.

The existing UTF-8 literal-search target reports replacement count and the offset in the resulting String. Omitted replacement deletes a match. This version rejects empty searches and invalid UTF-8 offsets with ordinary error 1; Unicode case-folding is an explicit unavailable-provider diagnostic.

The existing UTF-8 literal-search target reports replacement count and the offset in the resulting String. Omitted replacement deletes a match. This version rejects empty searches and invalid UTF-8 offsets with ordinary error 1; Unicode case-folding is an explicit unavailable-provider diagnostic.

Mode: execution. Profile: studio-draft. Capability: frog.studio.string.search_replace.v1.
Expected:
text-result (string) = "blue blue"
count-result (i32) = 2
offset-result (i32) = 9
error-status (bool) = false
error-code (i32) = 0


[Open the actual FROG document](main.frog).
