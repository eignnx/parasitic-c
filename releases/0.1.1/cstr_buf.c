#ifndef PARASITIC_C_FILE__3F0
#define PARASITIC_C_FILE__3F0

// DIRECTIVES //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cheats.h"

// STRUCT DECLS //
struct CStrBuf;

// ENUM DEFNS //

// STRUCT DEFNS //
struct CStrBuf {
    int cap;
    int nbytes;
    char *buf;
};


// GLOBAL DEFNS //
int INITIAL_CSTR_BUF_CAP = 64;


// FN DECLS //
struct CStrBuf init_cstr_buf();
void cstr_buf_realloc_if_full(struct CStrBuf *s);
void read_to_cstr_buf(FILE *f, struct CStrBuf *s);

// FN DEFNS //
struct CStrBuf init_cstr_buf()
{
struct CStrBuf s;
s.cap = INITIAL_CSTR_BUF_CAP;
s.buf = (char *) malloc(s.cap);
if (s.buf == NULL)
{
perror("Failed to allocate CStrBuf");
exit(1);
}
s.buf[0] = '\0';
s.nbytes = 1;
return s;
}

void cstr_buf_realloc_if_full(struct CStrBuf *s)
{
int space_available = s->cap - s->nbytes;
if (space_available <= 0)
{
int new_cap = s->cap + s->cap;
s->buf = (char *) realloc(s->buf, new_cap);
if (s->buf == NULL)
{
perror("Could not realloc CStrBuf");
exit(1);
}
s->cap = new_cap;
}
}

void read_to_cstr_buf(FILE *f, struct CStrBuf *s)
{
while (!feof(f))
{
cstr_buf_realloc_if_full(s);
char *write_here = s->buf + strnlen_s(s->buf, s->cap);
int space_available = s->cap - s->nbytes;
char *line = fgets(write_here, space_available, f);
if (line == NULL && !feof(f))
{
perror("Error reading to CStrBuf");
exit(1);
}
if (line == NULL)
return;
int line_len = strnlen_s(line, space_available);
s->nbytes = s->nbytes + line_len + sizeof ((char) '\0');
}
}


#endif // PARASITIC_C_FILE__3F0
