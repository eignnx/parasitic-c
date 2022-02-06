#ifndef PARASITIC_C_FILE__3E9
#define PARASITIC_C_FILE__3E9

// DIRECTIVES //
#include <stdio.h>
#include <stdlib.h>
#include "lex.c"
#include "list.c"
#include "cheats.h"

// STRUCT DECLS //
struct StructMember;
struct Type;
struct ConstantExpr;
struct Item;
struct FnParam;
struct TranslationUnit;
struct Stmt;
struct SwitchArm;
struct Expr;

// ENUM DEFNS //

// STRUCT DEFNS //
struct StructMember {
    struct Type *type;
    char *field;
};

struct Type {
    enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_VOID,
    TYPE_CSTR_ARR,
    TYPE_FILE,
    TYPE_PTR,
    TYPE_FN_PTR,
    TYPE_NAMED_STRUCT_REF,
    TYPE_NAMED_ENUM_REF,
    TYPE_ANON_STRUCT,
    TYPE_ANON_UNION,
    TYPE_ANON_ENUM,
} tag;
    union {
    struct {
    struct Type *child;
} ptr;
    struct {
    struct List param_types;
    struct Type *ret_type;
} fn_ptr;
    struct {
    char *name;
} named;
    struct {
    struct List struct_members;
} anon_struct_or_union;
    struct {
    struct List enum_members;
} anon_enum;
} as;
};

struct ConstantExpr {
    enum {
    CONST_INT,
    CONST_BOOL,
    CONST_CSTR,
    CONST_ARRAY,
    CONST_IDENT,
} tag;
    union {
    int const_int;
    bool const_bool;
    char *const_cstr;
    struct List const_array;
    char *const_ident;
} as;
};

struct Item {
    enum {
    ITEM_FN_DEF,
    ITEM_GLOBAL_DECL,
    ITEM_NAMED_STRUCT_DECL,
    ITEM_NAMED_ENUM_DECL,
    ITEM_POUND_INCLUDE,
} tag;
    union {
    struct {
    char *name;
    struct List params;
    struct Type *ret_type;
    struct List body_stmts;
} fn_def;
    struct {
    struct Type *type;
    char *name;
    struct ConstantExpr *initializer;
} global_decl;
    struct {
    char *name;
    struct List members;
} named_struct_decl;
    struct {
    char *name;
    struct List members;
} named_enum_decl;
    struct {
    char *filename;
    enum {
    POUND_INCLUDE_ANGLE_BRACKETS,
    POUND_INCLUDE_DOUBLE_QUOTES,
} kind;
} pound_include;
} as;
};

struct FnParam {
    struct Type *type;
    char *name;
};

struct TranslationUnit {
    int id;
    struct List directives;
    struct List enum_defns;
    struct List struct_defns;
    struct List global_defns;
    struct List fn_defns;
};

struct Stmt {
    enum {
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_SWITCH,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_RETURN,
    STMT_VAR_DECL,
    STMT_EXPRESSION,
} tag;
    union {
    struct {
    struct Expr *cond;
    struct Stmt *consequent;
    struct Stmt *alternative;
} if_stmt;
    struct {
    struct Expr *cond;
    struct Stmt *body;
} while_stmt;
    struct {
    struct Expr *scrutinee;
    struct List switch_arms;
} switch_stmt;
    struct {
    struct List stmts;
} block;
    struct {
    struct Expr *expr;
} ret;
    struct {
    struct Type *type;
    char *name;
    struct Expr *initializer;
} var_decl;
    struct {
    struct Expr *expr;
} expression;
} as;
};

struct SwitchArm {
    struct Expr *test;
    struct List stmts;
};

struct Expr {
    enum {
    EXPR_LITERAL_INT,
    EXPR_LITERAL_CHAR,
    EXPR_LITERAL_STRING,
    EXPR_LITERAL_BOOL,
    EXPR_IDENT,
    EXPR_PARENTHESIZED,
    EXPR_ARR_INDEX,
    EXPR_CALL,
    EXPR_DOT_FIELD_ACCESS,
    EXPR_ARROW_FIELD_ACCESS,
    EXPR_POSTFIX_PLUS_PLUS,
    EXPR_REFERENCE,
    EXPR_DEREFERENCE,
    EXPR_NEGATION,
    EXPR_SIZEOF_EXPR,
    EXPR_SIZEOF_TYPE,
    EXPR_CAST,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_GT,
    EXPR_GTE,
    EXPR_LT,
    EXPR_LTE,
    EXPR_EQUAL,
    EXPR_NOT_EQUAL,
    EXPR_AND,
    EXPR_OR,
    EXPR_ASSIGN,
} tag;
    union {
    struct {
    int value;
} literal_int;
    struct {
    char *value;
} literal_char;
    struct {
    char *value;
} literal_string;
    struct {
    bool value;
} literal_bool;
    struct {
    char *value;
} ident;
    struct {
    struct Expr *child;
} parenthesized;
    struct {
    struct Expr *arr;
    struct Expr *index;
} arr_index;
    struct {
    struct Expr *callable;
    struct List args;
} call;
    struct {
    struct Expr *object;
    char *field;
} field_access;
    struct {
    struct Expr *operand;
} postfix_plus_plus;
    struct {
    struct Expr *operand;
} unary_op;
    struct {
    struct Expr *operand;
} sizeof_expr;
    struct {
    struct Type *type;
} sizeof_type;
    struct {
    struct Type *type;
    struct Expr *expr;
} cast;
    struct {
    struct Expr *x;
    struct Expr *y;
} binary_op;
} as;
};


// GLOBAL DEFNS //
int NEXT_TU_ID = 1000;


