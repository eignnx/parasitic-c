#include <stdio.h>  // fgets
#include <stdlib.h> // malloc, realloc, exit, perror
#include <string.h> // strnlen_s
#include "cheats.h" // fn

struct CStrBuf
{
    int len;
    int cap;
    char *buf;
};

fn(struct CStrBuf init_cstr_buf(int initial_cap))
{
    struct CStrBuf s;
    s.cap = initial_cap;
    s.len = 0;
    s.buf = (char *)malloc(initial_cap);

    if (s.buf == NULL)
    {
        perror("Failed to allocate CStrBuf");
        exit(1);
    }

    return s;
}

fn(void cstr_buf_realloc_if_full(struct CStrBuf *buf))
{
    if (buf->len >= buf->cap)
    {
        buf->buf = (char *)realloc(buf->buf, buf->cap * 2);
        if (buf->buf == NULL)
        {
            perror("Could not realloc CStrBuf");
            exit(1);
        }
    }
}

fn(void read_to_cstr_buf(FILE *f, struct CStrBuf *buf))
{
    while (!feof(f))
    {
        cstr_buf_realloc_if_full(buf);

        char *line = fgets(buf->buf + buf->len, buf->cap - buf->len, f);
        buf->len = buf->len + strnlen_s(buf->buf + buf->len, buf->cap - buf->len);

        if (line == NULL && !feof(f))
        {
            perror("Error reading to CStrBuf");
            exit(1);
        }
    }
}
