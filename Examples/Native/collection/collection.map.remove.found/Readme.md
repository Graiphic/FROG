# Remove From Map — present key

Removal emits a new Map and the removed value. A missing key preserves the Map and returns the intrinsic I32 default with key_not_found true.

Removal emits a new Map and the removed value. A missing key preserves the Map and returns the intrinsic I32 default with key_not_found true.

Removal emits a new Map and the removed value. A missing key preserves the Map and returns the intrinsic I32 default with key_not_found true.

Mode: execution. Profile: studio-draft. Capability: frog.studio.collection@1.
Expected:
result (map<string,i32>) = {"entries":[{"key":"B","value":20}]}
removed (i32) = 10
not-found (bool) = false


[Open the actual FROG document](main.frog).
