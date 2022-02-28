#ifndef PARASITIC_C_FILE__3EB
#define PARASITIC_C_FILE__3EB

// DIRECTIVES //
#include <stdio.h>
#include <stdlib.h>
#include "lex.c"
#include "parse.c"
#include "cstr_buf.c"
#include "cheats.h"

// STRUCT DECLS //

// ENUM DEFNS //

// STRUCT DEFNS //

// GLOBAL DEFNS //
cstr_arr EXAMPLES = {
"hello_world.parac",
"higher_order_fns.parac",
"forward_decls.parac",
"object.parac",
NULL
};


// FN DECLS //
void test();
void test_lex(char *input);
void test_lexer();
void test_expr(char *input);
void test_parse_exprs();
void test_type(char *input);
void test_typed_ident(char *input);
void test_parse_types();
void test_stmt(char *input);
void test_parse_stmts();
void test_item(char *input);
void test_parse_items();
void test_translation_unit(char *input);
void test_parse_translation_unit();
void test_parser();
void read_lex_and_parse(char *path);
void test_examples();

// FN DEFNS //
void test()
{
test_lexer();
test_parser();
test_examples();
printf("\n\n\n%s\n", "Tests finished!");
}

void test_lex(char *input)
{
printf("--------\n");
lex_all_input(input);
}

void test_lexer()
{
printf("LEXER TESTS---------------------\n");
test_lex("  -123  ");
test_lex("  \"qwerty pgfdjba\"  ");
test_lex("  true  ");
test_lex("  false");
test_lex("  #include <stdio.h>  ");
test_lex("  #include \"stdio.h\"  ");
test_lex("  default // comment\n // blah\n default\n\n default  ");
}

