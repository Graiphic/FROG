# Rendezvous : seuil par défaut et variation nulle

Rendezvous : seuil par défaut et variation nulle

Create utilise le seuil initial 2. Une arrivée avec timeout=0 ne suffit pas et est retirée. Resize sans size_change utilise la variation 0 et conserve le seuil 2. Le compteur d'attentes revient à 0. Aucun message n'est transporté.

Rendezvous : seuil par défaut et variation nulle

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
timed_out (bool) = true
size (i32) = 2
new_size (i32) = 2
waiting (i32) = 0
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