// FN DECLS //
struct List parse_struct_member_list(struct Lexer *lxr);
bool display_fn_pointer_type(FILE *out, struct Type *type, char *binder);
bool display_fn_signature(FILE *out, char *name, struct List *fn_params, struct Type *ret_type);
bool display_typed_identifier(FILE *out, struct Type *type, char *ident);
bool display_type(FILE *out, struct Type *type);
bool display_struct_members(FILE *out, struct List *list);
bool display_enum_members(FILE *out, struct List *list);
struct Type * parse_type(struct Lexer *lxr);
struct Type * parse_direct_type(struct Lexer *lxr);
bool display_const_array(FILE *out, struct List *list);
bool display_constant_expr(FILE *out, struct ConstantExpr *expr);
struct List parse_initializer_list(struct Lexer *lxr);
struct ConstantExpr * parse_constant_expr(struct Lexer *lxr);
bool display_param_types(FILE *out, struct List *param_types);
bool display_fn_params(FILE *out, struct List *params);
bool display_item(FILE *out, struct Item *item);
struct List parse_param_types(struct Lexer *lxr);
struct List parse_parameters(struct Lexer *lxr);
struct TranslationUnit translation_unit_init();
void translation_unit_push_item(struct TranslationUnit *tu, struct Item *item);
struct TranslationUnit parse_translation_unit(struct Lexer *lxr);
bool display_item_list(FILE *out, struct List *items);
bool display_tu_struct_decls(FILE *out, struct TranslationUnit *tu);
bool display_tu_fn_decls(FILE *out, struct TranslationUnit *tu);
bool display_translation_unit(FILE *out, struct TranslationUnit *tu);
struct Item * parse_item(struct Lexer *lxr);
struct List parse_enumerator_list(struct Lexer *lxr);
bool display_switch_arms(FILE *out, struct List *arms);
bool display_stmt(FILE *out, struct Stmt *stmt);
bool display_stmt_list(FILE *out, struct List *list);
struct Stmt * parse_stmt(struct Lexer *lxr);
struct List parse_stmt_list(struct Lexer *lxr);
struct Stmt * parse_var_decl(struct Lexer *lxr);
struct List parse_switch_arms(struct Lexer *lxr);
bool display_bin_op(FILE *out, char *op, struct Expr *x, struct Expr *y);
bool display_expr(FILE *out, struct Expr *expr);
bool display_arglist(FILE *out, struct List *list);
struct Expr * parse_primary_expression(struct Lexer *lxr);
struct Expr * parse_postfix_expression(struct Lexer *lxr);
struct List parse_argument_expression_list(struct Lexer *lxr);
struct Expr * parse_unary_expression(struct Lexer *lxr);
struct Expr * parse_cast_expression(struct Lexer *lxr);
struct Expr * parse_multiplicative_expression(struct Lexer *lxr);
struct Expr * parse_additive_expression(struct Lexer *lxr);
struct Expr * parse_shift_expression(struct Lexer *lxr);
struct Expr * parse_relational_expression(struct Lexer *lxr);
struct Expr * parse_equality_expression(struct Lexer *lxr);
struct Expr * parse_logical_and_expression(struct Lexer *lxr);
struct Expr * parse_logical_or_expression(struct Lexer *lxr);
struct Expr * parse_conditional_expression(struct Lexer *lxr);
struct Expr * parse_assignment_expression(struct Lexer *lxr);
struct Expr * parse_expression(struct Lexer *lxr);
struct Expr * parse_constant(struct Lexer *lxr);
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

// FN DEFNS //
struct List parse_struct_member_list(struct Lexer *lxr)
{
struct List list = list_init();
while (lexer_accept(lxr, TOK_IDENT))
{
char *field = lxr->token;
lexer_expect(lxr, TOK_COLON);
struct Type *type;
if (!(type = parse_type(lxr)))
todo;
lexer_expect(lxr, TOK_COMMA);
struct StructMember *sm = malloc(sizeof (*sm));
sm->field = field;
sm->type = type;
list_push(&list, (void *) sm);
}
return list;
}

bool display_fn_pointer_type(FILE *out, struct Type *type, char *binder)
{
if (type == NULL || type->tag != TYPE_FN_PTR)
{
if (type == NULL)
return fprintf_s(out, "void ") >= 0;
else
return display_type(out, type) && fprintf_s(out, " ") >= 0;
}
else
if (type->tag == TYPE_FN_PTR && binder)
{
if (!display_fn_pointer_type(out, type->as.fn_ptr.ret_type, NULL))
return false;
if (!(fprintf_s(out, "(*%s)(", binder) >= 0 && display_param_types(out, &type->as.fn_ptr.param_types) && fprintf_s(out, ")") >= 0))
return false;
struct Type *t = type->as.fn_ptr.ret_type;
while (t != NULL && t->tag == TYPE_FN_PTR)
{
if (!(fprintf_s(out, ")(") >= 0 && display_param_types(out, &t->as.fn_ptr.param_types) && fprintf_s(out, ")") >= 0))
return false;
t = t->as.fn_ptr.ret_type;
}
return true;
}
else
{
return display_fn_pointer_type(out, type->as.fn_ptr.ret_type, NULL) && fprintf_s(out, "(*") >= 0;
}
}

bool display_fn_signature(FILE *out, char *name, struct List *fn_params, struct Type *ret_type)
{
if (!display_fn_pointer_type(out, ret_type, NULL))
return false;
if (!(fprintf_s(out, "%s(", name) >= 0 && display_fn_params(out, fn_params) && fprintf_s(out, ")") >= 0))
return false;
struct Type *t = ret_type;
while (t != NULL && t->tag == TYPE_FN_PTR)
{
if (!(fprintf_s(out, ")(") >= 0 && display_param_types(out, &t->as.fn_ptr.param_types) && fprintf_s(out, ")") >= 0))
return false;
t = t->as.fn_ptr.ret_type;
}
return true;
}

bool display_typed_identifier(FILE *out, struct Type *type, char *ident)
{
char *space = " ";
if (ident == NULL || ident[0] == '\0')
space = "";
switch (type->tag)
{
case TYPE_INT:
{
return fprintf_s(out, "int%s%s", space, ident) >= 0;
}
case TYPE_BOOL:
{
return fprintf_s(out, "bool%s%s", space, ident) >= 0;
}
case TYPE_CHAR:
{
return fprintf_s(out, "char%s%s", space, ident) >= 0;
}
case TYPE_VOID:
{
return fprintf_s(out, "void%s%s", space, ident) >= 0;
}
case TYPE_CSTR_ARR:
{
return fprintf_s(out, "cstr_arr%s%s", space, ident) >= 0;
}
case TYPE_FILE:
{
return fprintf_s(out, "FILE%s%s", space, ident) >= 0;
}
case TYPE_PTR:
{
return display_typed_identifier(out, type->as.ptr.child, "") && fprintf_s(out, " *%s", ident) >= 0;
}
case TYPE_FN_PTR:
{
return display_fn_pointer_type(out, type, ident);
}
case TYPE_NAMED_STRUCT_REF:
{
return fprintf_s(out, "struct %s%s%s", type->as.named.name, space, ident) >= 0;
}
case TYPE_NAMED_ENUM_REF:
{
return fprintf_s(out, "enum %s%s%s", type->as.named.name, space, ident) >= 0;
}
case TYPE_ANON_STRUCT:
{
return fprintf_s(out, "struct {\n") >= 0 && display_struct_members(out, &type->as.anon_struct_or_union.struct_members) && fprintf_s(out, "}%s%s", space, ident) >= 0;
}
case TYPE_ANON_UNION:
{
return fprintf_s(out, "union {\n") >= 0 && display_struct_members(out, &type->as.anon_struct_or_union.struct_members) && fprintf_s(out, "}%s%s", space, ident) >= 0;
}
case TYPE_ANON_ENUM:
{
return fprintf_s(out, "enum {\n") >= 0 && display_enum_members(out, &type->as.anon_enum.enum_members) && fprintf_s(out, "}%s%s", space, ident) >= 0;
}
}
}

bool display_type(FILE *out, struct Type *type)
{
return display_typed_identifier(out, type, "");
}

bool display_struct_members(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
struct StructMember *sm = (struct StructMember *) node->data;
bool result = fprintf_s(out, "    ") >= 0 && display_typed_identifier(out, sm->type, sm->field) && fprintf_s(out, ";\n", sm->field) >= 0;
if (!(result))
return false;
node = node->next;
}
return true;
}

bool display_enum_members(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
char *enum_constant_name = (char *) node->data;
bool result = fprintf_s(out, "    %s,\n", enum_constant_name) >= 0;
if (!result)
return false;
node = node->next;
}
return true;
}

