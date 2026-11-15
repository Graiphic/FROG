# Named rows keep selected field order

The selected row order is second, text, first, deliberately different from the Cluster declaration. Replacing only second must preserve first and text; every output is verified before and after replacement.

The selected row order is second, text, first, deliberately different from the Cluster declaration. Replacing only second must preserve first and text; every output is verified before and after replacement.

The selected row order is second, text, first, deliberately different from the Cluster declaration. Replacing only second must preserve first and text; every output is verified before and after replacement.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
before-second (i32) = 22
before-text (string) = "kept"
before-first (i32) = 11
after-second (i32) = 99
after-text (string) = "kept"
after-first (i32) = 11


[Open the actual FROG document](main.frog).
