#define __STDC_WANT_LIB_EXT1__ 1 // Include `*_s` functions.
#include <stdio.h>               // fprintf_s, stderr, fopen_s
#include <stdlib.h>              // exit, perror
#include "parse.h"               // *
#include "cheats.h"              // fn
#include "cstr_buf.h"            // struct CStrBuf, init_cstr_buf,
                                 // read_to_cstr_buf

global(bool COMP_LOG) = false;

fn(void test())
{
    test_lexer();
    test_parser();
}

fn(void usage())
{
    fprintf_s(stderr, "paracc - A compiler for Parasitic C\n");
    fprintf_s(stderr, "\n");
    fprintf_s(stderr, "  USAGE:\n");
    fprintf_s(stderr, "\n");
    fprintf_s(stderr, "    paracc FILE\n");
    fprintf_s(stderr, "\n");
    fprintf_s(stderr, "    WHERE:\n");
    fprintf_s(stderr, "\n");
    fprintf_s(stderr, "      FILE - The source file that contains the entry\n");
    fprintf_s(stderr, "             point to the program.\n");

    exit(1);
}

global(int AVG_LINE_LEN) = 40;
global(int AVG_FILE_LINES) = 500;

fn(void comp_log(char *msg))
{
    if (!COMP_LOG)
        return;

    if (fprintf_s(stderr, "%s", msg) < 0)
    {
        perror("Failure to comp_log message");
        exit(1);
    }
}

fn(int main(int argc, char **argv))
{
    if (argc <= 1 || argc > 2)
        usage();
    else // argc == 2
    {
        FILE *f;

        comp_log("[OPENING FILE...]\n");
        if (fopen_s(&f, argv[1], "r") != 0)
        {
            perror("Failed to open file");
            exit(1);
        }

        struct CStrBuf buf = init_cstr_buf(AVG_LINE_LEN * AVG_FILE_LINES);
        comp_log("[READING FILE TO STRING...]\n");
        read_to_cstr_buf(f, &buf);
        comp_log("[GOT INPUT]\n```\n");
        comp_log(buf.buf);
        comp_log("\n```\n");
        if (COMP_LOG)
        {
            comp_log("[LEXING INPUT...]\n");
            lex_all_input(buf.buf);
        }
        comp_log("[PARSING INPUT...]\n");
        struct Lexer lxr = lexer_init(buf.buf);
        struct List tu = parse_translation_unit(&lxr);
        display_translation_unit(stdout, &tu);
    }

    return 0;
}
