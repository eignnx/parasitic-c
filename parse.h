#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc, free
#include "lex.h"    // lex, lex_all_input
#include "cheats.h" // todo, fn, fndecl

// FORWARD DECLARATIONS
struct List
{
    struct ListNode *first;
    struct ListNode *last;
};

struct ListNode
{
    void *data;
    struct ListNode *next;
};

struct List list_init()
{
    struct List list;
    list.first = NULL;
    list.last = NULL;
    return list;
}

fn(void list_push(struct List *list, void *data))
{
    struct ListNode *new_node = malloc(sizeof(*new_node));
    new_node->data = data;
    new_node->next = NULL;

    if (list->first && list->last)
    {
        list->last->next = new_node;
        list->last = new_node;
    }
    else
    {
        list->first = new_node;
        list->last = new_node;
    }
}

// #-------------------------------------------------------------------------
// #  Types
// #-------------------------------------------------------------------------

struct StructMember
{
    struct Type *type;
    char *field;
};

fndecl(struct Type *parse_type(struct Lexer *));

// struct_member_list ::= struct_member*
// struct_member ::= type 'ident' ';'
fn(struct List parse_struct_member_list(struct Lexer *lxr))
{
    struct Type *type;
    struct List list = list_init();

    while ((type = parse_type(lxr)))
    {
        lexer_expect(lxr, TOK_IDENT);
        char *field = lxr->token;
        lexer_expect(lxr, TOK_SEMI);

        struct StructMember *sm = malloc(sizeof(*sm));
        sm->type = type;
        sm->field = field;

        list_push(&list, (void *)sm);
    }

    return list;
}

struct Type
{
    enum
    {
        // Atomic types
        TYPE_INT,
        TYPE_BOOL,
        TYPE_CHAR,
        TYPE_VOID,
        TYPE_CSTR_ARR,

        // Compound types
        TYPE_PTR,
        TYPE_NAMED_STRUCT_REF,
        TYPE_NAMED_ENUM_REF,
        TYPE_ANON_STRUCT,
        TYPE_ANON_UNION,
        TYPE_ANON_ENUM,
    } tag;

    union
    {
        struct
        {
            struct Type *child;
        } ptr;

        struct
        {
            char *name;
        } named;

        struct
        {
            struct List struct_members; // A `List` of `StructMember`s.
        } anon_struct_or_union;

        struct
        {
            struct List enum_members; // A `List` of `char *`s.
        } anon_enum;
    } as;
};

fndecl(bool display_struct_members(FILE *, struct List *));
fndecl(bool display_enum_members(FILE *, struct List *));

fn(bool display_type(FILE *out, struct Type *type))
{
    switch (type->tag)
    {
    case TYPE_INT:
        return fprintf_s(out, "int") >= 0;
    case TYPE_BOOL:
        return fprintf_s(out, "bool") >= 0;
    case TYPE_CHAR:
        return fprintf_s(out, "char") >= 0;
    case TYPE_VOID:
        return fprintf_s(out, "void") >= 0;
    case TYPE_CSTR_ARR:
        return fprintf_s(out, "cstr_arr") >= 0;
    case TYPE_PTR:
        return display_type(out, type->as.ptr.child) &&
               fprintf_s(out, "*") >= 0;
    case TYPE_NAMED_STRUCT_REF:
        return fprintf_s(out, "struct %s", type->as.named.name) >= 0;
    case TYPE_NAMED_ENUM_REF:
        return fprintf_s(out, "enum %s", type->as.named.name) >= 0;
    case TYPE_ANON_STRUCT:
        return fprintf_s(out, "struct {\n") >= 0 &&
               display_struct_members(out, &type->as.anon_struct_or_union.struct_members) &&
               fprintf_s(out, "}") >= 0;
    case TYPE_ANON_UNION:
        return fprintf_s(out, "union {\n") >= 0 &&
               display_struct_members(out, &type->as.anon_struct_or_union.struct_members) &&
               fprintf_s(out, "}") >= 0;
    case TYPE_ANON_ENUM:
        return fprintf_s(out, "enum {\n") >= 0 &&
               display_enum_members(out, &type->as.anon_enum.enum_members) &&
               fprintf_s(out, "}") >= 0;
    default:
        perror("Invalid Type tag");
        exit(1);
    }
}

fn(bool display_struct_members(FILE *out, struct List *list))
{
    struct ListNode *node = list->first;

    while (node)
    {
        struct StructMember *sm = (struct StructMember *)node->data;

        bool result = fprintf_s(out, "    ") >= 0 &&
                      display_type(out, sm->type) &&
                      fprintf_s(out, " %s;\n", sm->field) >= 0;

        if (!(result))
            return false;

        node = node->next;
    }

    return true;
}

fn(bool display_enum_members(FILE *out, struct List *list))
{
    struct ListNode *node = list->first;

    while (node)
    {
        char *enum_constant_name = (char *)node->data;
        bool result = fprintf_s(out, "    %s,\n", enum_constant_name) >= 0;

        if (!result)
            return false;

        node = node->next;
    }
    return true;
}

fndecl(struct Type *parse_direct_type(struct Lexer *));

// type ::= direct_type '*'*
fn(struct Type *parse_type(struct Lexer *lxr))
{
    struct Type *type;

    if (!(type = parse_direct_type(lxr)))
        return NULL;

    while (lexer_accept(lxr, TOK_STAR))
    {
        struct Type *new_type = malloc(sizeof(*new_type));
        new_type->tag = TYPE_PTR;
        new_type->as.ptr.child = type;
        type = new_type;
    }
    return type;
}

fndecl(struct List parse_enumerator_list(struct Lexer *));

