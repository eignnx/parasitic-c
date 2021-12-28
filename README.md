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
