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
struct DataMember;
struct Stmt;
struct SwitchArm;
struct BindingTriple;
struct Expr;
struct Kwarg;

// ENUM DEFNS //

// STRUCT DEFNS //
struct StructMember {
    char *field;
    struct Type *type;
};

struct Type {
    enum {
    P8C__Type__TAG__Int,
    P8C__Type__TAG__Bool,
    P8C__Type__TAG__Char,
    P8C__Type__TAG__Void,
    P8C__Type__TAG__CStrArr,
    P8C__Type__TAG__File,
    P8C__Type__TAG__Ptr,
    P8C__Type__TAG__FnPtr,
    P8C__Type__TAG__NamedStructRef,
    P8C__Type__TAG__NamedEnumRef,
    P8C__Type__TAG__NamedDataRef,
    P8C__Type__TAG__AnonStruct,
    P8C__Type__TAG__AnonUnion,
    P8C__Type__TAG__AnonEnum,
} tag;
    union {
    struct {
    char _;
} P8C__Type__AS__Int;
    struct {
    char _;
} P8C__Type__AS__Bool;
    struct {
    char _;
} P8C__Type__AS__Char;
    struct {
    char _;
} P8C__Type__AS__Void;
    struct {
    char _;
} P8C__Type__AS__CStrArr;
    struct {
    char _;
} P8C__Type__AS__File;
    struct {
    struct Type *child;
} P8C__Type__AS__Ptr;
    struct {
    struct List param_types;
    struct Type *ret_type;
} P8C__Type__AS__FnPtr;
    struct {
    char *name;
} P8C__Type__AS__NamedStructRef;
    struct {
    char *name;
} P8C__Type__AS__NamedEnumRef;
    struct {
    char *name;
} P8C__Type__AS__NamedDataRef;
    struct {
    struct List struct_members;
} P8C__Type__AS__AnonStruct;
    struct {
    struct List struct_members;
} P8C__Type__AS__AnonUnion;
    struct {
    struct List enum_members;
} P8C__Type__AS__AnonEnum;
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
    ITEM_NAMED_DATA_DECL,
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
    char *name;
    struct List members;
} named_data_decl;
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

struct DataMember {
    char *name;
    struct List struct_members;
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
    STMT_IF_LET,
    STMT_LET_ELSE,
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
    char *type_name;
    char *variant_name;
    struct List bindings;
    struct Expr *expr;
    struct Stmt *consequent;
    struct Stmt *alternative;
} if_let;
    struct {
    char *type_name;
    char *variant_name;
    struct List bindings;
    struct Expr *expr;
    struct Stmt *alternative;
} let_else;
    struct {
    struct Expr *expr;
} expression;
} as;
};

struct SwitchArm {
    struct Expr *test;
    struct List stmts;
};

struct BindingTriple {
    char *field;
    char *binder;
    struct Type *type;
};