// direct_type ::= 'void'
//               | 'int'
//               | 'char'
//               | 'bool'
//               | 'cstr_arr' /* Temporary! */
//               | named_struct_ref
//               | named_enum_ref
//               | anonymous_struct_or_union
//               | anonymous_enum
// named_struct_ref ::= 'struct' 'ident'
// named_enum_ref ::= 'enum' 'ident'
// anonymous_struct_or_union ::= ('struct' | 'union') '{' struct_member_list '}'
// anonymous_enum ::= 'enum' '{' enumerator_list '}'
fn(struct Type *parse_direct_type(struct Lexer *lxr))
{
    struct Type *type;

    if (lexer_accept(lxr, TOK_VOID) ||
        lexer_accept(lxr, TOK_CHAR) ||
        lexer_accept(lxr, TOK_INT) ||
        lexer_accept(lxr, TOK_BOOL) ||
        lexer_accept(lxr, TOK_CSTR_ARR))
    {
        type = malloc(sizeof(*type));

        switch (lxr->tok_tag)
        {
        case TOK_VOID:
            type->tag = TYPE_VOID;
            break;
        case TOK_CHAR:
            type->tag = TYPE_CHAR;
            break;
        case TOK_INT:
            type->tag = TYPE_INT;
            break;
        case TOK_BOOL:
            type->tag = TYPE_BOOL;
            break;
        case TOK_CSTR_ARR:
            type->tag = TYPE_CSTR_ARR;
            break;

        default:
            puts("unknown type token!\n");
            exit(1);
        }

        return type;
    }

    if (lexer_accept(lxr, TOK_STRUCT))
    {
        if (lexer_accept(lxr, TOK_IDENT))
        {
            type = malloc(sizeof(*type));
            type->tag = TYPE_NAMED_STRUCT_REF;
            type->as.named.name = lxr->token;
            return type;
        }

        // ANON STRUCT
        if (lexer_expect(lxr, TOK_OPEN_BRACE))
        {
            struct List members = parse_struct_member_list(lxr);
            lexer_expect(lxr, TOK_CLOSE_BRACE);

            type = malloc(sizeof(*type));
            type->tag = TYPE_ANON_STRUCT;
            type->as.anon_struct_or_union.struct_members = members;

            return type;
        }

        return NULL; // Unreachable.
    }

    if (lexer_accept(lxr, TOK_ENUM))
    {
        if (lexer_accept(lxr, TOK_IDENT))
        {
            type = malloc(sizeof(*type));
            type->tag = TYPE_NAMED_ENUM_REF;
            type->as.named.name = lxr->token;
            return type;
        }

        if (lexer_expect(lxr, TOK_OPEN_BRACE))
        {
            struct List members = parse_enumerator_list(lxr);
            lexer_expect(lxr, TOK_CLOSE_BRACE);

            type = malloc(sizeof(*type));
            type->tag = TYPE_ANON_ENUM;
            type->as.anon_enum.enum_members = members;

            return type;
        }

        return NULL; // Unreachable.
    }

    // ANON UNION
    if (lexer_accept(lxr, TOK_UNION))
    {
        lexer_expect(lxr, TOK_OPEN_BRACE);
        struct List members = parse_struct_member_list(lxr);
        lexer_expect(lxr, TOK_CLOSE_BRACE);

        type = malloc(sizeof(*type));
        type->tag = TYPE_ANON_UNION;
        type->as.anon_struct_or_union.struct_members = members;

        return type;
    }

    return NULL;
}

fndecl(struct Item *parse_compiler_directive(struct Lexer *));
fndecl(struct Stmt *parse_var_decl(struct Lexer *));
fndecl(struct List parse_switch_arms(struct Lexer *));
fndecl(struct List parse_stmt_list(struct Lexer *));
fndecl(bool display_stmt_list(FILE *, struct List *));
fndecl(struct Expr *parse_constant(struct Lexer *));
fndecl(struct Expr *parse_expression(struct Lexer *));
fndecl(struct Expr *parse_cast_expression(struct Lexer *));
fndecl(struct Expr *parse_assignment_expression(struct Lexer *));
fndecl(bool display_expr(FILE *, struct Expr *));
fndecl(struct List parse_argument_expression_list(struct Lexer *));
fndecl(bool display_arglist(FILE *, struct List *));
fndecl(bool display_type(FILE *, struct Type *));

// #-------------------------------------------------------------------------
// #  Items
// #-------------------------------------------------------------------------

struct ConstantExpr
{
    enum
    {
        CONST_INT,
        CONST_CSTR,
        CONST_ARRAY,
        CONST_IDENT,
    } tag;

    union
    {
        int const_int;
        char *const_cstr;
        struct List const_array; // A `List` of `ConstExpr*`s.
        char *const_ident;
    } as;
};

fndecl(bool display_constant_expr(FILE *, struct ConstantExpr *));

fn(bool display_const_array(FILE *out, struct List *list))
{
    struct ListNode *node = list->first;

    while (node)
    {
        struct ConstantExpr *expr = (struct ConstantExpr *)node->data;

        if (node->next)
        {
            bool result = display_constant_expr(out, expr) &&
                          fprintf_s(out, ",\n") >= 0;
            if (!result)
                return false;
        }
        else
        {
            bool result = display_constant_expr(out, expr) &&
                          fprintf_s(out, "\n") >= 0;
            if (!result)
                return false;
        }

        node = node->next;
    }
    return true;
}

fn(bool display_constant_expr(FILE *out, struct ConstantExpr *expr))
{
    switch (expr->tag)
    {
    case CONST_INT:
        return fprintf_s(out, "%d", expr->as.const_int) >= 0;
    case CONST_CSTR:
        return fprintf_s(out, "\"%s\"", expr->as.const_cstr) >= 0;
    case CONST_ARRAY:
        return fprintf_s(out, "{\n") >= 0 &&
               display_const_array(out, &expr->as.const_array) &&
               fprintf_s(out, "}") >= 0;
    case CONST_IDENT:
        return fprintf_s(out, "%s", expr->as.const_ident) >= 0;
    }
}

fndecl(struct ConstantExpr *parse_constant_expr(struct Lexer *));

fn(struct List parse_initializer_list(struct Lexer *lxr))
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

fn(struct ConstantExpr *parse_constant_expr(struct Lexer *lxr))
{
    struct ConstantExpr *expr;

    if (lexer_accept(lxr, TOK_LITERAL_INT))
    {
        expr = malloc(sizeof(*expr));
        expr->tag = CONST_INT;
        expr->as.const_int = atoi(lxr->token);
        return expr;
    }

    if (lexer_accept(lxr, TOK_LITERAL_STRING))
    {
        expr = malloc(sizeof(*expr));
        expr->tag = CONST_CSTR;
        expr->as.const_cstr = lxr->token;
        return expr;
    }

    if (lexer_accept(lxr, TOK_OPEN_BRACE))
    {
        struct List initializers = parse_initializer_list(lxr);
        lexer_expect(lxr, TOK_CLOSE_BRACE);

        expr = malloc(sizeof(*expr));
        expr->tag = CONST_ARRAY;
        expr->as.const_array = initializers;
        return expr;
    }

    if (lexer_accept(lxr, TOK_IDENT))
    {
        expr = malloc(sizeof(*expr));
        expr->tag = CONST_IDENT;
        expr->as.const_ident = lxr->token;
        return expr;
    }

    return NULL;
}

struct Item
{
    enum
    {
        ITEM_CONSTANT_DECL,
        ITEM_NAMED_STRUCT_DECL,
        ITEM_NAMED_ENUM_DECL,
        ITEM_POUND_INCLUDE,
    } tag;

    union
    {
        struct
        {
            struct Type *type;
            char *name;
            struct ConstantExpr *initializer;
        } constant_decl;

        struct
        {
            char *name;
            struct List members; // A `List` of `StructMember`s.
        } named_struct_decl;

        struct
        {
            char *name;
            struct List members; // A `List` of `char*`s.
        } named_enum_decl;

        struct
        {
            char *filename;
            enum
            {
                POUND_INCLUDE_ANGLE_BRACKETS,
                POUND_INCLUDE_DOUBLE_QUOTES,
            } kind;
        } pound_include;

    } as;
};

fn(bool display_item(FILE *out, struct Item *item))
{
    switch (item->tag)
    {
    case ITEM_CONSTANT_DECL:
        return display_type(out, item->as.constant_decl.type) &&
               fprintf_s(out, " %s = ", item->as.constant_decl.name) >= 0 &&
               display_constant_expr(out, item->as.constant_decl.initializer) &&
               fprintf_s(out, ";") >= 0;
    case ITEM_NAMED_STRUCT_DECL:
        return fprintf_s(out, "struct %s {\n", item->as.named_struct_decl.name) >= 0 &&
               display_struct_members(out, &item->as.named_struct_decl.members) &&
               fprintf_s(out, "};\n") >= 0;
    case ITEM_NAMED_ENUM_DECL:
        return fprintf_s(out, "enum %s {\n", item->as.named_enum_decl.name) >= 0 &&
               display_enum_members(out, &item->as.named_enum_decl.members) &&
               fprintf_s(out, "};\n") >= 0;
    case ITEM_POUND_INCLUDE:
        if (item->as.pound_include.kind == POUND_INCLUDE_ANGLE_BRACKETS)
        {
            return fprintf_s(out, "#include <%s>", item->as.pound_include.filename) >= 0;
        }
        else
        {
            return fprintf_s(out, "#include \"%s\"", item->as.pound_include.filename) >= 0;
        }
    }
}

