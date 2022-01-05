#ifndef STD_PARAC_H
#define STD_PARAC_H

typedef char *cstr_arr[];

#define unimplemented                                                          \
    printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__); \
    exit(1)

#endif // STD_PARAC_H
