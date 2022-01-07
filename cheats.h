#ifndef STD_PARAC_H
#define STD_PARAC_H

#include <stdio.h> // fprintf, stderr

// A file for putting temporary C code that can't be parsed
// by the Parasitic C compiler. This file can be included as
// a C header file and the definitions used in Parasitic C
// programs.

typedef char *cstr_arr[];

#define todo                                                          \
    do                                                                \
    {                                                                 \
        fprintf(stderr, "\n%s:%d: work needed in function `%s`!\n\n", \
                __FILE__, __LINE__, __func__);                        \
        exit(1);                                                      \
    } while (false)

#endif // STD_PARAC_H
