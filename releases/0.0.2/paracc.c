#ifndef PARASITIC_C_FILE__0000018DC38CC490
#define PARASITIC_C_FILE__0000018DC38CC490
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parse.c"
#include "lex.c"
#include "cheats.h"
#include "cstr_buf.c"
#include "list.c"
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
fn(void comp_log(char* msg))
{
if (!COMP_LOG)
return;
if (fprintf_s(stderr, "%s", msg) < 0)
{
perror("Failure to comp_log message");
exit(1);
}
}
fndecl(bool is_parac_file(char*));
fndecl(bool endswith(char*, char*));
fn(char* remove_parac_extension(char* filename))
{
char* last_slash = strrchr(filename, path_separator());
if (!last_slash)
last_slash = filename;
char* extension = strchr(last_slash, '.');
if (!extension)
{
fprintf_s(stderr, "`%s` doesn't have a file extension\n", filename);
exit(1);
}
if (!is_parac_file(extension))
{
fprintf_s(stderr, "ERROR:\n| `%s` is not a `.parac(.*)?` file\n", filename);
exit(1);
}
int ext_len = strlen(".parac");
int filename_len = strlen(filename);
return format_alloc("%.*s.c", filename_len - ext_len, filename);
}
struct Compiler {
    struct List processed_files;
    struct List files_to_be_processed;
};
fn(bool endswith(char* str, char* suffix))
{
if (!str || !suffix)
return false;
int lenstr = strlen(str);
int lensuffix = strlen(suffix);
if (lensuffix > lenstr)
return false;
return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
fn(bool is_parac_file(char* filename))
{
return endswith(filename, ".parac.c") || endswith(filename, ".parac.h") || endswith(filename, ".parac");
}
fn(struct List all_included_parac_filenames(struct TranslationUnit* tu))
{
struct ListNode* node = tu->directives.first;
struct List parac_filenames = list_init();
while (node)
{
struct Item* item = (struct Item*) node->data;
if (item->tag == ITEM_POUND_INCLUDE)
{
char* filename = item->as.pound_include.filename;
if (is_parac_file(filename))
{
list_push(&parac_filenames, (void*) filename);
char* new_filename = remove_parac_extension(filename);
item->as.pound_include.filename = new_filename;
}
}
node = node->next;
}
return parac_filenames;
}
fn(char* dirname(char* path))
{
char* dir = strdup(path);
char* last_sep = strrchr(dir, path_separator());
if (!last_sep)
{
return "";
}
*last_sep = '\0';
return dir;
}
fn(char* basename(char* path))
{
char* last_sep = strrchr(path, path_separator());
if (!last_sep)
return "";
last_sep++;
return last_sep;
}
fn(char* path_join(char* p1, char* p2))
{
return format_alloc("%s%c%s", p1, path_separator(), p2);
}
fn(struct List compile_file(char* filename, char* dir_root))
{
FILE* f;
comp_log("[OPENING FILE '");
comp_log(filename);
comp_log("']\n");
char* path = path_join(dir_root, filename);
if (fopen_s(&f, path, "r") != 0)
{
perror("Failed to open file");
fprintf_s(stderr, ">>> '%s'\n", path);
exit(1);
}
struct CStrBuf buf = init_cstr_buf();
comp_log("[READING FILE TO STRING...]\n");
read_to_cstr_buf(f, &buf);
fclose(f);
comp_log("[PARSING INPUT...]\n");
struct Lexer lxr = lexer_init(path, buf.buf);
struct TranslationUnit tu = parse_translation_unit(&lxr);
struct List included_parac_files = all_included_parac_filenames(&tu);
FILE* out;
char* new_filename = path_join(dir_root, remove_parac_extension(filename));
comp_log("[OPENING OUTPUT FILE...]\n");
if (fopen_s(&out, new_filename, "w") != 0)
{
perror("Failed to open file");
fprintf_s(stderr, ">>> '%s'\n", new_filename);
exit(1);
}
if (!display_translation_unit(out, &tu))
{
fprintf_s(stderr, "Error: Failure to output translation unit!\n");
fclose(out);
exit(1);
}
fclose(out);
return included_parac_files;
}
fn(void compile_project(struct Compiler* compiler, char* project_root))
{
char* base_dir = dirname(project_root);
char* project_root_basename = basename(project_root);
list_push(&compiler->files_to_be_processed, (void*) project_root_basename);
while (compiler->files_to_be_processed.first != NULL)
{
char* filename = (char*) list_pop_front(&compiler->files_to_be_processed);
struct List new_files = compile_file(filename, base_dir);
list_push(&compiler->processed_files, filename);
struct ListNode* node = new_files.first;
while (node)
{
char* new_file = (char*) node->data;
if (!list_contains_cstr(&compiler->processed_files, new_file))
{
list_push(&compiler->files_to_be_processed, (void*) new_file);
}
node = node->next;
}
}
}
fn(int main(int argc, char** argv))
{
struct Compiler compiler;
compiler.files_to_be_processed = list_init();
compiler.processed_files = list_init();
if (argc <= 1 || argc > 2)
usage();
else
if (strcmp(argv[1], "--test") == 0)
{
test();
}
else
{
compile_project(&compiler, argv[1]);
}
return 0;
}
#endif // PARASITIC_C_FILE__0000000000000000