// translation_unit ::= item*

// item ::= function_decl
//        | function_def
//        | constant_decl
//        | named_struct_or_union_decl
//        | named_enum_decl
//        | compiler_directive
// constant_decl ::= type 'ident' '=' ( initializer_list | expression ) ';'
// named_struct_decl ::= 'struct' 'ident' '{' struct_member+ '}' ';'
// named_enum_decl ::= 'enum' 'ident' '{' enumerator_list '}' ';'
fn(struct Item *parse_item(struct Lexer *lxr))
{
    struct Item *item;
    struct Type *type;

    // First, try parsing a type.
    if ((type = parse_type(lxr)))
    {
        // NAMED ENUM OR STRUCT DECLs begin with '{' after a Named{Struct,Enum}Ref.
        if (lexer_accept(lxr, TOK_OPEN_BRACE))
        {
            // NAMED STRUCT DECL
            if (type->tag == TYPE_NAMED_STRUCT_REF)
            {
                struct List members = parse_struct_member_list(lxr);
                lexer_expect(lxr, TOK_CLOSE_BRACE);
                lexer_expect(lxr, TOK_SEMI);

                struct Item *item = malloc(sizeof(*item));
                item->tag = ITEM_NAMED_STRUCT_DECL;
                item->as.named_struct_decl.name = type->as.named.name;
                item->as.named_struct_decl.members = members;

                return item;
            }

            // NAMED ENUM DECL
            if (type->tag == TYPE_NAMED_ENUM_REF)
            {
                struct List members = parse_enumerator_list(lxr);
                lexer_expect(lxr, TOK_CLOSE_BRACE);
                lexer_expect(lxr, TOK_SEMI);

                struct Item *item = malloc(sizeof(*item));
                item->tag = ITEM_NAMED_ENUM_DECL;
                item->as.named_enum_decl.name = type->as.named.name;
                item->as.named_enum_decl.members = members;

                return item;
            }

            todo; // Report error.
        }

        // Either a FUNCTION DECL, FUNCTION DEF, or CONSTANT DECL/DEF.
        if (lexer_accept(lxr, TOK_IDENT))
        {
            char *name = lxr->token;

            // CONSTANT DEFINION
            if (lexer_accept(lxr, TOK_EQUAL))
            {
                struct ConstantExpr *initializer;
                if (!(initializer = parse_constant_expr(lxr)))
                    todo; // Report error
                lexer_expect(lxr, TOK_SEMI);

                item = malloc(sizeof(*item));
                item->tag = ITEM_CONSTANT_DECL;
                item->as.constant_decl.type = type;
                item->as.constant_decl.name = name;
                item->as.constant_decl.initializer = initializer;

                return item;
            }

            todo;
        }
    }

    if ((item = parse_compiler_directive(lxr)))
        return item;

    return NULL;
}

// function_decl ::= type 'ident' '(' unnamed_parameter_list? ')' ';'

// function_def ::= type 'ident' '(' parameter_list? ')' stmt_block

// unnamed_parameter_list ::= type (',' type)* ','?

// parameter_list ::= type 'ident' (',' type 'ident')* ','?

// initializer_list ::= '{' '}' | '{' expression (',' expression)* ','? '}'

// enumerator_list ::= <empty string>
//                   | 'ident' (',' 'ident')* ','?
fn(struct List parse_enumerator_list(struct Lexer *lxr))
{
    struct List members = list_init();
    char *member;

    while (lexer_accept(lxr, TOK_IDENT))
    {
        member = lxr->token;
        list_push(&members, (void *)member);

        if (!lexer_accept(lxr, TOK_COMMA))
            return members;
    }

    return members;
}

// compiler_directive ::= '#' 'include' (angle_bracket_filename | double_quoted_filename)
fn(struct Item *parse_compiler_directive(struct Lexer *lxr))
{
    if (!lexer_accept(lxr, TOK_POUND))
        return NULL;

    if (!lexer_expect(lxr, TOK_INCLUDE))
        return NULL;

    if (!(lexer_accept(lxr, TOK_ANGLE_BRACK_FILENAME) || lexer_accept(lxr, TOK_LITERAL_STRING)))
        return NULL;

    struct Item *decl = malloc(sizeof(*decl));
    decl->tag = ITEM_POUND_INCLUDE;
    decl->as.pound_include.filename = lxr->token;

    if (lxr->tok_tag == TOK_ANGLE_BRACK_FILENAME)
        decl->as.pound_include.kind = POUND_INCLUDE_ANGLE_BRACKETS;
    else
        decl->as.pound_include.kind = POUND_INCLUDE_DOUBLE_QUOTES;

    return decl;
}

// #-------------------------------------------------------------------------
// #  A.2.3  Statements
// #-------------------------------------------------------------------------

struct Stmt
{
    enum
    {
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

    union
    {
        struct
        {
            struct Expr *cond;
            struct Stmt *consequent;
            struct Stmt *alternative; // Nullable ptr.
        } if_stmt;

        struct
        {
            struct Expr *cond;
            struct Stmt *body;
        } while_stmt;

        struct
        {
            struct Expr *scrutinee;
            struct List switch_arms; // A `List` of `SwitchArm*`s.
        } switch_stmt;

        struct
        {
            struct List stmts; // A `List` of `Stmt*`s
        } block;

        struct
        {
            struct Expr *expr; // Nullable ptr.
        } ret;

        struct
        {
            struct Type *type;
            char *name;
            struct Expr *initializer; // Nullable ptr.
        } var_decl;

