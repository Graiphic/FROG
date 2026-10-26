# Remove From Map — missing key

Removal emits a new Map and the removed value. A missing key preserves the Map and returns the intrinsic I32 default with key_not_found true.

Removal emits a new Map and the removed value. A missing key preserves the Map and returns the intrinsic I32 default with key_not_found true.

Removal emits a new Map and the removed value. A missing key preserves the Map and returns the intrinsic I32 default with key_not_found true.

Mode: execution. Profile: studio-draft. Capability: frog.studio.collection@1.
Expected:
result (map<string,i32>) = {"entries":[{"key":"A","value":10},{"key":"B","value":20}]}
removed (i32) = 0
not-found (bool) = true


[Open the actual FROG document](main.frog).
