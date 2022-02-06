#ifndef STD_PARAC_H
#define STD_PARAC_H

#include <stdio.h>  // fprintf, stderr
#include <stdlib.h> // malloc
#include <string.h> // _strdup
#include <stdarg.h> // va_list, va_start, va_copy, va_end

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

char *format_alloc(const char *fmt_str, ...)
{
    va_list args1;
    va_start(args1, fmt_str);
    va_list args2;
    va_copy(args2, args1);

    int buf_sz = vsnprintf(NULL, 0, fmt_str, args1) + 1;
    va_end(args1);
    char *str = malloc(buf_sz);
    vsnprintf(str, buf_sz, fmt_str, args2);
    va_end(args2);
    return str;
}

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
