/* SPDX-License-Identifier: Apache-2.0 */
#include "shared.h"
#include "generated.h"
static unsigned int adjust(unsigned int value) { return value * SCALE_VALUE; }
unsigned int middle(unsigned int value) { return leaf(adjust(value)) + GENERATED_VALUE; }
