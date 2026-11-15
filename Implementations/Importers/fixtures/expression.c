/* SPDX-License-Identifier: Apache-2.0 */
#include "expression.h"

typedef struct Parser { unsigned nodes; int error; } Parser;
typedef struct Value { double number; unsigned depth; } Value;

static Value fail(Parser *parser, int code)
{
    if (!parser->error) parser->error = code;
    return (Value){0, 0};
}

static Value checked(Parser *parser, double number, unsigned depth)
{
    if (++parser->nodes > 2000 || depth > 24) return fail(parser, 7308);
    if (!(__builtin_fpclassify(2, 1, -1, -2, 0, number) <= 0)) return fail(parser, 7306);
    return (Value){number, depth};
}

int fixture_check(double number, unsigned depth)
{
    Parser parser = {0, 0};
    (void)checked(&parser, number, depth);
    return parser.error;
}
