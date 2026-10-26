#define FROG_EXAMPLE_NATIVE_LIBRARY_BUILD 1
#include "resources_structures.h"

int32_t FROG_EXAMPLE_CALL FrogExampleEchoI32V1(int32_t value) { return value; }
uint32_t FROG_EXAMPLE_CALL FrogExampleEchoU32V1(uint32_t value) { return value; }
int64_t FROG_EXAMPLE_CALL FrogExampleEchoI64V1(int64_t value) { return value; }
uint64_t FROG_EXAMPLE_CALL FrogExampleEchoU64V1(uint64_t value) { return value; }
double FROG_EXAMPLE_CALL FrogExampleEchoF64V1(double value) { return value; }
int32_t FROG_EXAMPLE_CALL FrogExamplePackDigitsV1(int32_t a, int32_t b, int32_t c) {
    /* Restrict the teaching function to digits instead of risking signed
     * overflow for arbitrary ABI inputs. -1 is outside the admitted result. */
    if (a < 0 || a > 9 || b < 0 || b > 9 || c < 0 || c > 9) return -1;
    return 100 * a + 10 * b + c;
}
