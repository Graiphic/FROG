# Occurrence: one signal wakes three waits

Occurrence: one signal wakes three waits

Three independent Wait operations share the reference from the same Generate. ignore_previous=False allows Set to be observed regardless of branch interleaving. All three timed_out outputs are False. This is a shared signal, not a queue of three messages.

Occurrence: one signal wakes three waits

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
timed_out_0 (bool) = false
timed_out_1 (bool) = false
timed_out_2 (bool) = false


[Open the actual FROG document](main.frog).