struct Type * parse_type(struct Lexer *lxr)
{
struct Type *type;
if (!(type = parse_direct_type(lxr)))
return NULL;
while (lexer_accept(lxr, TOK_STAR))
{
struct Type *new_type = malloc(sizeof (*new_type));
new_type->tag = TYPE_PTR;
new_type->as.ptr.child = type;
type = new_type;
}
return type;
}

struct Type * parse_direct_type(struct Lexer *lxr)
{
struct Type *type;
if (lexer_accept(lxr, TOK_VOID) || lexer_accept(lxr, TOK_CHAR) || lexer_accept(lxr, TOK_INT) || lexer_accept(lxr, TOK_BOOL) || lexer_accept(lxr, TOK_CSTR_ARR) || lexer_accept(lxr, TOK_FILE))
{
type = malloc(sizeof (*type));
switch (lxr->tok_tag)
{
case TOK_VOID:
{
type->tag = TYPE_VOID;
break;
}
case TOK_CHAR:
{
type->tag = TYPE_CHAR;
break;
}
case TOK_INT:
{
type->tag = TYPE_INT;
break;
}
case TOK_BOOL:
{
type->tag = TYPE_BOOL;
break;
}
case TOK_CSTR_ARR:
{
type->tag = TYPE_CSTR_ARR;
break;
}
case TOK_FILE:
{
type->tag = TYPE_FILE;
break;
}
default:
{
fprintf_s(stderr, "Unknown Type token %s\n", tok_tag_names[lxr->tok_tag]);
exit(1);
}
}
return type;
}
if (lexer_accept(lxr, TOK_STRUCT))
{
if (lexer_accept(lxr, TOK_IDENT))
{
type = malloc(sizeof (*type));
type->tag = TYPE_NAMED_STRUCT_REF;
type->as.named.name = lxr->token;
return type;
}
if (lexer_expect(lxr, TOK_OPEN_BRACE))
{
struct List members = parse_struct_member_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
type = malloc(sizeof (*type));
type->tag = TYPE_ANON_STRUCT;
type->as.anon_struct_or_union.struct_members = members;
return type;
}
return NULL;
}
if (lexer_accept(lxr, TOK_ENUM))
{
if (lexer_accept(lxr, TOK_IDENT))
{
type = malloc(sizeof (*type));
type->tag = TYPE_NAMED_ENUM_REF;
type->as.named.name = lxr->token;
return type;
}
if (lexer_expect(lxr, TOK_OPEN_BRACE))
{
struct List members = parse_enumerator_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
type = malloc(sizeof (*type));
type->tag = TYPE_ANON_ENUM;
type->as.anon_enum.enum_members = members;
return type;
}
return NULL;
}
if (lexer_accept(lxr, TOK_UNION))
{
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List members = parse_struct_member_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
type = malloc(sizeof (*type));
type->tag = TYPE_ANON_UNION;
type->as.anon_struct_or_union.struct_members = members;
return type;
}
if (lexer_accept(lxr, TOK_FN))
{
struct Type *ret_type = NULL;
lexer_expect(lxr, TOK_OPEN_PAREN);
struct List param_types = parse_param_types(lxr);
lexer_expect(lxr, TOK_CLOSE_PAREN);
if (lexer_accept(lxr, TOK_ARROW))
{
if (!(ret_type = parse_type(lxr)))
{
fprintf_s(stderr, "PARSE ERROR at %s:%d\n", lxr->filename, lxr->line);
fprintf_s(stderr, "| Could not parse a return type after `->`.\n");
exit(1);
}
}
type = malloc(sizeof (*type));
type->tag = TYPE_FN_PTR;
type->as.fn_ptr.param_types = param_types;
type->as.fn_ptr.ret_type = ret_type;
return type;
}
return NULL;
}

bool display_const_array(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
struct ConstantExpr *expr = (struct ConstantExpr *) node->data;
if (node->next)
{
bool result = display_constant_expr(out, expr) && fprintf_s(out, ",\n") >= 0;
if (!result)
return false;
}
else
{
bool result = display_constant_expr(out, expr) && fprintf_s(out, "\n") >= 0;
if (!result)
return false;
}
node = node->next;
}
return true;
}

bool display_constant_expr(FILE *out, struct ConstantExpr *expr)
{
switch (expr->tag)
{
case CONST_INT:
{
return fprintf_s(out, "%d", expr->as.const_int) >= 0;
}
case CONST_BOOL:
{
if (expr->as.const_bool)
return fprintf_s(out, "true") >= 0;
else
return fprintf_s(out, "false") >= 0;
}
case CONST_CSTR:
{
return fprintf_s(out, "\"%s\"", expr->as.const_cstr) >= 0;
}
case CONST_ARRAY:
{
return fprintf_s(out, "{\n") >= 0 && display_const_array(out, &expr->as.const_array) && fprintf_s(out, "}") >= 0;
}
case CONST_IDENT:
{
return fprintf_s(out, "%s", expr->as.const_ident) >= 0;
}
}
}

struct List parse_initializer_list(struct Lexer *lxr)
{
struct List initializers = list_init();
struct ConstantExpr *expr;
while ((expr = parse_constant_expr(lxr)))
{
if (!lexer_accept(lxr, TOK_COMMA))
break;
list_push(&initializers, expr);
}
return initializers;
}

struct ConstantExpr * parse_constant_expr(struct Lexer *lxr)
{
struct ConstantExpr *expr;
if (lexer_accept(lxr, TOK_LITERAL_INT))
{
expr = malloc(sizeof (*expr));
expr->tag = CONST_INT;
expr->as.const_int = atoi(lxr->token);
return expr;
}
if (lexer_accept(lxr, TOK_TRUE) || lexer_accept(lxr, TOK_FALSE))
{
bool value = lxr->tok_tag == TOK_TRUE;
expr = malloc(sizeof (*expr));
expr->tag = CONST_BOOL;
expr->as.const_bool = value;
return expr;
}
if (lexer_accept(lxr, TOK_LITERAL_STRING))
{
expr = malloc(sizeof (*expr));
expr->tag = CONST_CSTR;
expr->as.const_cstr = lxr->token;
return expr;
}
if (lexer_accept(lxr, TOK_OPEN_BRACE))
{
struct List initializers = parse_initializer_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
expr = malloc(sizeof (*expr));
expr->tag = CONST_ARRAY;
expr->as.const_array = initializers;
return expr;
}
if (lexer_accept(lxr, TOK_IDENT))
{
expr = malloc(sizeof (*expr));
expr->tag = CONST_IDENT;
expr->as.const_ident = lxr->token;
return expr;
}
return NULL;
}

bool display_param_types(FILE *out, struct List *param_types)
{
struct ListNode *node = param_types->first;
while (node)
{
struct Type *type = (struct Type *) node->data;
if (node->next)
{
bool result = display_type(out, type) && fprintf_s(out, ", ");
if (!result)
return false;
}
else
{
bool result = display_type(out, type);
if (!result)
return false;
}
node = node->next;
}
return true;
}

