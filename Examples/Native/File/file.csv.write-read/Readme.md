# CSV — write and read a spreadsheet

Choose an absolute base filename in a writable folder (for example, readings.csv), then run in Simulation. Create Incrementing reserves a new file. Write two rows separated by semicolons, read the four numbers back, then delete the file that this example created. An existing file with the chosen name is preserved.

Choose an absolute base filename in a writable folder (for example, readings.csv), then run in Simulation. Create Incrementing reserves a new file. Write two rows separated by semicolons, read the four numbers back, then delete the file that this example created. An existing file with the chosen name is preserved.

Choose an absolute base filename in a writable folder (for example, readings.csv), then run in Simulation. Create Incrementing reserves a new file. Write two rows separated by semicolons, read the four numbers back, then delete the file that this example created. An existing file with the chosen name is preserved.

Mode: interactive. Profile: studio-draft. Capability: Studio File I/O in Simulation, with a user-selected writable path.
Expected:
readings (array<f64,rank=2>) = {"shape":[2,2],"elements":[1.25,2.5,3.75,4]}
error-out (cluster<"status":bool,"code":i32,"source":string>) = {"status":false,"code":0,"source":""}


[Open the actual FROG document](spreadsheet.frog).
