// TODO: figure out a way to set compiler flags like this.
// #define __STDC_WANT_LIB_EXT1__ 1 // Include `*_s` functions.
#include <stdio.h>          // fprintf_s, stderr, fopen_s
#include <stdlib.h>         // exit, perror
#include <string.h>         // strrchr
#include "parse.parac.h"    // *
#include "cheats.h"         // fn
#include "cstr_buf.parac.h" // struct CStrBuf, init_cstr_buf,
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

// "parse.parac.h" --> "parse.h"
// TODO: BUG: ".\thing.parac.h" doesn't work because of leading dot
fn(char *remove_parac_extension(char *filename))
{
    char *extension = strchr(filename, '.');
    if (!extension)
    {
        fprintf_s(stderr, "`%s` doesn't have a file extension\n", filename);
        exit(1);
    }

    if (!(strcmp(extension, ".parac.c") == 0 ||
          strcmp(extension, ".parac.h") == 0)) // TODO: use strncmp for safety
    {
        fprintf_s(stderr, "`%s` is not a `.parac.*` file\n", filename);
        exit(1);
    }

    int ext_len = strlen(".parac.*");
    int filename_len = strlen(filename); // TODO: use strnlen_s

    int buf_sz = filename_len - ext_len + 2 + 1;
    char *new_filename = malloc(buf_sz);

    char c_or_h;
    if (filename[filename_len - 1] == 'c')
        c_or_h = 'c';
    else if (filename[filename_len - 1] == 'h')
        c_or_h = 'h';
    else
        todo; // Error reporting

    if (snprintf(new_filename, buf_sz,
                 "%.*s.%c", filename_len - ext_len, filename,
                 c_or_h) < 0) // TODO: use snprintf_s
    {
        perror("Could not make new_filename");
        exit(1);
    }

    return new_filename;
}

fn(void compile_file(char *filename))
{
    FILE *f;

    comp_log("[OPENING FILE...]\n");
    if (fopen_s(&f, filename, "r") != 0)
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

    FILE *out;

    char *new_filename = remove_parac_extension(filename);

    comp_log("[OPENING OUTPUT FILE...]\n");
    if (fopen_s(&out, new_filename, "w") != 0)
    {
        perror("Failed to open file");
        exit(1);
    }

    display_translation_unit(out, &tu);
}

fn(int main(int argc, char **argv))
{
    if (argc <= 1 || argc > 2)
        usage();
    else // argc == 2
        compile_file(argv[1]);

    return 0;
}