bool display_fn_params(FILE *out, struct List *params)
{
struct ListNode *node = params->first;
while (node)
{
struct FnParam *param = (struct FnParam *) node->data;
if (node->next)
{
bool result = display_typed_identifier(out, param->type, param->name) && fprintf_s(out, ", ") >= 0;
if (!result)
return false;
}
else
{
bool result = display_typed_identifier(out, param->type, param->name);
if (!result)
return false;
}
node = node->next;
}
return true;
}

bool display_item(FILE *out, struct Item *item)
{
switch (item->tag)
{
case ITEM_FN_DEF:
{
return display_fn_signature(out, item->as.fn_def.name, &item->as.fn_def.params, item->as.fn_def.ret_type) && fprintf_s(out, "\n{\n") >= 0 && display_stmt_list(out, &item->as.fn_def.body_stmts) && fprintf_s(out, "}\n\n") >= 0;
}
case ITEM_GLOBAL_DECL:
{
return display_type(out, item->as.global_decl.type) && fprintf_s(out, " %s = ", item->as.global_decl.name) >= 0 && display_constant_expr(out, item->as.global_decl.initializer) && fprintf_s(out, ";\n\n") >= 0;
}
case ITEM_NAMED_STRUCT_DECL:
{
return fprintf_s(out, "struct %s {\n", item->as.named_struct_decl.name) >= 0 && display_struct_members(out, &item->as.named_struct_decl.members) && fprintf_s(out, "};\n\n") >= 0;
}
case ITEM_NAMED_ENUM_DECL:
{
return fprintf_s(out, "enum %s {\n", item->as.named_enum_decl.name) >= 0 && display_enum_members(out, &item->as.named_enum_decl.members) && fprintf_s(out, "};\n\n") >= 0;
}
case ITEM_POUND_INCLUDE:
{
if (item->as.pound_include.kind == POUND_INCLUDE_ANGLE_BRACKETS)
{
return fprintf_s(out, "#include <%s>\n", item->as.pound_include.filename) >= 0;
}
else
{
return fprintf_s(out, "#include \"%s\"\n", item->as.pound_include.filename) >= 0;
}
}
}
}

struct List parse_param_types(struct Lexer *lxr)
{
struct List types = list_init();
struct Type *type;
while ((type = parse_type(lxr)))
{
list_push(&types, type);
if (!lexer_accept(lxr, TOK_COMMA))
break;
}
return types;
}

struct List parse_parameters(struct Lexer *lxr)
{
struct List params = list_init();
while (lexer_accept(lxr, TOK_IDENT))
{
char *name = lxr->token;
lexer_expect(lxr, TOK_COLON);
struct Type *type;
if (!(type = parse_type(lxr)))
todo;
struct FnParam *param = malloc(sizeof (*param));
param->name = name;
param->type = type;
list_push(&params, (void *) param);
if (!lexer_accept(lxr, TOK_COMMA))
break;
}
return params;
}

struct TranslationUnit translation_unit_init()
{
struct TranslationUnit tu;
tu.id = NEXT_TU_ID;
NEXT_TU_ID = NEXT_TU_ID + 1;
tu.directives = list_init();
tu.enum_defns = list_init();
tu.struct_defns = list_init();
tu.global_defns = list_init();
tu.fn_defns = list_init();
return tu;
}

void translation_unit_push_item(struct TranslationUnit *tu, struct Item *item)
{
switch (item->tag)
{
case ITEM_POUND_INCLUDE:
{
list_push(&tu->directives, item);
return;
}
case ITEM_NAMED_ENUM_DECL:
{
list_push(&tu->enum_defns, item);
return;
}
case ITEM_NAMED_STRUCT_DECL:
{
list_push(&tu->struct_defns, item);
return;
}
case ITEM_GLOBAL_DECL:
{
list_push(&tu->global_defns, item);
return;
}
case ITEM_FN_DEF:
{
list_push(&tu->fn_defns, item);
return;
}
}
}

struct TranslationUnit parse_translation_unit(struct Lexer *lxr)
{
struct TranslationUnit tu = translation_unit_init();
struct Item *item;
while ((item = parse_item(lxr)))
translation_unit_push_item(&tu, item);
lexer_expect(lxr, TOK_END_OF_INPUT);
return tu;
}

bool display_item_list(FILE *out, struct List *items)
{
struct ListNode *node = items->first;
while (node)
{
struct Item *item = (struct Item *) node->data;
if (!display_item(out, item))
return false;
node = node->next;
}
return true;
}

bool display_tu_struct_decls(FILE *out, struct TranslationUnit *tu)
{
struct ListNode *node = tu->struct_defns.first;
while (node)
{
struct Item *item = (struct Item *) node->data;
if (item->tag != ITEM_NAMED_STRUCT_DECL)
todo;
if (!(fprintf_s(out, "struct %s;\n", item->as.named_struct_decl.name) >= 0))
return false;
node = node->next;
}
return true;
}

bool display_tu_fn_decls(FILE *out, struct TranslationUnit *tu)
{
struct ListNode *node = tu->fn_defns.first;
while (node)
{
struct Item *item = (struct Item *) node->data;
if (item->tag != ITEM_FN_DEF)
todo;
if (!(display_fn_signature(out, item->as.fn_def.name, &item->as.fn_def.params, item->as.fn_def.ret_type) && fprintf_s(out, ";\n") >= 0))
return false;
node = node->next;
}
return true;
}

bool display_translation_unit(FILE *out, struct TranslationUnit *tu)
{
return fprintf_s(out, "#ifndef PARASITIC_C_FILE__%X\n", tu->id) >= 0 && fprintf_s(out, "#define PARASITIC_C_FILE__%X\n", tu->id) >= 0 && fprintf_s(out, "\n// DIRECTIVES //\n") >= 0 && display_item_list(out, &tu->directives) && fprintf_s(out, "\n// STRUCT DECLS //\n") >= 0 && display_tu_struct_decls(out, tu) && fprintf_s(out, "\n// ENUM DEFNS //\n") >= 0 && display_item_list(out, &tu->enum_defns) && fprintf_s(out, "\n// STRUCT DEFNS //\n") >= 0 && display_item_list(out, &tu->struct_defns) && fprintf_s(out, "\n// GLOBAL DEFNS //\n") >= 0 && display_item_list(out, &tu->global_defns) && fprintf_s(out, "\n// FN DECLS //\n") >= 0 && display_tu_fn_decls(out, tu) && fprintf_s(out, "\n// FN DEFNS //\n") >= 0 && display_item_list(out, &tu->fn_defns) && fprintf_s(out, "\n#endif // PARASITIC_C_FILE__%X\n", tu->id);
}

