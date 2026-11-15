/* SPDX-License-Identifier: Apache-2.0 */
#include "shared.h"
static unsigned int adjust(unsigned int value) { return value + 2u; }
unsigned int compute(unsigned int value) { return middle(adjust(value)); }
