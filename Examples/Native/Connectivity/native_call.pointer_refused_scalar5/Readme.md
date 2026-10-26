# Appel C : signature pointeur non admise par scalar5

Appel C : signature pointeur non admise par scalar5

Le vrai nœud NativeCall utilise la bibliothèque C livrée avec les exemples, via un chemin relatif au document. Profil scalar5 : I32/U32/I64/U64/F64 par valeur, thread Any, C ABI. Les IDs param.<id>.in/out sont persistants. Les arguments restent dans l'ordre ABI même lorsque leurs lignes visuelles sont inversées. Les nombres entiers ne transitent jamais par un double.

Appel C : signature pointeur non admise par scalar5

Mode: expected-diagnostic. Profile: studio-draft. Capability: windows-x64/native-call.scalar5@1.
Expected:
Diagnostic: migration.native_call_signature_unavailable

[Open the actual FROG document](main.frog).
