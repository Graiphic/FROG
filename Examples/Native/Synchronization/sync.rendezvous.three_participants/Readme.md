# Rendezvous : trois préparations indépendantes

Rendezvous : trois préparations indépendantes

Run this document. Three independent exact-i32 Increment chains perform one, two and three increments from -2, -3 and -4; each computes timeout -1. All three Waits receive Create directly. passed_0/1/2 are False only after their successful barrier passage. Destroy consumes a typed Record join of all three outcomes; it cannot run directly after Create. These chains use the verified composed Increment profile without claiming other numeric corridors.

Rendezvous : trois préparations indépendantes

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
prepared_0 (i32) = -1
passed_0 (bool) = false
prepared_1 (i32) = -1
passed_1 (bool) = false
prepared_2 (i32) = -1
passed_2 (bool) = false


[Open the actual FROG document](main.frog).