struct Expr {
    enum {
    EXPR_LITERAL_INT,
    EXPR_LITERAL_CHAR,
    EXPR_LITERAL_STRING,
    EXPR_LITERAL_BOOL,
    EXPR_LITERAL_VARIANT,
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
    char *type_name;
    char *variant_name;
    struct List initializer_list;
} literal_variant;
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

struct Kwarg {
    char *field;
    struct Expr *expr;
};


// GLOBAL DEFNS //
char * PARAC_PREFIX = "P8C";

int NEXT_TU_ID = 1000;

int TEMP_ID = 0;


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
char * mangle_variant_tag_name(char *type_name, char *variant_name);
char * mangle_variant_struct_name(char *type_name, char *variant_name);
bool display_named_data_decl(FILE *out, char *name, struct List *members);
bool data_decl_is_pure_c_style_enum(struct List *members);
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
struct List parse_data_member_list(struct Lexer *lxr);
bool display_switch_arms(FILE *out, struct List *arms);
bool display_stmt(FILE *out, struct Stmt *stmt);
bool display_stmt_list(FILE *out, struct List *list);
char * gen_temp();
bool display_if_let(FILE *out, char *type_name, char *variant_name, struct List *bindings, struct Expr *expr, struct Stmt *consequent, struct Stmt *alternative);
bool display_let_else(FILE *out, char *type_name, char *variant_name, struct List *bindings, struct Expr *expr, struct Stmt *alternative);
struct Stmt * parse_stmt(struct Lexer *lxr);
struct List parse_stmt_list(struct Lexer *lxr);
struct Stmt * parse_var_decl_follow(struct Lexer *lxr, char *name);
struct List parse_struct_pattern_bindings(struct Lexer *lxr);
struct Stmt * parse_stmt_block(struct Lexer *lxr);
struct Stmt * parse_if_let_follow(struct Lexer *lxr);
struct Stmt * parse_let_else_follow(struct Lexer *lxr, char *type_name);
struct List parse_switch_arms(struct Lexer *lxr);
bool display_bin_op(FILE *out, char *op, struct Expr *x, struct Expr *y);
bool display_literal_variant(FILE *out, char *type_name, char *variant_name, struct List initializer_list);
bool display_expr(FILE *out, struct Expr *expr);
bool display_arglist(FILE *out, struct List *list);
struct List parse_struct_initializer_list(struct Lexer *lxr);
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
if (type == NULL)
{
return fprintf_s(out, "void ") >= 0;
}
struct Type P8C__TEMP_0 = *type;
if (P8C__TEMP_0.tag == P8C__Type__TAG__FnPtr) {struct List param_types = P8C__TEMP_0.as.P8C__Type__AS__FnPtr.param_types;struct Type *ret_type = P8C__TEMP_0.as.P8C__Type__AS__FnPtr.ret_type;{
if (binder)
{
if (!display_fn_pointer_type(out, ret_type, NULL))
return false;
if (!(fprintf_s(out, "(*%s)(", binder) >= 0 && display_param_types(out, &param_types) && fprintf_s(out, ")") >= 0))
return false;
struct Type *t = ret_type;
while (t != NULL)
{
struct Type P8C__TEMP_1 = *t;
if (P8C__TEMP_1.tag == P8C__Type__TAG__FnPtr) {struct List pts = P8C__TEMP_1.as.P8C__Type__AS__FnPtr.param_types;struct Type *child_ret_type = P8C__TEMP_1.as.P8C__Type__AS__FnPtr.ret_type;{
if (!(fprintf_s(out, ")(") >= 0 && display_param_types(out, &pts) && fprintf_s(out, ")") >= 0))
return false;
t = child_ret_type;
}

} else {
{
break;
}
}
}
return true;
}
else
{
return display_fn_pointer_type(out, ret_type, NULL) && fprintf_s(out, "(*") >= 0;
}
}

} else {
{
return display_type(out, type) && fprintf_s(out, " ") >= 0;
}
}
}

bool display_fn_signature(FILE *out, char *name, struct List *fn_params, struct Type *ret_type)
{
if (!display_fn_pointer_type(out, ret_type, NULL))
return false;
if (!(fprintf_s(out, "%s(", name) >= 0 && display_fn_params(out, fn_params) && fprintf_s(out, ")") >= 0))
return false;
struct Type *t = ret_type;
while (t)
{
struct Type P8C__TEMP_2 = *t;
if (P8C__TEMP_2.tag == P8C__Type__TAG__FnPtr) {struct List param_types = P8C__TEMP_2.as.P8C__Type__AS__FnPtr.param_types;struct Type *rt = P8C__TEMP_2.as.P8C__Type__AS__FnPtr.ret_type;{
if (!(fprintf_s(out, ")(") >= 0 && display_param_types(out, &param_types) && fprintf_s(out, ")") >= 0))
return false;
t = rt;
}

} else {
{
break;
}
}
}
return true;
}