struct Item * parse_item(struct Lexer *lxr)
{
struct Item *item;
if (lexer_accept(lxr, TOK_FN))
{
lexer_expect(lxr, TOK_IDENT);
char *name = lxr->token;
lexer_expect(lxr, TOK_OPEN_PAREN);
struct List param_types = parse_parameters(lxr);
lexer_expect(lxr, TOK_CLOSE_PAREN);
struct Type *ret_type;
if (lexer_accept(lxr, TOK_ARROW))
{
ret_type = parse_type(lxr);
}
else
{
ret_type = malloc(sizeof (*ret_type));
ret_type->tag = TYPE_VOID;
}
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List body_stmts = parse_stmt_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
item = malloc(sizeof (*item));
item->tag = ITEM_FN_DEF;
item->as.fn_def.name = name;
item->as.fn_def.params = param_types;
item->as.fn_def.ret_type = ret_type;
item->as.fn_def.body_stmts = body_stmts;
return item;
}
if (lexer_accept(lxr, TOK_GLOBAL))
{
lexer_expect(lxr, TOK_IDENT);
char *name = lxr->token;
lexer_expect(lxr, TOK_COLON);
struct Type *type;
if (!(type = parse_type(lxr)))
todo;
lexer_expect(lxr, TOK_EQUAL);
struct ConstantExpr *initializer;
if (!(initializer = parse_constant_expr(lxr)))
todo;
lexer_expect(lxr, TOK_SEMI);
item = malloc(sizeof (*item));
item->tag = ITEM_GLOBAL_DECL;
item->as.global_decl.type = type;
item->as.global_decl.name = name;
item->as.global_decl.initializer = initializer;
return item;
}
if (lexer_accept(lxr, TOK_STRUCT))
{
lexer_expect(lxr, TOK_IDENT);
char *name = lxr->token;
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List members = parse_struct_member_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
if (lexer_accept(lxr, TOK_SEMI))
{
fprintf_s(stderr, "[WARNING] %s:%d:\n", lxr->filename, lxr->line);
fprintf_s(stderr, "    A semicolon is no longer needed after a struct definition.\n");
}
struct Item *item = malloc(sizeof (*item));
item->tag = ITEM_NAMED_STRUCT_DECL;
item->as.named_struct_decl.name = name;
item->as.named_struct_decl.members = members;
return item;
}
if (lexer_accept(lxr, TOK_ENUM))
{
lexer_expect(lxr, TOK_IDENT);
char *name = lxr->token;
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List members = parse_enumerator_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
if (lexer_accept(lxr, TOK_SEMI))
{
fprintf_s(stderr, "[WARNING] %s:%d:\n", lxr->filename, lxr->line);
fprintf_s(stderr, "    A semicolon is no longer needed after an enum definition.\n");
}
struct Item *item = malloc(sizeof (*item));
item->tag = ITEM_NAMED_ENUM_DECL;
item->as.named_enum_decl.name = name;
item->as.named_enum_decl.members = members;
return item;
}
if (lexer_accept(lxr, TOK_POUND))
{
lexer_expect(lxr, TOK_INCLUDE);
int kind;
if (lexer_accept(lxr, TOK_ANGLE_BRACK_FILENAME))
kind = (int) POUND_INCLUDE_ANGLE_BRACKETS;
else
if (lexer_accept(lxr, TOK_LITERAL_STRING))
kind = (int) POUND_INCLUDE_DOUBLE_QUOTES;
else
return NULL;
item = malloc(sizeof (*item));
item->tag = ITEM_POUND_INCLUDE;
item->as.pound_include.filename = lxr->token;
item->as.pound_include.kind = kind;
return item;
}
return NULL;
}

struct List parse_enumerator_list(struct Lexer *lxr)
{
struct List members = list_init();
char *member;
while (lexer_accept(lxr, TOK_IDENT))
{
member = lxr->token;
list_push(&members, (void *) member);
if (!lexer_accept(lxr, TOK_COMMA))
return members;
}
return members;
}

bool display_switch_arms(FILE *out, struct List *arms)
{
struct ListNode *node = arms->first;
while (node)
{
struct SwitchArm *arm = (struct SwitchArm *) node->data;
if (arm->test == NULL)
{
bool result = fprintf_s(out, "default:\n") >= 0 && fprintf_s(out, "{\n") >= 0 && display_stmt_list(out, &arm->stmts) && fprintf_s(out, "}\n") >= 0;
if (!result)
return false;
}
else
{
bool result = fprintf_s(out, "case ") >= 0 && display_expr(out, arm->test) && fprintf_s(out, ":\n") >= 0 && fprintf_s(out, "{\n") >= 0 && display_stmt_list(out, &arm->stmts) && fprintf_s(out, "}\n") >= 0;
if (!result)
return false;
}
node = node->next;
}
return true;
}

bool display_stmt(FILE *out, struct Stmt *stmt)
{
switch (stmt->tag)
{
case STMT_BLOCK:
{
return fprintf_s(out, "{\n") >= 0 && display_stmt_list(out, &stmt->as.block.stmts) && fprintf_s(out, "}\n") >= 0;
}
case STMT_IF:
{
if (stmt->as.if_stmt.alternative)
{
return fprintf_s(out, "if (") >= 0 && display_expr(out, stmt->as.if_stmt.cond) && fprintf_s(out, ")\n") >= 0 && display_stmt(out, stmt->as.if_stmt.consequent) && fprintf_s(out, "else\n") >= 0 && display_stmt(out, stmt->as.if_stmt.alternative);
}
else
{
return fprintf_s(out, "if (") >= 0 && display_expr(out, stmt->as.if_stmt.cond) && fprintf_s(out, ")\n") >= 0 && display_stmt(out, stmt->as.if_stmt.consequent);
}
}
case STMT_WHILE:
{
return fprintf_s(out, "while (") >= 0 && display_expr(out, stmt->as.while_stmt.cond) && fprintf_s(out, ")\n") >= 0 && display_stmt(out, stmt->as.while_stmt.body);
}
case STMT_SWITCH:
{
return fprintf_s(out, "switch (") >= 0 && display_expr(out, stmt->as.switch_stmt.scrutinee) && fprintf_s(out, ")\n{\n") >= 0 && display_switch_arms(out, &stmt->as.switch_stmt.switch_arms) && fprintf_s(out, "}\n") >= 0;
}
case STMT_BREAK:
{
return fprintf_s(out, "break;\n") >= 0;
}
case STMT_CONTINUE:
{
return fprintf_s(out, "continue;\n") >= 0;
}
case STMT_RETURN:
{
if (stmt->as.ret.expr)
{
return fprintf_s(out, "return ") >= 0 && display_expr(out, stmt->as.ret.expr) && fprintf_s(out, ";\n") >= 0;
}
else
{
return fprintf_s(out, "return;\n") >= 0;
}
}
case STMT_VAR_DECL:
{
if (stmt->as.var_decl.initializer)
{
return display_typed_identifier(out, stmt->as.var_decl.type, stmt->as.var_decl.name) && fprintf_s(out, " = ") >= 0 && display_expr(out, stmt->as.var_decl.initializer) && fprintf_s(out, ";\n") >= 0;
}
else
{
return display_typed_identifier(out, stmt->as.var_decl.type, stmt->as.var_decl.name) && fprintf_s(out, ";\n", stmt->as.var_decl.name) >= 0;
}
}
case STMT_EXPRESSION:
{
return display_expr(out, stmt->as.expression.expr) && fprintf_s(out, ";\n") >= 0;
}
}
}

bool display_stmt_list(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
struct Stmt *stmt = (struct Stmt *) node->data;
bool result = display_stmt(out, stmt);
if (!result)
return false;
node = node->next;
}
return true;
}

