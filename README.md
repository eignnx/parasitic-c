# Parasitic C
A language that compiles to C. Initially, it will be built by parsing C code into an AST, then outputting that AST as C code again. This will make the language trivially bootstrappable, and I can begin evolving the language away from C from there.

## Running

I'm working on Windows, and I happen to have `zig` installed, so I compile and run the compile like this:

```powershell
> make head
> .\tgt\paracc-HEAD.exe my-file.parac
> zig cc my-file.c -std=c11 -o a.exe
```

## Example

```mun
#include <stdio.h>

data Tm { // Algebraic data type
    Int { value: int },
    Nop,
    Add { x: data Tm *, y: data Tm * },
    Print { arg: data Tm * },
}

fn eval(tm: data Tm *) -> int {
    if let Tm::Int { value = v: int } = *tm {
        return v;
    }
    if let Tm::Nop = *tm {
        return 0;
    }
    if let Tm::Print { arg: data Tm * } = *tm { // Name puns on data variant destructuring
        let res: int = eval(arg);
        printf("%d\n", res);
        return res;
    }
    if let Tm::Add { x = x: data Tm *, y = y: data Tm * } = *tm {
        let xx: int = eval(x);
        let yy: int = eval(y);
        return xx + yy;
    }
    return 0;
}

fn main() -> int {
    let one: data Tm = Tm::Int { value = 1 };
    let two: data Tm = Tm::Int { value = 2 };
    let sum: data Tm = Tm::Add { x = &one, y = &two };
    let put: data Tm = Tm::Print { arg = &sum };
    {
        let value: int = 1234;
        let blarg: data Tm = Tm::Int { value }; // Name puns on data variant construction
    }
    let res: int = eval(&put);
    printf("result = %d\n", res);
    return 0;
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
- [X] Complete parser
- [X] Output AST as C code
- [X] Bootstrap the compiler
- [X] Implement algebraic data types
- [ ] Implement generalized pattern matching
- [ ] Allow array types
- [ ] Add a type checker