        struct
        {
            struct Expr *expr;
        } expression;

    } as;
};

struct SwitchArm
{
    struct Expr *test; // Nullable. NULL indicates `default` arm.
    struct List stmts; // A `List` of `Stmts*`s.
};

fn(bool display_switch_arms(FILE *out, struct List *arms))
{
    struct ListNode *node = arms->first;

    while (node)
    {
        struct SwitchArm *arm = (struct SwitchArm *)node->data;

        // If `test` is NULL, this is a `default` arm.
        if (arm->test == NULL)
        {
            bool result = fprintf_s(out, "default:\n") >= 0 &&
                          display_stmt_list(out, &arm->stmts);
            if (!result)
                return false;
        }
        else
        {
            bool result = fprintf_s(out, "case ") >= 0 &&
                          display_expr(out, arm->test) &&
                          fprintf_s(out, ":\n") >= 0 &&
                          display_stmt_list(out, &arm->stmts);
            if (!result)
                return false;
        }

        node = node->next;
    }

    return true;
}

fn(bool display_stmt(FILE *out, struct Stmt *stmt))
{
    switch (stmt->tag)
    {
    case STMT_BLOCK:
        return fprintf_s(out, "{\n") >= 0 &&
               display_stmt_list(out, &stmt->as.block.stmts) &&
               fprintf_s(out, "}\n") >= 0;
    case STMT_IF:
        if (stmt->as.if_stmt.alternative)
        {
            return fprintf_s(out, "if (") >= 0 &&
                   display_expr(out, stmt->as.if_stmt.cond) &&
                   fprintf_s(out, ")\n") >= 0 &&
                   display_stmt(out, stmt->as.if_stmt.consequent) &&
                   fprintf_s(out, "else\n") >= 0 &&
                   display_stmt(out, stmt->as.if_stmt.alternative);
        }
        else
        {
            return fprintf_s(out, "if (") >= 0 &&
                   display_expr(out, stmt->as.if_stmt.cond) &&
                   fprintf_s(out, ")\n") >= 0 &&
                   display_stmt(out, stmt->as.if_stmt.consequent);
        }
    case STMT_WHILE:
        return fprintf_s(out, "while (") >= 0 &&
               display_expr(out, stmt->as.while_stmt.cond) &&
               fprintf_s(out, ")\n") >= 0 &&
               display_stmt(out, stmt->as.while_stmt.body);
    case STMT_SWITCH:
        return fprintf_s(out, "switch (") >= 0 &&
               display_expr(out, stmt->as.switch_stmt.scrutinee) &&
               fprintf_s(out, ")\n{\n") >= 0 &&
               display_switch_arms(out, &stmt->as.switch_stmt.switch_arms) &&
               fprintf_s(out, "}\n") >= 0;
    case STMT_BREAK:
        return fprintf_s(out, "break;\n") >= 0;
    case STMT_CONTINUE:
        return fprintf_s(out, "continue;\n") >= 0;
    case STMT_RETURN:
        if (stmt->as.ret.expr)
        {
            return fprintf_s(out, "return ") >= 0 &&
                   display_expr(out, stmt->as.ret.expr) &&
                   fprintf_s(out, ";\n") >= 0;
        }
        else
        {
            return fprintf_s(out, "return;\n") >= 0;
        }
    case STMT_VAR_DECL:
        if (stmt->as.var_decl.initializer)
        {
            return display_type(out, stmt->as.var_decl.type) &&
                   fprintf_s(out, " %s = ", stmt->as.var_decl.name) >= 0 &&
                   display_expr(out, stmt->as.var_decl.initializer) &&
                   fprintf_s(out, ";\n") >= 0;
        }
        else
        {
            return display_type(out, stmt->as.var_decl.type) &&
                   fprintf_s(out, " %s;\n", stmt->as.var_decl.name) >= 0;
        }
    case STMT_EXPRESSION:
        return display_expr(out, stmt->as.expression.expr) &&
               fprintf_s(out, ";\n") >= 0;
    }
}

fn(bool display_stmt_list(FILE *out, struct List *list))
{
    struct ListNode *node = list->first;

    while (node)
    {
        struct Stmt *stmt = (struct Stmt *)node->data;
        bool result = display_stmt(out, stmt);

        if (!result)
            return false;

        node = node->next;
    }

    return true;
}

// stmt ::= stmt_block
//        | if_stmt
//        | while_stmt
//        | switch_stmt
//        | jump_stmt
//        | return_stmt
//        | var_decl
//        | expr_stmt
// stmt_block ::= '{' stmt_list '}'
// if_stmt ::= 'if' '(' expression ')' stmt ('else' stmt)?
// while_stmt ::= 'while' '(' expression ')' stmt
// switch_stmt ::= 'switch' '(' expression ')' '{' switch_arms '}'
// switch_arms ::= switch_arm*
// switch_arm ::= ( 'case' expression | 'default' ) ':' stmt*
// jump_stmt ::= ('break' | 'continue') ';'
// return_stmt ::= 'return' expression? ';'
// expr_stmt ::= expression ';'
fn(struct Stmt *parse_stmt(struct Lexer *lxr))
{
    struct Stmt *stmt;

    if (lexer_accept(lxr, TOK_OPEN_BRACE))
    {
        struct List stmts = parse_stmt_list(lxr);
        lexer_expect(lxr, TOK_CLOSE_BRACE);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_BLOCK;
        stmt->as.block.stmts = stmts;

        return stmt;
    }

    // IF STMT
    if (lexer_accept(lxr, TOK_IF))
    {
        lexer_expect(lxr, TOK_OPEN_PAREN);
        struct Expr *cond = parse_expression(lxr);
        lexer_expect(lxr, TOK_CLOSE_PAREN);
        struct Stmt *consequent = parse_stmt(lxr);

        struct Stmt *alternative = NULL;
        if (lexer_accept(lxr, TOK_ELSE))
            if (!(alternative = parse_stmt(lxr)))
                return NULL; // TODO: report error here

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_IF;
        stmt->as.if_stmt.cond = cond;
        stmt->as.if_stmt.consequent = consequent;
        stmt->as.if_stmt.alternative = alternative;

        return stmt;
    }

    // WHILE STMT
    if (lexer_accept(lxr, TOK_WHILE))
    {
        lexer_expect(lxr, TOK_OPEN_PAREN);
        struct Expr *cond = parse_expression(lxr);
        lexer_expect(lxr, TOK_CLOSE_PAREN);
        struct Stmt *body = parse_stmt(lxr);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_WHILE;
        stmt->as.while_stmt.cond = cond;
        stmt->as.while_stmt.body = body;

        return stmt;
    }

    // SWITCH STMT
    if (lexer_accept(lxr, TOK_SWITCH))
    {
        lexer_expect(lxr, TOK_OPEN_PAREN);
        struct Expr *scrutinee = parse_expression(lxr);
        lexer_expect(lxr, TOK_CLOSE_PAREN);
        lexer_expect(lxr, TOK_OPEN_BRACE);
        struct List arms = parse_switch_arms(lxr);
        lexer_expect(lxr, TOK_CLOSE_BRACE);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_SWITCH;
        stmt->as.switch_stmt.scrutinee = scrutinee;
        stmt->as.switch_stmt.switch_arms = arms;

        return stmt;
    }

    // BREAK STMT
    if (lexer_accept(lxr, TOK_BREAK))
    {
        lexer_expect(lxr, TOK_SEMI);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_BREAK;

        return stmt;
    }

    // CONTINUE STMT
    if (lexer_accept(lxr, TOK_CONTINUE))
    {
        lexer_expect(lxr, TOK_SEMI);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_CONTINUE;

        return stmt;
    }

    // RETURN STMT
    if (lexer_accept(lxr, TOK_RETURN))
    {
        struct Expr *expr = NULL;
        expr = parse_expression(lxr);
        lexer_expect(lxr, TOK_SEMI);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_RETURN;
        stmt->as.ret.expr = expr;

        return stmt;
    }

    // VARIABLE DECLARATION STMT
    if ((stmt = parse_var_decl(lxr)))
        return stmt;

    // EXPRESSION STMT
    struct Expr *expr;
    if ((expr = parse_expression(lxr)))
    {
        lexer_expect(lxr, TOK_SEMI);

        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_EXPRESSION;
        stmt->as.expression.expr = expr;

        return stmt;
    }

    return NULL;
}

// stmt_list ::= stmt*
fn(struct List parse_stmt_list(struct Lexer *lxr))
{
    struct List stmts = list_init();
    struct Stmt *stmt;

    while ((stmt = parse_stmt(lxr)))
        list_push(&stmts, (void *)stmt);

    return stmts;
}

// var_decl ::= type 'ident' ('=' expression)? ';'
fn(struct Stmt *parse_var_decl(struct Lexer *lxr))
{
    struct Type *type;

    if (!(type = parse_type(lxr)))
        return NULL;

    lexer_expect(lxr, TOK_IDENT);
    char *name = lxr->token;

    struct Expr *initializer = NULL;

    if (lexer_accept(lxr, TOK_EQUAL))
    {
        if (!(initializer = parse_expression(lxr)))
            return NULL;
    }

    lexer_expect(lxr, TOK_SEMI);

    struct Stmt *stmt = malloc(sizeof(*stmt));
    stmt->tag = STMT_VAR_DECL;
    stmt->as.var_decl.type = type;
    stmt->as.var_decl.name = name;
    stmt->as.var_decl.initializer = initializer;

    return stmt;
}

fn(struct List parse_switch_arms(struct Lexer *lxr))
{
    struct List arms = list_init();

    while (lexer_accept(lxr, TOK_CASE) || lexer_accept(lxr, TOK_DEFAULT))
    {
        struct SwitchArm *arm;

        if (lxr->tok_tag == TOK_CASE)
        {
            struct Expr *test;

            if (!(test = parse_expression(lxr)))
                todo; // Report the error.

            lexer_expect(lxr, TOK_COLON);

            struct List stmts = parse_stmt_list(lxr);

            arm = malloc(sizeof(*arm));
            arm->test = test;
            arm->stmts = stmts;

            list_push(&arms, (void *)arm);
        }
        else if (lxr->tok_tag == TOK_DEFAULT)
        {
            lexer_expect(lxr, TOK_COLON);

            struct List stmts = parse_stmt_list(lxr);

            arm = malloc(sizeof(*arm));
            arm->test = NULL; // NULL indicates this is a `default` arm.
            arm->stmts = stmts;

            list_push(&arms, (void *)arm);
        }
    }

    return arms;
}

// #-------------------------------------------------------------------------
// #  A.2.1  Expressions
// #-------------------------------------------------------------------------

struct Expr
{
    enum
    {
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

