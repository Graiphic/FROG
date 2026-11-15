# Appel C : ordre visuel différent de l'ordre ABI

Appel C : ordre visuel différent de l'ordre ABI

Le vrai nœud NativeCall utilise la bibliothèque C livrée avec les exemples, via un chemin relatif au document. Profil scalar5 : I32/U32/I64/U64/F64 par valeur, thread Any, C ABI. Les IDs param.<id>.in/out sont persistants. Les arguments restent dans l'ordre ABI même lorsque leurs lignes visuelles sont inversées. Les nombres entiers ne transitent jamais par un double.

Appel C : ordre visuel différent de l'ordre ABI

Mode: execution. Profile: studio-draft. Capability: windows-x64/native-call.scalar5@1.
Expected:
returned (i32) = 123
error_status (bool) = false
error_code (i32) = 0
mirror_0 (i32) = 1
mirror_1 (i32) = 2
mirror_2 (i32) = 3


[Open the actual FROG document](main.frog).