bool display_typed_identifier(FILE *out, struct Type *type, char *ident)
{
char *space = " ";
if (ident == NULL || ident[0] == '\0')
space = "";
struct Type P8C__TEMP_3 = *type;
if (P8C__TEMP_3.tag == P8C__Type__TAG__Int) {{
return fprintf_s(out, "int%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_4 = *type;
if (P8C__TEMP_4.tag == P8C__Type__TAG__Bool) {{
return fprintf_s(out, "bool%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_5 = *type;
if (P8C__TEMP_5.tag == P8C__Type__TAG__Char) {{
return fprintf_s(out, "char%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_6 = *type;
if (P8C__TEMP_6.tag == P8C__Type__TAG__Void) {{
return fprintf_s(out, "void%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_7 = *type;
if (P8C__TEMP_7.tag == P8C__Type__TAG__CStrArr) {{
return fprintf_s(out, "cstr_arr%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_8 = *type;
if (P8C__TEMP_8.tag == P8C__Type__TAG__File) {{
return fprintf_s(out, "FILE%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_9 = *type;
if (P8C__TEMP_9.tag == P8C__Type__TAG__Ptr) {struct Type *child = P8C__TEMP_9.as.P8C__Type__AS__Ptr.child;{
return display_typed_identifier(out, child, "") && fprintf_s(out, " *%s", ident) >= 0;
}

}struct Type P8C__TEMP_10 = *type;
if (P8C__TEMP_10.tag == P8C__Type__TAG__FnPtr) {{
return display_fn_pointer_type(out, type, ident);
}

}struct Type P8C__TEMP_11 = *type;
if (P8C__TEMP_11.tag == P8C__Type__TAG__NamedStructRef) {char *name = P8C__TEMP_11.as.P8C__Type__AS__NamedStructRef.name;{
return fprintf_s(out, "struct %s%s%s", name, space, ident) >= 0;
}

}struct Type P8C__TEMP_12 = *type;
if (P8C__TEMP_12.tag == P8C__Type__TAG__NamedEnumRef) {char *name = P8C__TEMP_12.as.P8C__Type__AS__NamedEnumRef.name;{
return fprintf_s(out, "enum %s%s%s", name, space, ident) >= 0;
}

}struct Type P8C__TEMP_13 = *type;
if (P8C__TEMP_13.tag == P8C__Type__TAG__NamedDataRef) {char *name = P8C__TEMP_13.as.P8C__Type__AS__NamedDataRef.name;{
return fprintf_s(out, "struct %s%s%s", name, space, ident) >= 0;
}

}struct Type P8C__TEMP_14 = *type;
if (P8C__TEMP_14.tag == P8C__Type__TAG__AnonStruct) {struct List sms = P8C__TEMP_14.as.P8C__Type__AS__AnonStruct.struct_members;{
return fprintf_s(out, "struct {\n") >= 0 && display_struct_members(out, &sms) && fprintf_s(out, "}%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_15 = *type;
if (P8C__TEMP_15.tag == P8C__Type__TAG__AnonUnion) {struct List sms = P8C__TEMP_15.as.P8C__Type__AS__AnonUnion.struct_members;{
return fprintf_s(out, "union {\n") >= 0 && display_struct_members(out, &sms) && fprintf_s(out, "}%s%s", space, ident) >= 0;
}

}struct Type P8C__TEMP_16 = *type;
if (P8C__TEMP_16.tag == P8C__Type__TAG__AnonEnum) {struct List ems = P8C__TEMP_16.as.P8C__Type__AS__AnonEnum.enum_members;{
return fprintf_s(out, "enum {\n") >= 0 && display_enum_members(out, &ems) && fprintf_s(out, "}%s%s", space, ident) >= 0;
}

}todo;
return false;
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
struct StructMember *sm = (struct StructMember *) node->value;
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
char *enum_constant_name = (char *) node->value;
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
*new_type = (struct Type) {
.tag = P8C__Type__TAG__Ptr,
.as = {
.P8C__Type__AS__Ptr = {
.child = type,
},
},
};
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
*type = (struct Type) {
.tag = P8C__Type__TAG__Void,
.as = {
.P8C__Type__AS__Void = {
._ = 'x',
},
},
};
break;
}
case TOK_CHAR:
{
*type = (struct Type) {
.tag = P8C__Type__TAG__Char,
.as = {
.P8C__Type__AS__Char = {
._ = 'x',
},
},
};
break;
}
case TOK_INT:
{
*type = (struct Type) {
.tag = P8C__Type__TAG__Int,
.as = {
.P8C__Type__AS__Int = {
._ = 'x',
},
},
};
break;
}
case TOK_BOOL:
{
*type = (struct Type) {
.tag = P8C__Type__TAG__Bool,
.as = {
.P8C__Type__AS__Bool = {
._ = 'x',
},
},
};
break;
}
case TOK_CSTR_ARR:
{
*type = (struct Type) {
.tag = P8C__Type__TAG__CStrArr,
.as = {
.P8C__Type__AS__CStrArr = {
._ = 'x',
},
},
};
break;
}
case TOK_FILE:
{
*type = (struct Type) {
.tag = P8C__Type__TAG__File,
.as = {
.P8C__Type__AS__File = {
._ = 'x',
},
},
};
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
*type = (struct Type) {
.tag = P8C__Type__TAG__NamedStructRef,
.as = {
.P8C__Type__AS__NamedStructRef = {
.name = lxr->token,
},
},
};
return type;
}
if (lexer_expect(lxr, TOK_OPEN_BRACE))
{
struct List members = parse_struct_member_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__AnonStruct,
.as = {
.P8C__Type__AS__AnonStruct = {
.struct_members = members,
},
},
};
return type;
}
return NULL;
}
if (lexer_accept(lxr, TOK_ENUM))
{
if (lexer_accept(lxr, TOK_IDENT))
{
type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__NamedEnumRef,
.as = {
.P8C__Type__AS__NamedEnumRef = {
.name = lxr->token,
},
},
};
return type;
}
if (lexer_expect(lxr, TOK_OPEN_BRACE))
{
struct List members = parse_enumerator_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__AnonEnum,
.as = {
.P8C__Type__AS__AnonEnum = {
.enum_members = members,
},
},
};
return type;
}
return NULL;
}
if (lexer_accept(lxr, TOK_DATA))
{
lexer_expect(lxr, TOK_IDENT);
type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__NamedDataRef,
.as = {
.P8C__Type__AS__NamedDataRef = {
.name = lxr->token,
},
},
};
return type;
}
if (lexer_accept(lxr, TOK_UNION))
{
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List members = parse_struct_member_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__AnonUnion,
.as = {
.P8C__Type__AS__AnonUnion = {
.struct_members = members,
},
},
};
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
*type = (struct Type) {
.tag = P8C__Type__TAG__FnPtr,
.as = {
.P8C__Type__AS__FnPtr = {
.param_types = param_types,
.ret_type = ret_type,
},
},
};
return type;
}
return NULL;
}

bool display_const_array(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
struct ConstantExpr *expr = (struct ConstantExpr *) node->value;
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
struct Type *type = (struct Type *) node->value;
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
struct FnParam *param = (struct FnParam *) node->value;
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
case ITEM_NAMED_DATA_DECL:
{
return display_named_data_decl(out, item->as.named_data_decl.name, &item->as.named_data_decl.members);
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

char * mangle_variant_tag_name(char *type_name, char *variant_name)
{
return format_alloc("%s__%s__TAG__%s", PARAC_PREFIX, type_name, variant_name);
}

char * mangle_variant_struct_name(char *type_name, char *variant_name)
{
return format_alloc("%s__%s__AS__%s", PARAC_PREFIX, type_name, variant_name);
}

bool display_named_data_decl(FILE *out, char *name, struct List *members)
{
struct List struct_components = list_init();
{
struct List tags = list_init();
struct ListNode *iter = members->first;
while (iter)
{
struct DataMember *dm = (struct DataMember *) iter->value;
char *mangled_name = mangle_variant_tag_name(name, dm->name);
list_push(&tags, (void *) mangled_name);
iter = iter->next;
}
struct Type *tags_enum = malloc(sizeof (*tags_enum));
*tags_enum = (struct Type) {
.tag = P8C__Type__TAG__AnonEnum,
.as = {
.P8C__Type__AS__AnonEnum = {
.enum_members = tags,
},
},
};
{
struct StructMember *enum_component = malloc(sizeof (*enum_component));
enum_component->field = "tag";
enum_component->type = tags_enum;
list_push(&struct_components, (void *) enum_component);
}
}
if (!data_decl_is_pure_c_style_enum(members))
{
struct List struct_members = list_init();
struct ListNode *iter = members->first;
while (iter)
{
struct DataMember *dm = (struct DataMember *) iter->value;
if (!list_is_empty(&dm->struct_members))
{
struct Type *anon_struct = malloc(sizeof (*anon_struct));
*anon_struct = (struct Type) {
.tag = P8C__Type__TAG__AnonStruct,
.as = {
.P8C__Type__AS__AnonStruct = {
.struct_members = dm->struct_members,
},
},
};
struct StructMember *sm = malloc(sizeof (*sm));
sm->field = mangle_variant_struct_name(name, dm->name);
sm->type = anon_struct;
list_push(&struct_members, (void *) sm);
}
iter = iter->next;
}
{
struct Type *union_of_structs = malloc(sizeof (*union_of_structs));
*union_of_structs = (struct Type) {
.tag = P8C__Type__TAG__AnonUnion,
.as = {
.P8C__Type__AS__AnonUnion = {
.struct_members = struct_members,
},
},
};
struct StructMember *union_member = malloc(sizeof (*union_member));
union_member->field = "as";
union_member->type = union_of_structs;
list_push(&struct_components, (void *) union_member);
}
}
struct Item *struct_decl = malloc(sizeof (*struct_decl));
struct_decl->tag = ITEM_NAMED_STRUCT_DECL;
struct_decl->as.named_struct_decl.name = name;
struct_decl->as.named_struct_decl.members = struct_components;
return display_item(out, struct_decl);
}

bool data_decl_is_pure_c_style_enum(struct List *members)
{
struct ListNode *iter = members->first;
while (iter)
{
struct DataMember *dm = (struct DataMember *) iter->value;
if (!list_is_empty(&dm->struct_members))
return false;
iter = iter->next;
}
return true;
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
}
case ITEM_NAMED_DATA_DECL:
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
struct Item *item = (struct Item *) node->value;
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
struct Item *item = (struct Item *) node->value;
if (item->tag == ITEM_NAMED_STRUCT_DECL)
{
if (!(fprintf_s(out, "struct %s;\n", item->as.named_struct_decl.name) >= 0))
return false;
}
else
if (item->tag == ITEM_NAMED_DATA_DECL)
{
if (!(fprintf_s(out, "struct %s;\n", item->as.named_data_decl.name) >= 0))
return false;
}
else
todo;
node = node->next;
}
return true;
}

bool display_tu_fn_decls(FILE *out, struct TranslationUnit *tu)
{
struct ListNode *node = tu->fn_defns.first;
while (node)
{
struct Item *item = (struct Item *) node->value;
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
*ret_type = (struct Type) {
.tag = P8C__Type__TAG__Void,
.as = {
.P8C__Type__AS__Void = {
._ = 'x',
},
},
};
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
if (lexer_accept(lxr, TOK_DATA))
{
lexer_expect(lxr, TOK_IDENT);
char *name = lxr->token;
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List members = parse_data_member_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
if (lexer_accept(lxr, TOK_SEMI))
{
fprintf_s(stderr, "[WARNING] %s:%d:\n", lxr->filename, lxr->line);
fprintf_s(stderr, "    A semicolon is not needed after a data definition.\n");
}
struct Item *item = malloc(sizeof (*item));
item->tag = ITEM_NAMED_DATA_DECL;
item->as.named_data_decl.name = name;
item->as.named_data_decl.members = members;
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

struct List parse_data_member_list(struct Lexer *lxr)
{
struct List list = list_init();
while (lexer_accept(lxr, TOK_IDENT))
{
char *variant_name = lxr->token;
struct List struct_members = list_init();
if (lexer_accept(lxr, TOK_OPEN_BRACE))
{
while (lexer_accept(lxr, TOK_IDENT))
{
char *field = lxr->token;
lexer_expect(lxr, TOK_COLON);
struct Type *type;
if (!(type = parse_type(lxr)))
todo;
struct StructMember *sm = malloc(sizeof (*sm));
sm->field = field;
sm->type = type;
list_push(&struct_members, (void *) sm);
if (!lexer_accept(lxr, TOK_COMMA))
break;
}
lexer_expect(lxr, TOK_CLOSE_BRACE);
}
struct DataMember *dm = malloc(sizeof (*dm));
dm->name = variant_name;
dm->struct_members = struct_members;
list_push(&list, (void *) dm);
if (!lexer_accept(lxr, TOK_COMMA))
break;
}
return list;
}

bool display_switch_arms(FILE *out, struct List *arms)
{
struct ListNode *node = arms->first;
while (node)
{
struct SwitchArm *arm = (struct SwitchArm *) node->value;
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
case STMT_IF_LET:
{
return display_if_let(out, stmt->as.if_let.type_name, stmt->as.if_let.variant_name, &stmt->as.if_let.bindings, stmt->as.if_let.expr, stmt->as.if_let.consequent, stmt->as.if_let.alternative);
}
case STMT_LET_ELSE:
{
return display_let_else(out, stmt->as.let_else.type_name, stmt->as.let_else.variant_name, &stmt->as.let_else.bindings, stmt->as.let_else.expr, stmt->as.let_else.alternative);
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
struct Stmt *stmt = (struct Stmt *) node->value;
bool result = display_stmt(out, stmt);
if (!result)
return false;
node = node->next;
}
return true;
}

char * gen_temp()
{
char *name = format_alloc("%s__TEMP_%d", PARAC_PREFIX, TEMP_ID);
TEMP_ID = TEMP_ID + 1;
return name;
}

bool display_if_let(FILE *out, char *type_name, char *variant_name, struct List *bindings, struct Expr *expr, struct Stmt *consequent, struct Stmt *alternative)
{
char *expr_temp = gen_temp();
struct Type *type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__NamedDataRef,
.as = {
.P8C__Type__AS__NamedDataRef = {
.name = type_name,
},
},
};
if (!(display_typed_identifier(out, type, expr_temp) && fprintf_s(out, " = ") >= 0 && display_expr(out, expr) && fprintf_s(out, ";\n") >= 0))
return false;
{
char *tag_name = mangle_variant_tag_name(type_name, variant_name);
if (!(fprintf_s(out, "if (%s.tag == %s) {", expr_temp, tag_name) >= 0))
return false;
}
struct ListNode *iter = bindings->first;
while (iter)
{
struct BindingTriple *bt = (struct BindingTriple *) iter->value;
if (!display_typed_identifier(out, bt->type, bt->binder))
return false;
char *variant_struct_name = mangle_variant_struct_name(type_name, variant_name);
if (!(fprintf_s(out, " = %s.as.%s.%s;", expr_temp, variant_struct_name, bt->field) >= 0))
return false;
iter = iter->next;
}
if (!display_stmt(out, consequent))
return false;
if (!(fprintf_s(out, "\n}") >= 0))
return false;
if (alternative != NULL)
{
if (!(fprintf_s(out, " else {\n") >= 0))
return false;
if (!display_stmt(out, alternative))
return false;
if (!(fprintf_s(out, "}\n") >= 0))
return false;
}
return true;
}

bool display_let_else(FILE *out, char *type_name, char *variant_name, struct List *bindings, struct Expr *expr, struct Stmt *alternative)
{
char *expr_temp = gen_temp();
struct Type *type = malloc(sizeof (*type));
*type = (struct Type) {
.tag = P8C__Type__TAG__NamedDataRef,
.as = {
.P8C__Type__AS__NamedDataRef = {
.name = type_name,
},
},
};
if (!(display_typed_identifier(out, type, expr_temp) && fprintf_s(out, " = ") >= 0 && display_expr(out, expr) && fprintf_s(out, ";\n") >= 0))
return false;
{
char *tag_name = mangle_variant_tag_name(type_name, variant_name);
if (!(fprintf_s(out, "if (%s.tag != %s)", expr_temp, tag_name) >= 0 && display_stmt(out, alternative)))
return false;
}
struct ListNode *iter = bindings->first;
while (iter)
{
struct BindingTriple *bt = (struct BindingTriple *) iter->value;
if (!display_typed_identifier(out, bt->type, bt->binder))
return false;
char *variant_struct_name = mangle_variant_struct_name(type_name, variant_name);
if (!(fprintf_s(out, " = %s.as.%s.%s;", expr_temp, variant_struct_name, bt->field) >= 0))
return false;
iter = iter->next;
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
if (lexer_accept(lxr, TOK_LET))
{
return parse_if_let_follow(lxr);
}
else
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
if (lexer_accept(lxr, TOK_LET))
{
lexer_expect(lxr, TOK_IDENT);
char *ident = lxr->token;
if (lexer_accept(lxr, TOK_COLON))
{
return parse_var_decl_follow(lxr, ident);
}
if (lexer_accept(lxr, TOK_COLON_COLON))
{
return parse_let_else_follow(lxr, ident);
}
todo;
}
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

struct Stmt * parse_var_decl_follow(struct Lexer *lxr, char *name)
{
struct Type *type;
if (!(type = parse_type(lxr)))
{
fprintf_s(stderr, "You must provide a type after a let binding!\n");
exit(1);
}
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

struct List parse_struct_pattern_bindings(struct Lexer *lxr)
{
struct List bindings = list_init();
if (!lexer_accept(lxr, TOK_OPEN_BRACE))
{
return bindings;
}
while (true)
{
if (!(lexer_accept(lxr, TOK_IDENT)))
break;
char *field = lxr->token;
char *binder;
if (lexer_accept(lxr, TOK_EQUAL))
{
lexer_expect(lxr, TOK_IDENT);
binder = lxr->token;
}
else
{
binder = field;
}
char *filename = lxr->filename;
int line = lxr->line;
struct Type *type;
if (!(lexer_accept(lxr, TOK_COLON) && (type = parse_type(lxr))))
{
fprintf_s(stderr, "PARSE_ERROR:\n| at %s:%d\n", filename, line);
fprintf_s(stderr, "|\n");
fprintf_s(stderr, "|  expected type annotation after pattern destructuring variable name\n");
fprintf_s(stderr, "|\n");
fprintf_s(stderr, "|> `%.20s`\n", lxr->old_input);
exit(1);
}
struct BindingTriple *bt = malloc(sizeof (*bt));
bt->field = field;
bt->binder = binder;
bt->type = type;
list_push(&bindings, (void *) bt);
if (!(lexer_accept(lxr, TOK_COMMA)))
break;
}
lexer_expect(lxr, TOK_CLOSE_BRACE);
return bindings;
}

struct Stmt * parse_stmt_block(struct Lexer *lxr)
{
lexer_expect(lxr, TOK_OPEN_BRACE);
struct List stmts = parse_stmt_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
struct Stmt *block = malloc(sizeof (*block));
block->tag = STMT_BLOCK;
block->as.block.stmts = stmts;
return block;
}

struct Stmt * parse_if_let_follow(struct Lexer *lxr)
{
lexer_expect(lxr, TOK_IDENT);
char *type_name = lxr->token;
lexer_expect(lxr, TOK_COLON_COLON);
lexer_expect(lxr, TOK_IDENT);
char *variant_name = lxr->token;
struct List bindings = parse_struct_pattern_bindings(lxr);
lexer_expect(lxr, TOK_EQUAL);
struct Expr *expr;
if (!(expr = parse_expression(lxr)))
todo;
struct Stmt *consequent = parse_stmt_block(lxr);
struct Stmt *alternative = NULL;
if (lexer_accept(lxr, TOK_ELSE))
{
alternative = parse_stmt_block(lxr);
}
struct Stmt *stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_IF_LET;
stmt->as.if_let.type_name = type_name;
stmt->as.if_let.variant_name = variant_name;
stmt->as.if_let.bindings = bindings;
stmt->as.if_let.expr = expr;
stmt->as.if_let.alternative = alternative;
stmt->as.if_let.consequent = consequent;
return stmt;
}

struct Stmt * parse_let_else_follow(struct Lexer *lxr, char *type_name)
{
lexer_expect(lxr, TOK_IDENT);
char *variant_name = lxr->token;
struct List bindings = parse_struct_pattern_bindings(lxr);
lexer_expect(lxr, TOK_EQUAL);
struct Expr *expr;
if (!(expr = parse_expression(lxr)))
todo;
lexer_expect(lxr, TOK_ELSE);
struct Stmt *alternative = parse_stmt_block(lxr);
struct Stmt *stmt = malloc(sizeof (*stmt));
stmt->tag = STMT_LET_ELSE;
stmt->as.let_else.type_name = type_name;
stmt->as.let_else.variant_name = variant_name;
stmt->as.let_else.bindings = bindings;
stmt->as.let_else.expr = expr;
stmt->as.let_else.alternative = alternative;
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

bool display_literal_variant(FILE *out, char *type_name, char *variant_name, struct List initializer_list)
{
if (!(fprintf_s(out, "(struct %s) {\n", type_name) >= 0))
return false;
if (!(fprintf_s(out, ".tag = %s,\n", mangle_variant_tag_name(type_name, variant_name)) >= 0))
return false;
if (!list_is_empty(&initializer_list))
{
if (!(fprintf_s(out, ".as = {\n") >= 0))
return false;
if (!(fprintf_s(out, ".%s = {\n", mangle_variant_struct_name(type_name, variant_name)) >= 0))
return false;
struct ListNode *iter = initializer_list.first;
while (iter)
{
struct Kwarg *kwarg = (struct Kwarg *) iter->value;
if (!(fprintf_s(out, ".%s = ", kwarg->field) >= 0))
return false;
if (!display_expr(out, kwarg->expr))
return false;
if (!(fprintf_s(out, ",\n") >= 0))
return false;
iter = iter->next;
}
if (!(fprintf_s(out, "},\n") >= 0))
return false;
if (!(fprintf_s(out, "},\n") >= 0))
return false;
}
return fprintf_s(out, "}") >= 0;
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
case EXPR_LITERAL_VARIANT:
{
return display_literal_variant(out, expr->as.literal_variant.type_name, expr->as.literal_variant.variant_name, expr->as.literal_variant.initializer_list);
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
}
}

bool display_arglist(FILE *out, struct List *list)
{
struct ListNode *node = list->first;
while (node)
{
struct Expr *expr = (struct Expr *) node->value;
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

struct List parse_struct_initializer_list(struct Lexer *lxr)
{
struct List list = list_init();
while (lexer_accept(lxr, TOK_IDENT))
{
char *field = lxr->token;
struct Kwarg *kwarg = malloc(sizeof (*kwarg));
kwarg->field = field;
struct Expr *expr;
if (lexer_accept(lxr, TOK_EQUAL))
{
if (!(expr = parse_expression(lxr)))
todo;
}
else
{
expr = malloc(sizeof (*expr));
expr->tag = EXPR_IDENT;
expr->as.ident.value = field;
}
kwarg->expr = expr;
list_push(&list, (void *) kwarg);
if (!lexer_accept(lxr, TOK_COMMA))
break;
}
return list;
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
char *name = lxr->token;
if (lexer_accept(lxr, TOK_COLON_COLON))
{
char *type_name = name;
lexer_expect(lxr, TOK_IDENT);
char *variant_name = lxr->token;
struct List init_list;
if (lexer_accept(lxr, TOK_OPEN_BRACE))
{
init_list = parse_struct_initializer_list(lxr);
lexer_expect(lxr, TOK_CLOSE_BRACE);
}
else
{
init_list = list_init();
}
expr = malloc(sizeof (*expr));
expr->tag = EXPR_LITERAL_VARIANT;
expr->as.literal_variant.type_name = type_name;
expr->as.literal_variant.variant_name = variant_name;
expr->as.literal_variant.initializer_list = init_list;
return expr;
}
else
{
expr = malloc(sizeof (*expr));
expr->tag = EXPR_IDENT;
expr->as.ident.value = name;
return expr;
}
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


#endif // PARASITIC_C_FILE__3E9