    union
    {
        struct
        {
            int value;
        } literal_int;

        struct
        {
            char *value;
        } literal_char;

        struct
        {
            char *value;
        } literal_string;

        struct
        {
            bool value;
        } literal_bool;

        struct
        {
            char *value;
        } ident;

        struct
        {
            struct Expr *child;
        } parenthesized;

        struct
        {
            struct Expr *arr;
            struct Expr *index;
        } arr_index;

        struct
        {
            struct Expr *callable;
            struct List args;
        } call;

        struct
        {
            struct Expr *object;
            char *field;
        } field_access;

        struct
        {
            struct Expr *operand;
        } postfix_plus_plus;

        struct
        {
            struct Expr *operand;
        } unary_op;

        struct
        {
            struct Expr *operand;
        } sizeof_expr;

        struct
        {
            struct Type *type;
        } sizeof_type;

        struct
        {
            struct Type *type;
            struct Expr *expr;
        } cast;

        struct
        {
            struct Expr *x;
            struct Expr *y;
        } binary_op;

    } as;
};

fn(bool display_bin_op(FILE *out, char *op, struct Expr *x, struct Expr *y))
{
    return display_expr(out, x) &&
           fprintf_s(out, " %s ", op) >= 0 &&
           display_expr(out, y);
}

fn(bool display_expr(FILE *out, struct Expr *expr))
{
    switch (expr->tag)
    {
    case EXPR_LITERAL_INT:
        return fprintf_s(out, "%d", expr->as.literal_int.value) >= 0;
    case EXPR_LITERAL_CHAR:
        return fprintf_s(out, "'%s'", expr->as.literal_char.value) >= 0;
    case EXPR_LITERAL_STRING:
        return fprintf_s(out, "\"%s\"", expr->as.literal_string.value) >= 0;
    case EXPR_LITERAL_BOOL:
        if (expr->as.literal_bool.value)
            return fprintf_s(out, "true") >= 0;
        else
            return fprintf_s(out, "false") >= 0;
    case EXPR_IDENT:
        return fprintf_s(out, "%s", expr->as.ident.value) >= 0;
    case EXPR_PARENTHESIZED:
        return fprintf_s(out, "(") >= 0 &&
               display_expr(out, expr->as.parenthesized.child) &&
               fprintf_s(out, ")") >= 0;
    case EXPR_ARR_INDEX:
        return display_expr(out, expr->as.arr_index.arr) &&
               fprintf_s(out, "[") >= 0 &&
               display_expr(out, expr->as.arr_index.index) &&
               fprintf_s(out, "]") >= 0;
    case EXPR_CALL:
        return display_expr(out, expr->as.call.callable) &&
               fprintf_s(out, "(") >= 0 &&
               display_arglist(out, &expr->as.call.args) &&
               fprintf_s(out, ")") >= 0;
    case EXPR_DOT_FIELD_ACCESS:
        return display_expr(out, expr->as.field_access.object) &&
               fprintf_s(out, ".%s", expr->as.field_access.field) >= 0;
    case EXPR_ARROW_FIELD_ACCESS:
        return display_expr(out, expr->as.field_access.object) &&
               fprintf_s(out, "->%s", expr->as.field_access.field) >= 0;
    case EXPR_POSTFIX_PLUS_PLUS:
        return display_expr(out, expr->as.postfix_plus_plus.operand) &&
               fprintf_s(out, "++") >= 0;
    case EXPR_REFERENCE:
        return fprintf_s(out, "&") >= 0 &&
               display_expr(out, expr->as.unary_op.operand);
    case EXPR_DEREFERENCE:
        return fprintf_s(out, "*") >= 0 &&
               display_expr(out, expr->as.unary_op.operand);
    case EXPR_NEGATION:
        return fprintf_s(out, "!") >= 0 &&
               display_expr(out, expr->as.unary_op.operand);
    case EXPR_SIZEOF_EXPR:
        return fprintf_s(out, "sizeof ") >= 0 &&
               display_expr(out, expr->as.sizeof_expr.operand);
    case EXPR_SIZEOF_TYPE:
        return fprintf_s(out, "sizeof(") >= 0 &&
               display_type(out, expr->as.sizeof_type.type) &&
               fprintf_s(out, ")") >= 0;
    case EXPR_CAST:
        return fprintf_s(out, "(") >= 0 &&
               display_type(out, expr->as.cast.type) &&
               fprintf_s(out, ") ") >= 0 &&
               display_expr(out, expr->as.cast.expr);
    case EXPR_ADD:
        return display_bin_op(out, "+", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_SUB:
        return display_bin_op(out, "-", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_GT:
        return display_bin_op(out, ">", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_GTE:
        return display_bin_op(out, ">=", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_LT:
        return display_bin_op(out, "<", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_LTE:
        return display_bin_op(out, "<=", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_EQUAL:
        return display_bin_op(out, "==", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_NOT_EQUAL:
        return display_bin_op(out, "!=", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_AND:
        return display_bin_op(out, "&&", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_OR:
        return display_bin_op(out, "||", expr->as.binary_op.x, expr->as.binary_op.y);
    case EXPR_ASSIGN:
        return display_bin_op(out, "=", expr->as.binary_op.x, expr->as.binary_op.y);
    default:
        printf("display_expr is not implemented for EXPR tag %d!\n", expr->tag);
        exit(1);
    }
}

fn(bool display_arglist(FILE *out, struct List *list))
{
    struct ListNode *node = list->first;

    while (node)
    {
        struct Expr *expr = (struct Expr *)node->data;

        if (node->next)
        {
            bool result = display_expr(out, expr) &&
                          fprintf_s(out, ", ") >= 0;

            if (!result)
                return false;

            node = node->next;
        }
        else
        {
            // Don't display the comma.
            return display_expr(out, expr);
        }
    }

    return true;
}

// PrimaryExpression
//    <- StringLiteral
//     / Constant
//     / Identifier
//     / LPAR Expression RPAR
fn(struct Expr *parse_primary_expression(struct Lexer *lxr))
{
    struct Expr *expr;

    if (lexer_accept(lxr, TOK_LITERAL_STRING))
    {
        expr = malloc(sizeof(*expr));
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
        expr = malloc(sizeof(*expr));
        expr->tag = EXPR_IDENT;
        expr->as.ident.value = lxr->token;
        return expr;
    }

    if (lexer_accept(lxr, TOK_OPEN_PAREN) &&
        (expr = parse_expression(lxr)) &&
        lexer_expect(lxr, TOK_CLOSE_PAREN))
    {
        // NOTE: We need to store parenthesized exprs as their own AST node so
        // that they can be printed into the output file without changing
        // precedence of operations.
        struct Expr *paren_expr = malloc(sizeof(*paren_expr));
        paren_expr->tag = EXPR_PARENTHESIZED;
        paren_expr->as.parenthesized.child = expr;
        return paren_expr;
    }

    return NULL;
}

// PostfixExpression
//    <- ( PrimaryExpression
//       / LPAR TypeName RPAR LWING InitializerList COMMA? RWING  # :TODO: parse compound literals https://en.cppreference.com/w/c/language/compound_literal
//       )
//       ( LBRK Expression RBRK
//       / LPAR ArgumentExpressionList? RPAR
//       / DOT Identifier
//       / PTR Identifier
//       / INC
//       / DEC          # :IGNORED
//       )*
fn(struct Expr *parse_postfix_expression(struct Lexer *lxr))
{
    struct Expr *expr;

    if (!(expr = parse_primary_expression(lxr)))
        return NULL;

    while (true)
    {

        // ARRAY INDEX EXPRESSION
        struct Expr *index;
        if (lexer_accept(lxr, TOK_OPEN_BRACK) &&
            (index = parse_expression(lxr)) &&
            lexer_expect(lxr, TOK_CLOSE_BRACK))
        {
            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_ARR_INDEX;
            new_expr->as.arr_index.arr = expr;
            new_expr->as.arr_index.index = index;

            expr = new_expr;
            continue;
        }

        // FUNCTION CALL
        if (lexer_accept(lxr, TOK_OPEN_PAREN))
        {
            struct List arg_list = parse_argument_expression_list(lxr);
            lexer_expect(lxr, TOK_CLOSE_PAREN);

            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_CALL;
            new_expr->as.call.callable = expr;
            new_expr->as.call.args = arg_list;

            expr = new_expr;
            continue;
        }

        // DOT FIELD ACCESS
        if (lexer_accept(lxr, TOK_DOT) &&
            lexer_expect(lxr, TOK_IDENT))
        {
            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_DOT_FIELD_ACCESS;
            new_expr->as.field_access.object = expr;
            new_expr->as.field_access.field = lxr->token;

            expr = new_expr;
            continue;
        }

        // ARROW FIELD ACCESS
        if (lexer_accept(lxr, TOK_ARROW) &&
            lexer_expect(lxr, TOK_IDENT))
        {
            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_ARROW_FIELD_ACCESS;
            new_expr->as.field_access.object = expr;
            new_expr->as.field_access.field = lxr->token;

            expr = new_expr;
            continue;
        }

        // POSTFIX PLUS PLUS
        if (lexer_accept(lxr, TOK_PLUS_PLUS))
        {
            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_POSTFIX_PLUS_PLUS;
            new_expr->as.postfix_plus_plus.operand = expr;

            expr = new_expr;
            continue;
        }

        break;
    }
    return expr;
}

// ArgumentExpressionList
//      <- <empty string>
//       / AssignmentExpression (COMMA AssignmentExpression)*
fn(struct List parse_argument_expression_list(struct Lexer *lxr))
{
    struct Expr *expr;
    struct List list = list_init();

    if (!(expr = parse_assignment_expression(lxr)))
        return list;

    list_push(&list, (void *)expr);

    while (lexer_accept(lxr, TOK_COMMA))
    {
        if (!(expr = parse_assignment_expression(lxr)))
            break;

        list_push(&list, (void *)expr);
    }

    return list;
}

// UnaryExpression
//    <- PostfixExpression
//     / INC UnaryExpression # :IGNORED
//     / DEC UnaryExpression # :IGNORED
//     / UnaryOperator CastExpression
//     / SIZEOF (UnaryExpression / LPAR TypeName RPAR )
//
// UnaryOperator
//    <- AND
//     / STAR
//     / PLUS # :IGNORED
//     / MINUS # :IGNORED
//     / TILDA # :IGNORED
//     / BANG
//
// TODO: remove backtracking by rewriting grammar slightly:
//   unary_expression -> 'sizeof' unary_expression'
//                     | 'constant'
//                     | '(' expression ')'
//  unary_expression' -> 'sizeof' unary_expression'
//                     | 'constant'
//                     | '(' expression ')'
//                     | '(' type_name ')'
fn(struct Expr *parse_unary_expression(struct Lexer *lxr))
{
    struct Expr *expr;

    if ((expr = parse_postfix_expression(lxr)))
        return expr;

    if (lexer_accept(lxr, TOK_AMPERSAND))
    {
        struct Expr *operand;
        if (!(operand = parse_cast_expression(lxr)))
            return NULL;

        expr = malloc(sizeof(*expr));
        expr->tag = EXPR_REFERENCE;
        expr->as.unary_op.operand = operand;
        return expr;
    }

    if (lexer_accept(lxr, TOK_STAR))
    {
        struct Expr *operand;
        if (!(operand = parse_cast_expression(lxr)))
            return NULL;

        expr = malloc(sizeof(*expr));
        expr->tag = EXPR_DEREFERENCE;
        expr->as.unary_op.operand = operand;
        return expr;
    }

    if (lexer_accept(lxr, TOK_BANG))
    {
        struct Expr *operand;
        if (!(operand = parse_cast_expression(lxr)))
            return NULL;

        expr = malloc(sizeof(*expr));
        expr->tag = EXPR_NEGATION;
        expr->as.unary_op.operand = operand;
        return expr;
    }

    if (lexer_accept(lxr, TOK_SIZEOF))
    {
        struct Expr *operand;
        {
            struct Lexer saved_lxr = *lxr; // HACK: save lexer state
            if ((operand = parse_unary_expression(lxr)))
            {
                expr = malloc(sizeof(*expr));
                expr->tag = EXPR_SIZEOF_EXPR;
                expr->as.sizeof_expr.operand = operand;
                return expr;
            }
            *lxr = saved_lxr; // HACK: restore lexer state
        }

        struct Type *type;
        if (lexer_accept(lxr, TOK_OPEN_PAREN) &&
            (type = parse_type(lxr)) &&
            lexer_expect(lxr, TOK_CLOSE_PAREN))
        {
            expr = malloc(sizeof(*expr));
            expr->tag = EXPR_SIZEOF_TYPE;
            expr->as.sizeof_type.type = type;
            return expr;
        }

        return NULL;
    }

    return NULL;
}

// CastExpression
//      <- (LPAR TypeName RPAR CastExpression)
//       / UnaryExpression
//
// TODO: remove backtracking by rewriting grammar slightly:
//    cast_expression -> '(' type_name ')' cast_expression
//                     | 'sizeof' unary_expression'
//                     | 'constant'
//                     | '(' expression ')'
//  unary_expression' -> 'sizeof' unary_expression'
//                     | 'constant'
//                     | '(' expression ')'
//                     | '(' type_name ')'
fn(struct Expr *parse_cast_expression(struct Lexer *lxr))
{
    {
        struct Lexer saved_lxr = *lxr; // HACK: is there a better way?
        {
            struct Type *type;
            struct Expr *expr;
            if (lexer_accept(lxr, TOK_OPEN_PAREN) &&
                (type = parse_type(lxr)) &&
                lexer_accept(lxr, TOK_CLOSE_PAREN) &&
                (expr = parse_cast_expression(lxr)))
            {
                struct Expr *cast_expr = malloc(sizeof(*cast_expr));
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

// MultiplicativeExpression
//      <- CastExpression ((STAR / DIV / MOD) CastExpression)*
fn(struct Expr *parse_multiplicative_expression(struct Lexer *lxr))
{
    // TODO: impl multiplication, division, modulo
    return parse_cast_expression(lxr);
}

// AdditiveExpression
//      <- MultiplicativeExpression ((PLUS / MINUS) MultiplicativeExpression)*
fn(struct Expr *parse_additive_expression(struct Lexer *lxr))
{
    struct Expr *accum;

    if (!(accum = parse_multiplicative_expression(lxr)))
        return NULL;

    while (lexer_accept(lxr, TOK_PLUS) || lexer_accept(lxr, TOK_MINUS))
    {
        struct Expr *y;

        // Save the token type.
        enum TokTag op = lxr->tok_tag;

        if (!(y = parse_multiplicative_expression(lxr)))
        {
            printf("ERROR: expected expression after `%s`, got nothing\n", tok_tag_names[op]);
            exit(1);
        }

        struct Expr *new_expr = malloc(sizeof(*new_expr));
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

// ShiftExpression
//      <- AdditiveExpression ((LEFT / RIGHT) AdditiveExpression)*
fn(struct Expr *parse_shift_expression(struct Lexer *lxr))
{
    return parse_additive_expression(lxr);
}

// RelationalExpression
//      <- ShiftExpression ((LE / GE / LT / GT) ShiftExpression)*
fn(struct Expr *parse_relational_expression(struct Lexer *lxr))
{
    struct Expr *accum;

    if (!(accum = parse_shift_expression(lxr)))
        return NULL;

    while (lexer_accept(lxr, TOK_LTE) ||
           lexer_accept(lxr, TOK_GTE) ||
           lexer_accept(lxr, TOK_LT) ||
           lexer_accept(lxr, TOK_GT))
    {
        struct Expr *y;

        // Save the token type.
        enum TokTag op = lxr->tok_tag;

        if (!(y = parse_shift_expression(lxr)))
        {
            printf("ERROR: expected expression after `%s`, got nothing\n", tok_tag_names[op]);
            exit(1);
        }

        struct Expr *new_expr = malloc(sizeof(*new_expr));
        new_expr->as.binary_op.x = accum;
        new_expr->as.binary_op.y = y;
        if (op == TOK_LTE)
            new_expr->tag = EXPR_LTE;
        else if (op == TOK_GTE)
            new_expr->tag = EXPR_GTE;
        else if (op == TOK_LT)
            new_expr->tag = EXPR_LT;
        else if (op == TOK_GT)
            new_expr->tag = EXPR_GT;

        accum = new_expr;
    }

    return accum;
}

// EqualityExpression
//      <- RelationalExpression ((EQUEQU / BANGEQU) RelationalExpression)*
fn(struct Expr *parse_equality_expression(struct Lexer *lxr))
{
    struct Expr *accum;

    if (!(accum = parse_relational_expression(lxr)))
        return NULL;

    while (lexer_accept(lxr, TOK_EQUAL_EQUAL) ||
           lexer_accept(lxr, TOK_NOT_EQUAL))
    {
        struct Expr *y;

        // Save the token type.
        enum TokTag op = lxr->tok_tag;

        if (!(y = parse_relational_expression(lxr)))
        {
            printf("ERROR: expected expression after `%s`, got nothing\n", tok_tag_names[op]);
            exit(1);
        }

        struct Expr *new_expr = malloc(sizeof(*new_expr));
        new_expr->as.binary_op.x = accum;
        new_expr->as.binary_op.y = y;
        if (op == TOK_EQUAL_EQUAL)
            new_expr->tag = EXPR_EQUAL;
        else if (op == TOK_NOT_EQUAL)
            new_expr->tag = EXPR_NOT_EQUAL;

        accum = new_expr;
    }

    return accum;
}

// :IGNORED
// ANDExpression <- EqualityExpression (AND EqualityExpression)*

// :IGNORED
// ExclusiveORExpression <- ANDExpression (HAT ANDExpression)*

// :IGNORED
// InclusiveORExpression <- ExclusiveORExpression (OR ExclusiveORExpression)*

// LogicalANDExpression
//      <- InclusiveORExpression (ANDAND InclusiveORExpression)*
fn(struct Expr *parse_logical_and_expression(struct Lexer *lxr))
{
    struct Expr *accum;

    if (!(accum = parse_equality_expression(lxr)))
        return NULL;

    while (lexer_accept(lxr, TOK_AND))
    {
        struct Expr *y;

        if (!(y = parse_equality_expression(lxr)))
        {
            printf("ERROR: expected expression after `%s`, got nothing\n", tok_tag_names[TOK_AND]);
            exit(1);
        }

        struct Expr *new_expr = malloc(sizeof(*new_expr));
        new_expr->tag = EXPR_AND;
        new_expr->as.binary_op.x = accum;
        new_expr->as.binary_op.y = y;

        accum = new_expr;
    }

    return accum;
}

// LogicalORExpression
//      <- LogicalANDExpression (OROR LogicalANDExpression)*
fn(struct Expr *parse_logical_or_expression(struct Lexer *lxr))
{
    struct Expr *accum;

    if (!(accum = parse_logical_and_expression(lxr)))
        return NULL;

    while (lexer_accept(lxr, TOK_OR))
    {
        struct Expr *y;

        if (!(y = parse_logical_and_expression(lxr)))
        {
            printf("ERROR: expected expression after `%s`, got nothing\n", tok_tag_names[TOK_OR]);
            exit(1);
        }

        struct Expr *new_expr = malloc(sizeof(*new_expr));
        new_expr->tag = EXPR_OR;
        new_expr->as.binary_op.x = accum;
        new_expr->as.binary_op.y = y;

        accum = new_expr;
    }

    return accum;
}

// ConditionalExpression
//      <- LogicalORExpression (QUERY Expression COLON LogicalORExpression)* # :IGNORED
fn(struct Expr *parse_conditional_expression(struct Lexer *lxr))
{
    return parse_logical_or_expression(lxr);
}

// AssignmentExpression
//    <- UnaryExpression AssignmentOperator AssignmentExpression
//     / ConditionalExpression
//
// AssignmentOperator
//    <- EQU
//     / STAREQU  # :IGNORED
//     / DIVEQU   # :IGNORED
//     / MODEQU   # :IGNORED
//     / PLUSEQU  # :IGNORED
//     / MINUSEQU # :IGNORED
//     / LEFTEQU  # :IGNORED
//     / RIGHTEQU # :IGNORED
//     / ANDEQU   # :IGNORED
//     / HATEQU   # :IGNORED
//     / OREQU    # :IGNORED
//
// TODO: remove backtracking by left factoring grammar:
//  assignment_expression -> 'sizeof' unary_expression' '=' assignment_expression
//                         | 'constant' '=' assignment_expression
//                         | '(' expression ')' '=' assignment_expression
//                         | conditional_expression
// conditional_expression -> '(' type_name ')' cast_expression
//                         | 'sizeof' unary_expression'
//                         | 'constant'
//                         | '(' expression ')'
//      unary_expression' -> 'sizeof' unary_expression'
//                         | 'constant'
//                         | '(' expression ')'
//                         | '(' type_name ')'
fn(struct Expr *parse_assignment_expression(struct Lexer *lxr))
{
    struct Expr *unary, *assignment;

    {
        struct Lexer saved_lxr = *lxr; // HACK: backtracking hack
        if ((unary = parse_unary_expression(lxr)) &&
            lexer_accept(lxr, TOK_EQUAL) &&
            (assignment = parse_assignment_expression(lxr)))
        {
            struct Expr *expr = malloc(sizeof(*expr));
            expr->tag = EXPR_ASSIGN;
            expr->as.binary_op.x = unary;
            expr->as.binary_op.y = assignment;
            return expr;
        }
        *lxr = saved_lxr;
    }

    return parse_conditional_expression(lxr);
}

// Expression
//      <- AssignmentExpression (COMMA AssignmentExpression)* # :IGNORED
fn(struct Expr *parse_expression(struct Lexer *lxr))
{
    return parse_assignment_expression(lxr);
}

// #-------------------------------------------------------------------------
// #  A.1.5  Constants
// #-------------------------------------------------------------------------

// Constant
//    <- FloatConstant         # :TODO
//     / IntegerConstant       # Note: can be a prefix of Float Constant!
//     / EnumerationConstant   # :IGNORED
//     / BooleanConstant       # :NEW
//     / CharacterConstant
fn(struct Expr *parse_constant(struct Lexer *lxr))
{
    if (lexer_accept(lxr, TOK_LITERAL_INT))
    {
        struct Expr *expr = malloc(sizeof(*expr));
        expr->tag = EXPR_LITERAL_INT;
        expr->as.literal_int.value = atoi(lxr->token);
        return expr;
    }

    if (lexer_accept(lxr, TOK_TRUE) || lexer_accept(lxr, TOK_FALSE))
    {
        struct Expr *expr = malloc(sizeof(*expr));
        expr->tag = EXPR_LITERAL_BOOL;
        expr->as.literal_bool.value = (lxr->tok_tag == TOK_TRUE);
        return expr;
    }

    if (lexer_accept(lxr, TOK_LITERAL_CHAR))
    {
        struct Expr *expr = malloc(sizeof(*expr));
        expr->tag = EXPR_LITERAL_CHAR;
        expr->as.literal_char.value = lxr->token;
        return expr;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TESTS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

fn(void test_expr(char *input))
{
    struct Lexer lxr;
    struct Expr *expr;

    printf("\n");
    lxr = lexer_init(input);
    if ((expr = parse_expression(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
        display_expr(stdout, expr);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

fn(void test_parse_exprs())
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
    test_expr("  100 >= 0 ");
    test_expr("  100 != 0 ");
    test_expr("  100 != 0 && -4 < -3  ");
    test_expr("  100 != 0 || -4 < -3  ");
    test_expr("  **ptr_ptr = *ptr = 123  ");
}

fn(void test_type(char *input))
{
    struct Lexer lxr;
    struct Type *type;

    printf("\n");
    lxr = lexer_init(input);
    if ((type = parse_type(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
        display_type(stdout, type);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

fn(void test_parse_types())
{
    printf("\n\n----------------------TEST TYPES-----------------------------");
    test_type("  int  ");
    test_type("  bool  ");
    test_type("  char  ");
    test_type("  void  ");
    test_type("  cstr_arr  ");
    test_type("  cstr_arr  ");
    test_type("  char**  ");
    test_type("  struct Expr  ");
    test_type("  enum Tag  ");
    test_type("  struct { int x; int y; }  ");
    test_type("  union { int x; void *y; }  ");
    test_type("  enum { TOK_PLUS, TOK_MINUS, TOK_INT }  ");
    test_type("  enum { SOMETHING, TRAILING_COMMA, }  ");
    test_type("  enum { }  ");
}

fn(void test_stmt(char *input))
{
    struct Lexer lxr;
    struct Stmt *stmt;

    printf("\n");
    lxr = lexer_init(input);
    if ((stmt = parse_stmt(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
        display_stmt(stdout, stmt);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

fn(void test_parse_stmts())
{
    printf("\n\n----------------------TEST STMTS-----------------------------");
    test_stmt("  {}  ");
    test_stmt("  { int x = 1; launch_missiles(x, 2, 3); }  ");
    test_stmt("  if (123 < 456) { return; }  ");
    test_stmt("  if (need_else_here) { return 100; } else { return -1; }  ");
    test_stmt("  while (i < len) { iterate(); }  ");
    test_stmt("  switch (123) {}  ");
    test_stmt("  switch (x) { case 1: return 1; case 2: f(); break; default: return 0;}  ");
    test_stmt("  break;  ");
    test_stmt("  continue;  ");
    test_stmt("  return 123;  ");
    test_stmt("  return;  ");
    test_stmt("  int x;  ");
    test_stmt("  int x = 123;  ");
    test_stmt("  launch_missiles(1, 2, 3);  ");
}

fn(void test_item(char *input))
{
    struct Lexer lxr;
    struct Item *item;

    printf("\n");
    lxr = lexer_init(input);
    if ((item = parse_item(&lxr)) && lexer_accept(&lxr, TOK_END_OF_INPUT))
        display_item(stdout, item);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

fn(void test_parse_items())
{
    printf("\n\n----------------------TEST ITEMS-----------------------------");
    test_item("  int my_const = 100;  ");
    test_item("  cstr_arr names = { \"Jenn\", \"Tim\", \"Scott\", };  ");
    test_item("  struct EmptyStruct {};  ");
    test_item("  struct Point { int x; int y; };  ");
    test_item("  enum EmptyEnum {};  ");
    test_item("  enum TokTag { TOK_IDENT, TOK_PLUS };  ");
    test_item("  #include <stdio.h>  ");
    test_item("  #include \"my_file.h\"  ");
}

fn(void test_parser())
{
    printf("%s\n", "Running parser tests...");
    test_parse_exprs();
    test_parse_types();
    test_parse_stmts();
    test_parse_items();
    printf("\n\n%s\n", "Tests finished!");
}
