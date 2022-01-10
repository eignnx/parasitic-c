#ifndef STD_PARAC_H
#define STD_PARAC_H

#include <stdio.h>  // fprintf, stderr
#include <string.h> // _strdup

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

// These are psuedo keywords that will be used in the defined language.
#define fn(signature) signature
#define fndecl(signature) signature
#define global(type_and_name) type_and_name
// #define let(type_and_name) type_and_name
// #define cast(type) (type)

#ifdef _WIN32
#define strdup _strdup
#endif

inline char path_separator()
{
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

#endif // STD_PARAC_H
