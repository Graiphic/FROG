# Native example library — ABI 1

This small C library belongs to the FROG example corpus. It contains no OS calls,
heap allocations, callbacks, global mutable state, private SDK types or C++ ABI.
Its source and header are redistributable with the Studio examples under the
repository licence. The normal Studio build creates `frog-example-native-library`;
the distribution copies that target into `Examples/Native/lib`.

The public header is the exact ABI: C linkage, cdecl on Windows, fixed-width
I32/U32/I64/U64 and binary64 double by value. All functions are synchronous,
reentrant and have no ownership or lifetime requirements beyond their call.
`FrogExamplePackDigitsV1(a,b,c)` returns `100*a+10*b+c` for three digits; it returns
`-1` for an out-of-range argument. This makes visual argument reordering
observable without signed overflow.

The portable source can also be rebuilt without a FROG SDK:

```text
cl /nologo /LD /TC resources_structures.c /link /OUT:frog-example-native-library.dll
cc -std=c11 -fPIC -shared resources_structures.c -o libfrog-example-native-library.so
```

These commands build the library, not the Studio application. Current executable
examples target Windows x64 and the existing local scalar5 NativeCall provider.
The presence of portable C source does not claim a Linux/macOS Studio provider.
The scalar5 examples deliberately refuse pointer, array, string, callback and
UI-thread signatures; other existing legacy providers retain their own scope.
