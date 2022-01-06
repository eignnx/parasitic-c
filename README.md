# Parasitic C
A language that compiles to C. Initially, it will be built by parsing C code into an AST, then outputting that AST as C code again. This will make the language trivially bootstrappable, and I can begin evolving the language away from C from there.

I'm keeping the code intentionally simple right now so that fewer constructs have to be implemented (hence no enums or static arrays).

## Running

I'm working on Windows, and I happen to have `zig` installed, so I compile and run the compile like this:

```powershell
> zig cc paracc.c -o paracc.exe; if ($?) { .\paracc.exe }
```

## Example

The following code is currently lexable.

```c
#include <stdio.h>
#include "my_file.h"

struct my_type {
    int x;
};

void my_func123()
{ // this is a comment!
    bool my_bool = true;
    int my_int = -1234;
    char my_char1 = '\n';
    char my_char2 = 'A';
    char my_char3 = '\'';
    char *my_str = "qwea sdf sgd \" \n rty";
}
```

## C Features NOT Supported
These are features of C that the *initial* C compiler will *not* support due to complexity. These features may be added into the *new* language eventually.

- Array types
    - Arrays will be avoided whenever possible, and `typedef`ed into external header files when unavoidable. I just don't want to complicate the parser since you cannot specify an array type on its own without mentioning an associated identifier. For example: `int my_ints[]` is an object who's type cannot be written apart from the identifier `my_ints` (`int[]` is not a valid type spelling in C).
- Macro definitions
    - Externally-defined macros may be used, but macros cannot be defined due to complexity of parsing.
- Typedefs
    - Because a recursively defined struct `Foo` must be referred to with the `struct` keyword in its definition, and because `typedef`ing `struct Foo` to just `Foo` would not change this fact, typedefs are not allowed. All non-atomic types must be prefixed with either `struct`, `union`, or `enum`. This means that any `typedef`ed type included from header files *must* be specified in the lexer as its own token, and parsed as an atomic type.

## TODO
- [X] Complete lexer
- [ ] Complete parser
- [ ] Output AST as C code
- [ ] Add new language features