void test_expr(char *input)
{
struct Lexer lxr;
struct Expr *expr;
printf("\n");
lxr = lexer_init("<expr parse test>", input);
if ((expr = parse_expression(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
display_expr(stdout, expr);
else
fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_exprs()
{
printf("\n\n----------------------TEST EXPRS-----------------------------");
test_expr("  -531  ");
test_expr("  '\\n'   ");
test_expr("  \"adfkjlhksjdfh \\n kajdfjahdfs adf a.\"   ");
test_expr("  true ");
test_expr("  false ");
test_expr("  Expr::Add { x: xxx, y: yyy  }   ");
test_expr("  Expr::Add { x: xxx, y: yyy, }   ");
test_expr("  List::Nil { }   ");
test_expr("  my_dog->birth_date[100].year++ ");
test_expr("  my_func(1, 2, 3) ");
test_expr("  my_func(1, 2, 3, ) ");
test_expr("  my_func() ");
test_expr("  !*&*ptr_to_bool  ");
test_expr("  sizeof(*expr)  ");
test_expr("  sizeof(void)  ");
test_expr("  (int) true ");
test_expr("  1 + 2 - 3 + 4 ");
test_expr("  100 > 0 ");
test_expr("  100 >= 0 ");
test_expr("  100 < 0 ");
test_expr("  100 < 0 || 100 > 0");
test_expr("  100 <= 0 ");
test_expr("  100 != 0 ");
test_expr("  100 != 0 && -4 < -3  ");
test_expr("  100 != 0 || -4 < -3  ");
test_expr("  **ptr_ptr = *ptr = 123  ");
}

void test_type(char *input)
{
struct Lexer lxr;
struct Type *type;
printf("\n");
lxr = lexer_init("<type parse test>", input);
if ((type = parse_type(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
display_type(stdout, type);
else
fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_typed_ident(char *input)
{
struct Lexer lxr;
struct Type *type;
printf("\n");
lxr = lexer_init("<typed ident parse test>", input);
if ((type = parse_type(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
display_typed_identifier(stdout, type, "foo");
else
fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_types()
{
printf("\n\n----------------------TEST TYPES-----------------------------");
test_type("  int  ");
test_type("  bool  ");
test_type("  char  ");
test_type("  void  ");
test_type("  cstr_arr  ");
test_type("  FILE*  ");
test_type("  char**  ");
test_type("  struct Expr  ");
test_type("  enum Tag  ");
test_type("  data Expr  ");
test_type("  struct { x: int, y: int, }  ");
test_type("  union { x: int, y: void *, }  ");
test_type("  enum { TOK_PLUS, TOK_MINUS, TOK_INT }  ");
test_type("  enum { SOMETHING, TRAILING_COMMA, }  ");
test_type("  enum { }  ");
test_type("  fn()  ");
test_type("  fn(int, bool)  ");
test_type("  fn(int, bool,)  ");
test_type("  fn(int, bool,) -> struct Expr* ");
test_typed_ident("  fn()  ");
test_typed_ident("  fn(int, bool)  ");
test_typed_ident("  fn(int, bool,) -> struct Expr* ");
test_typed_ident("  fn(int) -> fn(bool) -> fn(FILE) -> char  ");
}

void test_stmt(char *input)
{
struct Lexer lxr;
struct Stmt *stmt;
printf("\n");
lxr = lexer_init("<stmt parse test>", input);
if ((stmt = parse_stmt(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
display_stmt(stdout, stmt);
else
fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_stmts()
{
printf("\n\n----------------------TEST STMTS-----------------------------");
test_stmt("  {}  ");
test_stmt("  { let x: int = 1; launch_missiles(x, 2, 3); }  ");
test_stmt("  if (123 < 456) { return; }  ");
test_stmt("  if (need_else_here) { return 100; } else { return -1; }  ");
test_stmt("  while (i < len) { iterate(); }  ");
test_stmt("  switch (123) {}  ");
test_stmt("  switch (x) { case 1: return 1; case 2: f(); break; default: return 0;}  ");
test_stmt("  break;  ");
test_stmt("  continue;  ");
test_stmt("  return 123;  ");
test_stmt("  return;  ");
test_stmt("  let x: int;  ");
test_stmt("  let f: FILE *;  ");
test_stmt("  let x: int = 123;  ");
test_stmt("  launch_missiles(1, 2, 3);  ");
}

void test_item(char *input)
{
struct Lexer lxr;
struct Item *item;
printf("\n");
lxr = lexer_init("<item parse test>", input);
if ((item = parse_item(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
display_item(stdout, item);
else
fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_items()
{
printf("\n\n----------------------TEST ITEMS-----------------------------");
test_item("  fn foo(x: int, y: int) -> int { return x + y; }  ");
test_item("  fn bar() -> void {}  ");
test_item("  fn three_things() { a(); b(); c(); }  ");
test_item("  global my_const: int = 100;  ");
test_item("  global names: cstr_arr = { \"Jenn\", \"Tim\", \"Scott\", };  ");
test_item("  struct EmptyStruct {}  ");
test_item("  struct Point { x: int, y: int, }  ");
test_item("  enum EmptyEnum {}  ");
test_item("  enum TokTag { TOK_IDENT, TOK_PLUS }  ");
test_item("  #include <stdio.h>  ");
test_item("  #include \"my_file.h\"  ");
}

void test_translation_unit(char *input)
{
struct Lexer lxr;
struct TranslationUnit translation_unit;
printf("\n");
lxr = lexer_init("<translation unit parse test>", input);
translation_unit = parse_translation_unit(&lxr);
display_translation_unit(stdout, &translation_unit);
}

void test_parse_translation_unit()
{
printf("\n\n----------------------TEST TRANSLATION UNITS-----------------");
test_translation_unit("#include <stdio.h>\n fn main() -> int\n{\nreturn 0;\n}");
}

void test_parser()
{
printf("%s\n", "Running parser tests...");
test_parse_exprs();
test_parse_types();
test_parse_stmts();
test_parse_items();
test_parse_translation_unit();
printf("\n\n%s\n", "Parser tests finished!");
}

void read_lex_and_parse(char *path)
{
FILE *f;
if (fopen_s(&f, path, "r") != 0)
{
perror("Failed to open file");
fprintf_s(stderr, ">>> '%s'\n", path);
exit(1);
}
struct CStrBuf buf = init_cstr_buf();
read_to_cstr_buf(f, &buf);
fclose(f);
struct Lexer lxr = lexer_init(path, buf.buf);
parse_translation_unit(&lxr);
}

void test_examples()
{
char * *ex = &EXAMPLES[0];
while (*ex != NULL)
{
char *filename = *ex;
char sep = path_separator();
char *path = format_alloc(".%ctests%c%s", sep, sep, filename);
read_lex_and_parse(path);
printf("\nFile `%s` parses successfully.", path);
free(path);
ex++;
}
}


#endif // PARASITIC_C_FILE__3EB
