#include <stdio.h>
#include <stdbool.h>
#include "../cheats.h"
#include "goodbye_world.parac.h"

fn(void main())
{
    printf("Hello, World!\n");

    // Test that this doesn't break the lexer. It needs to not parse as an
    // `TOK_ANGLE_BRACKET_FILENAME`.
    bool condition = 1 < 100 && 100 > 1;

    goodbye();
    return 0;
}
