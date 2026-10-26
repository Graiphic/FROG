#ifndef FROG_EXAMPLE_RESOURCES_STRUCTURES_H
#define FROG_EXAMPLE_RESOURCES_STRUCTURES_H

#include <stdint.h>

#if defined(_WIN32)
#  if defined(FROG_EXAMPLE_NATIVE_LIBRARY_BUILD)
#    define FROG_EXAMPLE_API __declspec(dllexport)
#  else
#    define FROG_EXAMPLE_API __declspec(dllimport)
#  endif
#  define FROG_EXAMPLE_CALL __cdecl
#else
#  define FROG_EXAMPLE_API __attribute__((visibility("default")))
#  define FROG_EXAMPLE_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ABI version 1: fixed-width scalars, C linkage, by value, no allocation,
 * ownership transfer, process state, callback or platform API dependency.
 * Functions are reentrant. Each call is synchronous and bounded. */
FROG_EXAMPLE_API int32_t FROG_EXAMPLE_CALL FrogExampleEchoI32V1(int32_t value);
FROG_EXAMPLE_API uint32_t FROG_EXAMPLE_CALL FrogExampleEchoU32V1(uint32_t value);
FROG_EXAMPLE_API int64_t FROG_EXAMPLE_CALL FrogExampleEchoI64V1(int64_t value);
FROG_EXAMPLE_API uint64_t FROG_EXAMPLE_CALL FrogExampleEchoU64V1(uint64_t value);
FROG_EXAMPLE_API double FROG_EXAMPLE_CALL FrogExampleEchoF64V1(double value);
/* The example calls this with digits 0..9. The specified ABI order is a,b,c;
 * presentation order in the caller has no effect on the result 100*a+10*b+c. */
FROG_EXAMPLE_API int32_t FROG_EXAMPLE_CALL FrogExamplePackDigitsV1(int32_t a, int32_t b, int32_t c);

#ifdef __cplusplus
}
#endif
#endif
