# Queue : capacité, Preview, perte contrôlée et Flush

Queue : capacité, Preview, perte contrôlée et Flush

Une Queue<f64> de capacité 2 reçoit 10 puis 20. Preview lit 10 sans l'enlever : Status retourne toujours [10,20] et count=2. Lossy Enqueue de 30 évince exactement 10 ; Flush retourne [20,30]. Dequeue avec timeout=0 constate ensuite une file vide, sans créer de valeur. Release ferme le handle et retourne un tableau restant vide.

Queue : capacité, Preview, perte contrôlée et Flush

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
preview (f64) = 10
count (i32) = 2
capacity (i32) = 2
before (array<f64>) = [10,20]
overflow (bool) = true
evicted (f64) = 10
flushed (array<f64>) = [20,30]
empty_timeout (bool) = true
remaining (array<f64>) = []
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