struct Stmt * parse_stmt(struct Lexer *lxr)
{
struct Stmt *stmt;
if (lexer_accept(lxr, TOK_OPEN_BRACE))
{
struct List stmts = parse_stmt_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_BLOCK;
stmt->as.block.stmts = stmts;
return stmt;
}
if (lexer_accept(lxr, TOK_IF))
{
lexer_expect(lxr, TOK_OPEN_PAREN);
struct Expr *cond = parse_expression(lxr);
lexer_expect(lxr, TOK_CLOSE_PAREN);
struct Stmt *consequent = parse_stmt(lxr);
struct Stmt *alternative = NULL;
if (lexer_accept(lxr, TOK_ELSE))
if (!(alternative = parse_stmt(lxr)))
return NULL;
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_IF;
stmt->as.if_stmt.cond = cond;
stmt->as.if_stmt.consequent = consequent;
stmt->as.if_stmt.alternative = alternative;
return stmt;
}
if (lexer_accept(lxr, TOK_WHILE))
{
lexer_expect(lxr, TOK_OPEN_PAREN);
struct Expr *cond = parse_expression(lxr);
lexer_expect(lxr, TOK_CLOSE_PAREN);
struct Stmt *body = parse_stmt(lxr);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_WHILE;
stmt->as.while_stmt.cond = cond;
stmt->as.while_stmt.body = body;
return stmt;
}
if (lexer_accept(lxr, TOK_SWITCH))
{
lexer_expect(lxr, TOK_OPEN_PAREN);
struct Expr *scrutinee = parse_expression(lxr);
lexer_expect(lxr, TOK_CLOSE_PAREN);
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List arms = parse_switch_arms(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_SWITCH;
stmt->as.switch_stmt.scrutinee = scrutinee;
stmt->as.switch_stmt.switch_arms = arms;
return stmt;
}
if (lexer_accept(lxr, TOK_BREAK))
{
lexer_expect(lxr, TOK_SEMI);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_BREAK;
return stmt;
}
if (lexer_accept(lxr, TOK_CONTINUE))
{
lexer_expect(lxr, TOK_SEMI);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_CONTINUE;
return stmt;
}
if (lexer_accept(lxr, TOK_RETURN))
{
struct Expr *expr = NULL;
expr = parse_expression(lxr);
lexer_expect(lxr, TOK_SEMI);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_RETURN;
stmt->as.ret.expr = expr;
return stmt;
}
if ((stmt = parse_var_decl(lxr)))
return stmt;
struct Expr *expr;
if ((expr = parse_expression(lxr)))
{
lexer_expect(lxr, TOK_SEMI);
stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_EXPRESSION;
stmt->as.expression.expr = expr;
return stmt;
}
return NULL;
}

struct List parse_stmt_list(struct Lexer *lxr)
{
struct List stmts = list_init();
struct Stmt *stmt;
while ((stmt = parse_stmt(lxr)))
list_push(&stmts, (void *) stmt);
return stmts;
}

struct Stmt * parse_var_decl(struct Lexer *lxr)
{
if (!lexer_accept(lxr, TOK_LET))
return NULL;
lexer_expect(lxr, TOK_IDENT);
char *name = lxr->token;
lexer_expect(lxr, TOK_COLON);
struct Type *type;
if (!(type = parse_type(lxr)))
todo;
struct Expr *initializer = NULL;
if (lexer_accept(lxr, TOK_EQUAL))
{
if (!(initializer = parse_expression(lxr)))
return NULL;
}
lexer_expect(lxr, TOK_SEMI);
struct Stmt *stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_VAR_DECL;
stmt->as.var_decl.name = name;
stmt->as.var_decl.type = type;
stmt->as.var_decl.initializer = initializer;
return stmt;
}

struct List parse_switch_arms(struct Lexer *lxr)
{
struct List arms = list_init();
while (lexer_accept(lxr, TOK_CASE) || lexer_accept(lxr, TOK_DEFAULT))
{
struct SwitchArm *arm;
if (lxr->tok_tag == TOK_CASE)
{
struct Expr *test;
if (!(test = parse_expression(lxr)))
todo;
lexer_expect(lxr, TOK_COLON);
struct List stmts = parse_stmt_list(lxr);
arm = malloc(sizeof (*arm));
arm->test = test;
arm->stmts = stmts;
list_push(&arms, (void *) arm);
}
else
if (lxr->tok_tag == TOK_DEFAULT)
{
lexer_expect(lxr, TOK_COLON);
struct List stmts = parse_stmt_list(lxr);
arm = malloc(sizeof (*arm));
arm->test = NULL;
arm->stmts = stmts;
list_push(&arms, (void *) arm);
}
}
return arms;
}

bool display_bin_op(FILE *out, char *op, struct Expr *x, struct Expr *y)
{
return display_expr(out, x) && fprintf_s(out, " %s ", op) >= 0 && display_expr(out, y);
}

bool display_expr(FILE *out, struct Expr *expr)
{
switch (expr->tag)
{
case EXPR_LITERAL_INT:
{
return fprintf_s(out, "%d", expr->as.literal_int.value) >= 0;
}
case EXPR_LITERAL_CHAR:
{
return fprintf_s(out, "'%s'", expr->as.literal_char.value) >= 0;
}
case EXPR_LITERAL_STRING:
{
return fprintf_s(out, "\"%s\"", expr->as.literal_string.value) >= 0;
}
case EXPR_LITERAL_BOOL:
{
if (expr->as.literal_bool.value)
return fprintf_s(out, "true") >= 0;
else
return fprintf_s(out, "false") >= 0;
}
case EXPR_IDENT:
{
return fprintf_s(out, "%s", expr->as.ident.value) >= 0;
}
case EXPR_PARENTHESIZED:
{
return fprintf_s(out, "(") >= 0 && display_expr(out, expr->as.parenthesized.child) && fprintf_s(out, ")") >= 0;
}
case EXPR_ARR_INDEX:
{
return display_expr(out, expr->as.arr_index.arr) && fprintf_s(out, "[") >= 0 && display_expr(out, expr->as.arr_index.index) && fprintf_s(out, "]") >= 0;
}
case EXPR_CALL:
{
return display_expr(out, expr->as.call.callable) && fprintf_s(out, "(") >= 0 && display_arglist(out, &expr->as.call.args) && fprintf_s(out, ")") >= 0;
}
case EXPR_DOT_FIELD_ACCESS:
{
return display_expr(out, expr->as.field_access.object) && fprintf_s(out, ".%s", expr->as.field_access.field) >= 0;
}
case EXPR_ARROW_FIELD_ACCESS:
{
return display_expr(out, expr->as.field_access.object) && fprintf_s(out, "->%s", expr->as.field_access.field) >= 0;
}
case EXPR_POSTFIX_PLUS_PLUS:
{
return display_expr(out, expr->as.postfix_plus_plus.operand) && fprintf_s(out, "++") >= 0;
}
case EXPR_REFERENCE:
{
return fprintf_s(out, "&") >= 0 && display_expr(out, expr->as.unary_op.operand);
}
case EXPR_DEREFERENCE:
{
return fprintf_s(out, "*") >= 0 && display_expr(out, expr->as.unary_op.operand);
}
case EXPR_NEGATION:
{
return fprintf_s(out, "!") >= 0 && display_expr(out, expr->as.unary_op.operand);
}
case EXPR_SIZEOF_EXPR:
{
return fprintf_s(out, "sizeof ") >= 0 && display_expr(out, expr->as.sizeof_expr.operand);
}
case EXPR_SIZEOF_TYPE:
{
return fprintf_s(out, "sizeof(") >= 0 && display_type(out, expr->as.sizeof_type.type) && fprintf_s(out, ")") >= 0;
}
case EXPR_CAST:
{
return fprintf_s(out, "(") >= 0 && display_type(out, expr->as.cast.type) && fprintf_s(out, ") ") >= 0 && display_expr(out, expr->as.cast.expr);
}
case EXPR_ADD:
{
return display_bin_op(out, "+", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_SUB:
{
return display_bin_op(out, "-", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_GT:
{
return display_bin_op(out, ">", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_GTE:
{
return display_bin_op(out, ">=", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_LT:
{
return display_bin_op(out, "<", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_LTE:
{
return display_bin_op(out, "<=", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_EQUAL:
{
return display_bin_op(out, "==", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_NOT_EQUAL:
{
return display_bin_op(out, "!=", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_AND:
{
return display_bin_op(out, "&&", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_OR:
{
return display_bin_op(out, "||", expr->as.binary_op.x, expr->as.binary_op.y);
}
case EXPR_ASSIGN:
{
return display_bin_op(out, "=", expr->as.binary_op.x, expr->as.binary_op.y);
}
default:
{
printf("display_expr is not implemented for EXPR tag %d!\n", expr->tag);
exit(1);
}
}
}

bool display_arglist(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
struct Expr *expr = (struct Expr *) node->data;
if (node->next)
{
bool result = display_expr(out, expr) && fprintf_s(out, ", ") >= 0;
if (!result)
return false;
node = node->next;
}
else
{
return display_expr(out, expr);
}
}
return true;
}

struct Expr * parse_primary_expression(struct Lexer *lxr)
{
struct Expr *expr;
if (lexer_accept(lxr, TOK_LITERAL_STRING))
{
expr = malloc(sizeof (*expr));
expr->tag = EXPR_LITERAL_STRING;
expr->as.literal_string.value = lxr->token;
return expr;
}
if ((expr = parse_constant(lxr)))
{
return expr;
}
if (lexer_accept(lxr, TOK_IDENT))
{
expr = malloc(sizeof (*expr));
expr->tag = EXPR_IDENT;
expr->as.ident.value = lxr->token;
return expr;
}
if (lexer_accept(lxr, TOK_OPEN_PAREN) && (expr = parse_expression(lxr)) && lexer_expect(lxr, TOK_CLOSE_PAREN))
{
struct Expr *paren_expr = malloc(sizeof (*paren_expr));
paren_expr->tag = EXPR_PARENTHESIZED;
paren_expr->as.parenthesized.child = expr;
return paren_expr;
}
return NULL;
}

struct Expr * parse_postfix_expression(struct Lexer *lxr)
{
struct Expr *expr;
if (!(expr = parse_primary_expression(lxr)))
return NULL;
while (true)
{
struct Expr *index;
if (lexer_accept(lxr, TOK_OPEN_BRACK) && (index = parse_expression(lxr)) && lexer_expect(lxr, TOK_CLOSE_BRACK))
{
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_ARR_INDEX;
new_expr->as.arr_index.arr = expr;
new_expr->as.arr_index.index = index;
expr = new_expr;
continue;
}
if (lexer_accept(lxr, TOK_OPEN_PAREN))
{
struct List arg_list = parse_argument_expression_list(lxr);
lexer_expect(lxr, TOK_CLOSE_PAREN);
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_CALL;
new_expr->as.call.callable = expr;
new_expr->as.call.args = arg_list;
expr = new_expr;
continue;
}
if (lexer_accept(lxr, TOK_DOT) && lexer_expect(lxr, TOK_IDENT))
{
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_DOT_FIELD_ACCESS;
new_expr->as.field_access.object = expr;
new_expr->as.field_access.field = lxr->token;
expr = new_expr;
continue;
}
if (lexer_accept(lxr, TOK_ARROW) && lexer_expect(lxr, TOK_IDENT))
{
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_ARROW_FIELD_ACCESS;
new_expr->as.field_access.object = expr;
new_expr->as.field_access.field = lxr->token;
expr = new_expr;
continue;
}
if (lexer_accept(lxr, TOK_PLUS_PLUS))
{
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_POSTFIX_PLUS_PLUS;
new_expr->as.postfix_plus_plus.operand = expr;
expr = new_expr;
continue;
}
break;
}
return expr;
}

struct List parse_argument_expression_list(struct Lexer *lxr)
{
struct Expr *expr;
struct List list = list_init();
if (!(expr = parse_assignment_expression(lxr)))
return list;
list_push(&list, (void *) expr);
while (lexer_accept(lxr, TOK_COMMA))
{
if (!(expr = parse_assignment_expression(lxr)))
break;
list_push(&list, (void *) expr);
}
return list;
}

struct Expr * parse_unary_expression(struct Lexer *lxr)
{
struct Expr *expr;
if ((expr = parse_postfix_expression(lxr)))
return expr;
if (lexer_accept(lxr, TOK_AMPERSAND))
{
struct Expr *operand;
if (!(operand = parse_cast_expression(lxr)))
return NULL;
expr = malloc(sizeof (*expr));
expr->tag = EXPR_REFERENCE;
expr->as.unary_op.operand = operand;
return expr;
}
if (lexer_accept(lxr, TOK_STAR))
{
struct Expr *operand;
if (!(operand = parse_cast_expression(lxr)))
return NULL;
expr = malloc(sizeof (*expr));
expr->tag = EXPR_DEREFERENCE;
expr->as.unary_op.operand = operand;
return expr;
}
if (lexer_accept(lxr, TOK_BANG))
{
struct Expr *operand;
if (!(operand = parse_cast_expression(lxr)))
return NULL;
expr = malloc(sizeof (*expr));
expr->tag = EXPR_NEGATION;
expr->as.unary_op.operand = operand;
return expr;
}
if (lexer_accept(lxr, TOK_SIZEOF))
{
struct Expr *operand;
{
struct Lexer saved_lxr = *lxr;
if ((operand = parse_unary_expression(lxr)))
{
expr = malloc(sizeof (*expr));
expr->tag = EXPR_SIZEOF_EXPR;
expr->as.sizeof_expr.operand = operand;
return expr;
}
*lxr = saved_lxr;
}
struct Type *type;
if (lexer_accept(lxr, TOK_OPEN_PAREN) && (type = parse_type(lxr)) && lexer_expect(lxr, TOK_CLOSE_PAREN))
{
expr = malloc(sizeof (*expr));
expr->tag = EXPR_SIZEOF_TYPE;
expr->as.sizeof_type.type = type;
return expr;
}
return NULL;
}
return NULL;
}

struct Expr * parse_cast_expression(struct Lexer *lxr)
{
{
struct Lexer saved_lxr = *lxr;
{
struct Type *type;
struct Expr *expr;
if (lexer_accept(lxr, TOK_OPEN_PAREN) && (type = parse_type(lxr)) && lexer_accept(lxr, TOK_CLOSE_PAREN) && (expr = parse_cast_expression(lxr)))
{
struct Expr *cast_expr = malloc(sizeof (*cast_expr));
cast_expr->tag = EXPR_CAST;
cast_expr->as.cast.type = type;
cast_expr->as.cast.expr = expr;
return cast_expr;
}
}
*lxr = saved_lxr;
}
return parse_unary_expression(lxr);
}

struct Expr * parse_multiplicative_expression(struct Lexer *lxr)
{
return parse_cast_expression(lxr);
}

struct Expr * parse_additive_expression(struct Lexer *lxr)
{
struct Expr *accum;
if (!(accum = parse_multiplicative_expression(lxr)))
return NULL;
while (lexer_accept(lxr, TOK_PLUS) || lexer_accept(lxr, TOK_MINUS))
{
struct Expr *y;
enum TokTag op = lxr->tok_tag;
if (!(y = parse_multiplicative_expression(lxr)))
{
fprintf_s(stderr, "PARSE ERROR: %s:%d\n", lxr->filename, lxr->line);
fprintf_s(stderr, "| expected expression after `%s`, got nothing\n", tok_tag_names[op]);
exit(1);
}
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->as.binary_op.x = accum;
new_expr->as.binary_op.y = y;
if (op == TOK_PLUS)
new_expr->tag = EXPR_ADD;
else
new_expr->tag = EXPR_SUB;
accum = new_expr;
}
return accum;
}

struct Expr * parse_shift_expression(struct Lexer *lxr)
{
return parse_additive_expression(lxr);
}

struct Expr * parse_relational_expression(struct Lexer *lxr)
{
struct Expr *accum;
if (!(accum = parse_shift_expression(lxr)))
return NULL;
while (lexer_accept(lxr, TOK_LTE) || lexer_accept(lxr, TOK_GTE) || lexer_accept(lxr, TOK_LT) || lexer_accept(lxr, TOK_GT))
{
struct Expr *y;
enum TokTag op = lxr->tok_tag;
if (!(y = parse_shift_expression(lxr)))
{
fprintf_s(stderr, "PARSE ERROR: %s:%d\n", lxr->filename, lxr->line);
fprintf_s(stderr, "| expected expression after `%s`, got nothing\n", tok_tag_names[op]);
exit(1);
}
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->as.binary_op.x = accum;
new_expr->as.binary_op.y = y;
if (op == TOK_LTE)
new_expr->tag = EXPR_LTE;
else
if (op == TOK_GTE)
new_expr->tag = EXPR_GTE;
else
if (op == TOK_LT)
new_expr->tag = EXPR_LT;
else
if (op == TOK_GT)
new_expr->tag = EXPR_GT;
accum = new_expr;
}
return accum;
}

struct Expr * parse_equality_expression(struct Lexer *lxr)
{
struct Expr *accum;
if (!(accum = parse_relational_expression(lxr)))
return NULL;
while (lexer_accept(lxr, TOK_EQUAL_EQUAL) || lexer_accept(lxr, TOK_NOT_EQUAL))
{
struct Expr *y;
enum TokTag op = lxr->tok_tag;
if (!(y = parse_relational_expression(lxr)))
{
fprintf_s(stderr, "PARSE ERROR: %s:%d\n", lxr->filename, lxr->line);
fprintf_s(stderr, "| expected expression after `%s`, got nothing\n", tok_tag_names[op]);
exit(1);
}
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->as.binary_op.x = accum;
new_expr->as.binary_op.y = y;
if (op == TOK_EQUAL_EQUAL)
new_expr->tag = EXPR_EQUAL;
else
if (op == TOK_NOT_EQUAL)
new_expr->tag = EXPR_NOT_EQUAL;
accum = new_expr;
}
return accum;
}

struct Expr * parse_logical_and_expression(struct Lexer *lxr)
{
struct Expr *accum;
if (!(accum = parse_equality_expression(lxr)))
return NULL;
while (lexer_accept(lxr, TOK_AND))
{
struct Expr *y;
if (!(y = parse_equality_expression(lxr)))
{
fprintf_s(stderr, "PARSE ERROR: %s:%d\n", lxr->filename, lxr->line);
fprintf_s(stderr, "| expected expression after `&&`, got nothing\n");
exit(1);
}
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_AND;
new_expr->as.binary_op.x = accum;
new_expr->as.binary_op.y = y;
accum = new_expr;
}
return accum;
}

struct Expr * parse_logical_or_expression(struct Lexer *lxr)
{
struct Expr *accum;
if (!(accum = parse_logical_and_expression(lxr)))
return NULL;
while (lexer_accept(lxr, TOK_OR))
{
struct Expr *y;
if (!(y = parse_logical_and_expression(lxr)))
{
fprintf_s(stderr, "PARSE ERROR: %s:%d\n", lxr->filename, lxr->line);
fprintf_s(stderr, "| expected expression after `||`, got nothing\n");
exit(1);
}
struct Expr *new_expr = malloc(sizeof (*new_expr));
new_expr->tag = EXPR_OR;
new_expr->as.binary_op.x = accum;
new_expr->as.binary_op.y = y;
accum = new_expr;
}
return accum;
}

struct Expr * parse_conditional_expression(struct Lexer *lxr)
{
return parse_logical_or_expression(lxr);
}

struct Expr * parse_assignment_expression(struct Lexer *lxr)
{
struct Expr *unary;
struct Expr *assignment;
{
struct Lexer saved_lxr = *lxr;
if ((unary = parse_unary_expression(lxr)) && lexer_accept(lxr, TOK_EQUAL) && (assignment = parse_assignment_expression(lxr)))
{
struct Expr *expr = malloc(sizeof (*expr));
expr->tag = EXPR_ASSIGN;
expr->as.binary_op.x = unary;
expr->as.binary_op.y = assignment;
return expr;
}
*lxr = saved_lxr;
}
return parse_conditional_expression(lxr);
}

struct Expr * parse_expression(struct Lexer *lxr)
{
return parse_assignment_expression(lxr);
}

struct Expr * parse_constant(struct Lexer *lxr)
{
if (lexer_accept(lxr, TOK_LITERAL_INT))
{
struct Expr *expr = malloc(sizeof (*expr));
expr->tag = EXPR_LITERAL_INT;
expr->as.literal_int.value = atoi(lxr->token);
return expr;
}
if (lexer_accept(lxr, TOK_TRUE) || lexer_accept(lxr, TOK_FALSE))
{
struct Expr *expr = malloc(sizeof (*expr));
expr->tag = EXPR_LITERAL_BOOL;
expr->as.literal_bool.value = (lxr->tok_tag == TOK_TRUE);
return expr;
}
if (lexer_accept(lxr, TOK_LITERAL_CHAR))
{
struct Expr *expr = malloc(sizeof (*expr));
expr->tag = EXPR_LITERAL_CHAR;
expr->as.literal_char.value = lxr->token;
return expr;
}
return NULL;
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
printf("\n\n%s\n", "Tests finished!");
}


#endif // PARASITIC_C_FILE__3E9
