# Queue: capacity, Preview, controlled loss and Flush

Queue: capacity, Preview, controlled loss and Flush

A Queue<f64> of capacity 2 receives 10, then 20. Preview reads 10 without removing it: Status still returns [10,20] and count=2. Lossy Enqueue of 30 evicts exactly 10; Flush returns [20,30]. Dequeue with timeout=0 then observes an empty queue without creating a value. Release closes the handle and returns an empty remaining array.

Queue: capacity, Preview, controlled loss and Flush

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
