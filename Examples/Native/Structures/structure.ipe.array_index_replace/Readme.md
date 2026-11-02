# In Place: index and replace an array element

In Place: index and replace an array element

Index 1 reads 20 from [10,20,30]. The body adds 1 and writes 21 through the replacement side. The result is [10,21,30], while the source branch retains [10,20,30]. The structure is not a semaphore and promises no global lock.

In Place: index and replace an array element

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
result (array<f64>) = [10,21,30]
original (array<f64>) = [10,20,30]


[Open the actual FROG document](main.frog).
