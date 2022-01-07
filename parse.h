#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc, free
#include "lex.h"    // lex, lex_all_input
#include "cheats.h" // todo

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

void list_push(struct List *list, void *data)
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

struct Item *parse_compiler_directive(struct Lexer *);
bool display_struct_members(FILE *, struct List *);
bool display_enum_members(FILE *, struct List *);
struct Stmt *parse_var_decl(struct Lexer *);
struct Type *parse_direct_type(struct Lexer *);
struct Type *parse_type(struct Lexer *);
struct Expr *parse_constant(struct Lexer *);
struct Expr *parse_expression(struct Lexer *);
struct Expr *parse_cast_expression(struct Lexer *);
struct Expr *parse_assignment_expression(struct Lexer *);
bool display_expr(FILE *, struct Expr *);
struct List parse_argument_expression_list(struct Lexer *);
bool display_arglist(FILE *, struct List *);

// #-------------------------------------------------------------------------
// #  Items
// #-------------------------------------------------------------------------

struct Item
{
    enum
    {
        ITEM_POUND_INCLUDE,
    } tag;

    union
    {
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

bool display_item(FILE *out, struct Item *item)
{
    switch (item->tag)
    {
    case ITEM_POUND_INCLUDE:
        if (item->as.pound_include.kind == POUND_INCLUDE_ANGLE_BRACKETS)
        {
            return fprintf_s(out, "#include <%s>", item->as.pound_include.filename) >= 0;
        }
        else
        {
            return fprintf_s(out, "#include \"%s\"", item->as.pound_include.filename) >= 0;
        }
    default:
        perror("Invalid Item tag");
        exit(1);
    }
}

// translation_unit ::= item*

// item ::= function_decl
//        | function_def
//        | constant_decl
//        | named_struct_or_union_decl
//        | named_enum_decl
//        | compiler_directive
struct Item *parse_item(struct Lexer *lxr)
{
    struct Item *item;

    if ((item = parse_compiler_directive(lxr)))
        return item;

