/* SPDX-License-Identifier: Apache-2.0 */
#include "shared.h"
#include <stdio.h>
int main(void) {
    const unsigned int inputs[] = {0u, 5u, 29u, 4294967295u};
    for (unsigned int i = 0; i < sizeof inputs / sizeof inputs[0]; ++i)
        printf("%u %u\n", inputs[i], compute(inputs[i]));
    return 0;
}
