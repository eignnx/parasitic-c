// TODO: figure out a way to set compiler flags like this.
// #define __STDC_WANT_LIB_EXT1__ 1 // Include `*_s` functions.
#include <stdio.h>          // fprintf_s, stderr, fopen_s
#include <stdlib.h>         // exit, perror
#include <string.h>         // strrchr
#include <stdbool.h>        // true, false
#include "parse.parac.h"    // *
#include "cheats.h"         // fn
#include "cstr_buf.parac.h" // struct CStrBuf, init_cstr_buf,
                            // read_to_cstr_buf

global(bool COMP_LOG) = true;

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

struct Compiler
{
    struct List processed_files;       // A `List` of `char*`s.
    struct List files_to_be_processed; // A `List` of `char*`s.
};

// From: https://stackoverflow.com/a/744822/9045161
fn(bool endswith(char *str, char *suffix))
{
    if (!str || !suffix)
        return false;
    int lenstr = strlen(str);
    int lensuffix = strlen(suffix); // TODO: change these back to size_t
    if (lensuffix > lenstr)
        return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

fn(bool is_parac_file(char *filename))
{
    return endswith(filename, ".parac.c") ||
           endswith(filename, ".parac.h");
}

fn(struct List all_included_parac_filenames(struct List *translation_unit))
{
    struct ListNode *node = translation_unit->first;
    struct List parac_filenames = list_init();

    while (node)
    {
        struct Item *item = (struct Item *)node->data;

        if (item->tag == ITEM_POUND_INCLUDE)
        {
            char *filename = item->as.pound_include.filename;
            if (is_parac_file(filename))
            {
                list_push(&parac_filenames, (void *)filename);
                char *new_filename = remove_parac_extension(filename);
                item->as.pound_include.filename = new_filename; // TODO: free the old string
            }
        }

        node = node->next;
    }

    return parac_filenames;
}

// Example: "/asdf/qwer/poiu.txt" --> "/asdf/qwer"
fn(char *dirname(char *path))
{
    char *dir = strdup(path);
    char *last_sep = strrchr(dir, path_separator());
    if (!last_sep)
    {
        // TODO: free dir?
        return "";
    }

    *last_sep = '\0';
    return dir;
}

// Example: "/asdf/qwer/poiu.txt" --> "poiu.txt"
fn(char *basename(char *path))
{
    char *last_sep = strrchr(path, path_separator());
    if (!last_sep)
        return "";
    last_sep++; // Skip the path separator.
    return last_sep;
}

fn(char *path_join(char *p1, char *p2))
{
    char *path = malloc(1000); //TODO: fix this.
    sprintf_s(path, 1000, "%s%c%s", p1, path_separator(), p2);
    return path;
}

fn(struct List compile_file(char *filename, char *dir_root))
{
    FILE *f;

    printf(">> filename = %s, dir_root = %s\n", filename, dir_root);

    comp_log("[OPENING FILE '");
    comp_log(filename);
    comp_log("']\n");

    char *path = path_join(dir_root, filename);
    if (fopen_s(&f, path, "r") != 0)
    {
        perror("Failed to open file");
        fprintf_s(stderr, ">>> '%s'\n", path);
        exit(1);
    }

    struct CStrBuf buf = init_cstr_buf(AVG_LINE_LEN * AVG_FILE_LINES);

    comp_log("[READING FILE TO STRING...]\n");
    read_to_cstr_buf(f, &buf);

    // comp_log("[GOT INPUT]\n```\n");
    // comp_log(buf.buf);
    // comp_log("\n```\n");
    if (COMP_LOG)
    {
        comp_log("[LEXING INPUT...]\n");
        lex_all_input(buf.buf);
    }

    comp_log("[PARSING INPUT...]\n");
    struct Lexer lxr = lexer_init(buf.buf);
    struct List tu = parse_translation_unit(&lxr);
    struct List included_parac_files = all_included_parac_filenames(&tu);

    FILE *out;

    char *new_filename = path_join(dir_root, remove_parac_extension(filename));
    comp_log("[OPENING OUTPUT FILE...]\n");
    if (fopen_s(&out, new_filename, "w") != 0)
    {
        perror("Failed to open file");
        fprintf_s(stderr, ">>> '%s'\n", path);
        exit(1);
    }

    display_translation_unit(out, &tu);

    return included_parac_files;
}

fn(void compile_project(struct Compiler *compiler, char *project_root))
{
    char *base_dir = dirname(project_root);
    char *project_root_basename = basename(project_root);
    list_push(&compiler->files_to_be_processed, (void *)project_root_basename);

    // TODO: Don't recompile already compiled files

    while (compiler->files_to_be_processed.first != NULL)
    {
        char *filename = (char *)list_pop_front(&compiler->files_to_be_processed);
        struct List new_files = compile_file(filename, base_dir);
        list_push(&compiler->processed_files, filename);

        struct ListNode *node = new_files.first;
        while (node)
        {
            char *new_file = (char *)node->data;
            list_push(&compiler->files_to_be_processed, (void *)new_file);

            node = node->next;
        }
    }
}

fn(int main(int argc, char **argv))
{
    struct Compiler compiler;
    compiler.files_to_be_processed = list_init();
    compiler.processed_files = list_init();

    if (argc <= 1 || argc > 2)
        usage();
    else // argc == 2
        compile_project(&compiler, argv[1]);
    // test();

    return 0;
}