    todo;
}

// function_decl ::= type 'ident' '(' unnamed_parameter_list? ')' ';'

// function_def ::= type 'ident' '(' parameter_list? ')' stmt_block

// unnamed_parameter_list ::= type (',' type)* ','?

// parameter_list ::= type 'ident' (',' type 'ident')* ','?

// constant_decl ::= type 'ident' '=' ( initializer_list | expression ) ';'

// initializer_list ::= '{' '}' | '{' expression (',' expression)* ','? '}'

// anonymous_struct_or_union ::= ('struct' | 'union') '{' struct_member_list '}'
struct Type *parse_anon_struct_or_union(struct Lexer *lxr)
{
    todo;
}

struct StructMember
{
    struct Type *type;
    char *field;
};

// struct_member_list ::= struct_member*
// struct_member ::= type 'ident' ';'
struct List parse_struct_member_list(struct Lexer *lxr)
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

// enumerator_list ::= <empty string>
//                   | 'ident' (',' 'ident')* ','?
struct List parse_enumerator_list(struct Lexer *lxr)
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

// named_struct_decl ::= named_struct_ref '{' struct_member+ '}' ';'

// named_enum_decl ::= named_enum_ref '{' enumerator_list '}' ';'

// compiler_directive ::= '#' 'include' (angle_bracket_filename | double_quoted_filename)
struct Item *parse_compiler_directive(struct Lexer *lxr)
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
// #  Types
// #-------------------------------------------------------------------------

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

bool display_type(FILE *out, struct Type *type)
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

bool display_struct_members(FILE *out, struct List *list)
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

bool display_enum_members(FILE *out, struct List *list)
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

// type ::= direct_type '*'*
struct Type *parse_type(struct Lexer *lxr)
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
// anonymous_enum ::= 'enum' '{' enumerator_list '}'
struct Type *parse_direct_type(struct Lexer *lxr)
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

// #-------------------------------------------------------------------------
// #  A.2.3  Statements
// #-------------------------------------------------------------------------

struct Stmt
{
    enum
    {
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

bool display_stmt(FILE *out, struct Stmt *stmt)
{
    switch (stmt->tag)
    {
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

// stmt ::= stmt_block
//        | if_stmt
//        | while_stmt
//        | switch_stmt
//        | labelled_stmt
//        | jump_stmt
//        | return_stmt
//        | var_decl
//        | expr_stmt
// stmt_block ::= '{' stmt_list '}'
// if_stmt ::= 'if' '(' expression ')' stmt ('else' stmt)?
// while_stmt ::= 'while' '(' expression ')' stmt
// switch_stmt ::= 'switch' '(' expression ')' stmt
// case_stmt ::= 'case' expression ':'
// default_stmt ::= 'default' ':'
// jump_stmt ::= ('break' | 'continue') ';'
// return_stmt ::= 'return' expression? ';'
struct Stmt *parse_stmt(struct Lexer *lxr)
{
    struct Stmt *stmt;

    // BREAK STMT
    if (lexer_accept(lxr, TOK_BREAK))
    {
        stmt = malloc(sizeof(*stmt));
        stmt->tag = STMT_BREAK;

        return stmt;
    }

    // CONTINUE STMT
    if (lexer_accept(lxr, TOK_CONTINUE))
    {
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
struct List parse_stmt_list(struct Lexer *lxr)
{
    todo;
}

// var_decl ::= type 'ident' ('=' expression)? ';'
struct Stmt *parse_var_decl(struct Lexer *lxr)
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

// expr_stmt ::= expression ';'

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

bool display_bin_op(FILE *out, char *op, struct Expr *x, struct Expr *y)
{
    return display_expr(out, x) &&
           fprintf_s(out, " %s ", op) >= 0 &&
           display_expr(out, y);
}

bool display_expr(FILE *out, struct Expr *expr)
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

bool display_arglist(FILE *out, struct List *list)
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
struct Expr *parse_primary_expression(struct Lexer *lxr)
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
struct Expr *parse_postfix_expression(struct Lexer *lxr)
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
struct List parse_argument_expression_list(struct Lexer *lxr)
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
struct Expr *parse_unary_expression(struct Lexer *lxr)
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
struct Expr *parse_cast_expression(struct Lexer *lxr)
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
struct Expr *parse_multiplicative_expression(struct Lexer *lxr)
{
    // TODO: impl multiplication, division, modulo
    return parse_cast_expression(lxr);
}

// AdditiveExpression
//      <- MultiplicativeExpression ((PLUS / MINUS) MultiplicativeExpression)*
struct Expr *parse_additive_expression(struct Lexer *lxr)
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
struct Expr *parse_shift_expression(struct Lexer *lxr)
{
    return parse_additive_expression(lxr);
}

// RelationalExpression
//      <- ShiftExpression ((LE / GE / LT / GT) ShiftExpression)*
struct Expr *parse_relational_expression(struct Lexer *lxr)
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
struct Expr *parse_equality_expression(struct Lexer *lxr)
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
struct Expr *parse_logical_and_expression(struct Lexer *lxr)
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
struct Expr *parse_logical_or_expression(struct Lexer *lxr)
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
struct Expr *parse_conditional_expression(struct Lexer *lxr)
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
struct Expr *parse_assignment_expression(struct Lexer *lxr)
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
struct Expr *parse_expression(struct Lexer *lxr)
{
    return parse_assignment_expression(lxr);
}

// ConstantExpression <- ConditionalExpression
struct Expr *parse_constant_expression(struct Lexer *lxr)
{
    return parse_conditional_expression(lxr);
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
struct Expr *parse_constant(struct Lexer *lxr)
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

void test_expr(char *input)
{
    struct Lexer lxr;
    struct Expr *expr;

    printf("\n");
    lxr = lexer_init(input);
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_exprs()
{
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

void test_type(char *input)
{
    struct Lexer lxr;
    struct Type *type;

    printf("\n");
    lxr = lexer_init(input);
    if ((type = parse_type(&lxr)))
        display_type(stdout, type);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_types()
{
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

void test_stmt(char *input)
{
    struct Lexer lxr;
    struct Stmt *stmt;

    lxr = lexer_init(input);
    if ((stmt = parse_stmt(&lxr)))
        display_stmt(stdout, stmt);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_stmts()
{
    printf("\n\n");
    test_stmt("  break;  ");
    test_stmt("  continue;  ");
    test_stmt("  return 123;  ");
    test_stmt("  return;  ");
    test_stmt("  int x;  ");
    test_stmt("  int x = 123;  ");
    test_stmt("  launch_missiles(1, 2, 3);  ");
}

void test_item(char *input)
{
    struct Lexer lxr;
    struct Item *item;

    printf("\n");
    lxr = lexer_init(input);
    if ((item = parse_item(&lxr)))
        display_item(stdout, item);
    else
        fprintf_s(stderr, "\n!!! Test failure on input:\n  \"%s\"\n", input);
}

void test_parse_items()
{
    test_item("  #include <stdio.h>  ");
    test_item("  #include \"my_file.h\"  ");
}

void test_parser()
{
    printf("%s\n", "Running parser tests...");
    test_parse_exprs();
    test_parse_types();
    test_parse_stmts();
    test_parse_items();
    printf("\n\n%s\n", "Tests finished!");
}
