#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc, free
#include "lex.h"    // lex, lex_all_input

// Parasitic C

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
        EXPR_DOT_FIELD_ACCESS,
        EXPR_ARROW_FIELD_ACCESS,
        EXPR_POSTFIX_PLUS_PLUS,

        EXPR_REFERENCE,
        EXPR_DEREFERENCE,
        EXPR_NEGATION,
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

    } as;
};

struct Type
{
    enum
    {
        // Atomic types
        TYPE_INT,
        TYPE_BOOL,
        TYPE_CHAR,
        TYPE_VOID,

        // Compound types
        // TYPE_PTR,
        // TYPE_ARRAY,
        // TYPE_STRUCT,
        // TYPE_ENUM,
        // TYPE_UNION,
    } tag;

    // union
    // {
    // } as;
};

// struct Stmt
// {
//     enum {} tag;
//     union {} as;
// };

// Top-level declaration or compiler directive
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

        // struct
        // {
        //     ...
        // } fn_decl;
    } as;
};

int display_expr(FILE *out, struct Expr *expr)
{
    switch (expr->tag)
    {
    case EXPR_LITERAL_INT:
        return fprintf_s(out, "%d", expr->as.literal_int.value);
    case EXPR_LITERAL_CHAR:
        return fprintf_s(out, "'%s'", expr->as.literal_char.value);
    case EXPR_LITERAL_STRING:
        return fprintf_s(out, "\"%s\"", expr->as.literal_string.value);
    case EXPR_LITERAL_BOOL:
        if (expr->as.literal_bool.value)
            return fprintf_s(out, "true");
        else
            return fprintf_s(out, "false");
    case EXPR_IDENT:
        return fprintf_s(out, "%s", expr->as.ident.value);
    case EXPR_PARENTHESIZED:
        return fprintf_s(out, "(") &&
               display_expr(out, expr->as.parenthesized.child) &&
               fprintf_s(out, ")");
    case EXPR_ARR_INDEX:
        return display_expr(out, expr->as.arr_index.arr) &&
               fprintf_s(out, "[") &&
               display_expr(out, expr->as.arr_index.index) &&
               fprintf_s(out, "]");
    case EXPR_DOT_FIELD_ACCESS:
        return display_expr(out, expr->as.field_access.object) &&
               fprintf_s(out, ".%s", expr->as.field_access.field);
    case EXPR_ARROW_FIELD_ACCESS:
        return display_expr(out, expr->as.field_access.object) &&
               fprintf_s(out, "->%s", expr->as.field_access.field);
    case EXPR_POSTFIX_PLUS_PLUS:
        return display_expr(out, expr->as.postfix_plus_plus.operand) &&
               fprintf_s(out, "++");
    case EXPR_REFERENCE:
        return fprintf_s(out, "&") &&
               display_expr(out, expr->as.unary_op.operand);
    case EXPR_DEREFERENCE:
        return fprintf_s(out, "*") &&
               display_expr(out, expr->as.unary_op.operand);
    case EXPR_NEGATION:
        return fprintf_s(out, "!") &&
               display_expr(out, expr->as.unary_op.operand);
    default:
        perror("Invalid Expr tag");
        exit(1);
    }
}

int display_type(FILE *out, struct Type *type)
{
    switch (type->tag)
    {
    case TYPE_INT:
        return fprintf_s(out, "int");
    case TYPE_BOOL:
        return fprintf_s(out, "bool");
    case TYPE_CHAR:
        return fprintf_s(out, "char");
    case TYPE_VOID:
        return fprintf_s(out, "void");
    default:
        perror("Invalid Type tag");
        exit(1);
    }
}

int display_item(FILE *out, struct Item *item)
{
    switch (item->tag)
    {
    case ITEM_POUND_INCLUDE:
        fprintf_s(out, "#include ");
        if (item->as.pound_include.kind == POUND_INCLUDE_ANGLE_BRACKETS)
        {
            return fprintf_s(out, "<%s>", item->as.pound_include.filename);
        }
        else
        {
            return fprintf_s(out, "\"%s\"", item->as.pound_include.filename);
        }
    default:
        perror("Invalid Item tag");
        exit(1);
    }
}

//////////////////////////// EXPRS ////////////////////////////////////////////

struct Expr *parse_expr_literal_int(struct Lexer *lxr)
{
    if (!lexer_accept(lxr, TOK_LITERAL_INT))
        return NULL;

    struct Expr *expr = malloc(sizeof(*expr));
    expr->tag = EXPR_LITERAL_INT;
    expr->as.literal_int.value = atoi(lxr->token);
    return expr;
}

struct Expr *parse_expr_literal_char(struct Lexer *lxr)
{
    if (!lexer_accept(lxr, TOK_LITERAL_CHAR))
        return NULL;

    struct Expr *expr = malloc(sizeof(*expr));
    expr->tag = EXPR_LITERAL_CHAR;
    expr->as.literal_char.value = lxr->token;
    return expr;
}

struct Expr *parse_expr_literal_string(struct Lexer *lxr)
{
    if (!lexer_accept(lxr, TOK_LITERAL_STRING))
        return NULL;

    struct Expr *expr = malloc(sizeof(*expr));
    expr->tag = EXPR_LITERAL_STRING;
    expr->as.literal_string.value = lxr->token;
    return expr;
}

struct Expr *parse_expr_literal_bool(struct Lexer *lxr)
{
    if (!(lxr->next_tok_tag == TOK_TRUE || lxr->next_tok_tag == TOK_FALSE))
        return NULL;

    lexer_advance(lxr);
    struct Expr *expr = malloc(sizeof(*expr));
    expr->tag = EXPR_LITERAL_BOOL;
    expr->as.literal_bool.value = (lxr->tok_tag == TOK_TRUE);
    return expr;
}

struct Expr *parse_constant_or_string_expr(struct Lexer *lxr)
{
    struct Expr *expr;

    if ((expr = parse_expr_literal_int(lxr)))
        return expr;

    if ((expr = parse_expr_literal_bool(lxr)))
        return expr;

    if ((expr = parse_expr_literal_char(lxr)))
        return expr;

    if ((expr = parse_expr_literal_string(lxr)))
        return expr;

    return NULL;
}

struct Expr *parse_primary_expression(struct Lexer *);
struct Expr *parse_postfix_expression(struct Lexer *);
struct Expr *parse_cast_expression(struct Lexer *);
struct Expr *parse_unary_expression(struct Lexer *);
struct Expr *parse_assignment_expression(struct Lexer *);

struct Expr *parse_expression(struct Lexer *lxr)
{
    return parse_assignment_expression(lxr);
}

// primary_expression
// 	: IDENTIFIER
// 	| CONSTANT
// 	| STRING_LITERAL
// 	| '(' expression ')'
// 	;
struct Expr *parse_primary_expression(struct Lexer *lxr)
{
    struct Expr *expr;

    if (lexer_accept(lxr, TOK_IDENT))
    {
        expr = malloc(sizeof(*expr));
        expr->tag = EXPR_IDENT;
        expr->as.ident.value = lxr->token;
        return expr;
    }
    if ((expr = parse_constant_or_string_expr(lxr)))
    {
        return expr;
    }
    if (lexer_accept(lxr, TOK_OPEN_PAREN) &&
        (expr = parse_expression(lxr)) &&
        lexer_expect(lxr, TOK_CLOSE_PAREN))
    {
        // NOTE: We need to parse parenthesized exprs as their own thing so that
        // they can be printed into the output file without changing precedence
        // of operations.
        struct Expr *paren_expr = malloc(sizeof(*paren_expr));
        paren_expr->tag = EXPR_PARENTHESIZED;
        paren_expr->as.parenthesized.child = expr;
        return paren_expr;
    }
    return NULL;
}

// cast_expression
// 	: unary_expression
// 	| '(' type_name ')' cast_expression
// 	;
struct Expr *parse_cast_expression(struct Lexer *lxr)
{
    // TODO: actually handle cast expressions.
    return parse_unary_expression(lxr);
}

// unary_expression
// 	: postfix_expression
// // | '++' unary_expression
// // | '--' unary_expression
// 	| ('&' | '*' | '!') cast_expression
// 	| SIZEOF unary_expression
// 	| SIZEOF '(' type_name ')'
// 	;
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

    return NULL;
}

// multiplicative_expression
// 	: cast_expression
// 	| multiplicative_expression '*' cast_expression
// 	| multiplicative_expression '/' cast_expression
// 	| multiplicative_expression '%' cast_expression
// 	;
struct Expr *parse_multiplicative_expression(struct Lexer *lxr)
{
    return parse_cast_expression(lxr);
}

// additive_expression
// 	: multiplicative_expression
// 	| additive_expression '+' multiplicative_expression
// 	| additive_expression '-' multiplicative_expression
// 	;
struct Expr *parse_additive_expression(struct Lexer *lxr)
{
    return parse_multiplicative_expression(lxr);
}

// relational_expression
// 	: additive_expression
// 	| relational_expression '<' shift_expression
// 	| relational_expression '>' shift_expression
// 	| relational_expression LE_OP shift_expression
// 	| relational_expression GE_OP shift_expression
// 	;
struct Expr *parse_relational_expression(struct Lexer *lxr)
{
    return parse_additive_expression(lxr);
}

// equality_expression
// 	: relational_expression
// 	| equality_expression == relational_expression
// 	| equality_expression != relational_expression
// 	;
struct Expr *parse_equality_expression(struct Lexer *lxr)
{
    return parse_relational_expression(lxr);
}

// logical_and_expression
// 	: equality_expression
// 	| logical_and_expression && inclusive_or_expression
// 	;
struct Expr *parse_logical_and_expression(struct Lexer *lxr)
{
    return parse_equality_expression(lxr);
}

// logical_or_expression
// 	: logical_and_expression
// 	| logical_or_expression || logical_and_expression
// 	;
struct Expr *parse_logical_or_expression(struct Lexer *lxr)
{
    return parse_logical_and_expression(lxr);
}

// conditional_expression
// 	: logical_or_expression
// 	| logical_or_expression '?' expression ':' conditional_expression
// 	;
struct Expr *parse_conditional_expression(struct Lexer *lxr)
{
    return parse_logical_or_expression(lxr);
}

// assignment_expression
// 	: conditional_expression
// 	| unary_expression '=' assignment_expression
// 	;
struct Expr *parse_assignment_expression(struct Lexer *lxr)
{
    return parse_conditional_expression(lxr);
}

// struct Expr *parse_argument_expression_list(struct Lexer *lxr)
// {
// }

// postfix_expression -->
//     primary_expression {
//         '[' expression ']'
//         '(' ')'
//         '(' argument_expression_list ')'
//         '.' IDENTIFIER
//         '->' IDENTIFIER
//         '++'
//         '--'
//     }
struct Expr *parse_postfix_expression(struct Lexer *lxr)
{
    struct Expr *expr;

    if (!(expr = parse_primary_expression(lxr)))
        return NULL;

    while (true)
    {

        // ARRAY INDEX EXPRESSION
        if (lexer_accept(lxr, TOK_OPEN_BRACK))
        {
            struct Expr *index;
            if (!(index = parse_expression(lxr)))
                return NULL;
            lexer_expect(lxr, TOK_CLOSE_BRACK);

            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_ARR_INDEX;
            new_expr->as.arr_index.arr = expr;
            new_expr->as.arr_index.index = index;

            expr = new_expr;
            continue;
        }

        // DOT FIELD ACCESS
        if (lexer_accept(lxr, TOK_DOT))
        {
            lexer_expect(lxr, TOK_IDENT);

            struct Expr *new_expr = malloc(sizeof(*new_expr));
            new_expr->tag = EXPR_DOT_FIELD_ACCESS;
            new_expr->as.field_access.object = expr;
            new_expr->as.field_access.field = lxr->token;

            expr = new_expr;
            continue;
        }

        // ARROW FIELD ACCESS
        if (lexer_accept(lxr, TOK_ARROW))
        {
            lexer_expect(lxr, TOK_IDENT);

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

//////////////////////////// TYPES ////////////////////////////////////////////

struct Type *parse_atomic_type(struct Lexer *lxr)
{
    if (!(lxr->next_tok_tag == TOK_INT ||
          lxr->next_tok_tag == TOK_BOOL ||
          lxr->next_tok_tag == TOK_CHAR ||
          lxr->next_tok_tag == TOK_VOID))
        return NULL;

    lexer_advance(lxr);

    struct Type *type = malloc(sizeof(*type));

    switch (lxr->tok_tag)
    {
    case TOK_INT:
        type->tag = TYPE_INT;
        break;
    case TOK_BOOL:
        type->tag = TYPE_BOOL;
        break;
    case TOK_CHAR:
        type->tag = TYPE_CHAR;
        break;
    case TOK_VOID:
        type->tag = TYPE_VOID;
        break;

    default:
        perror("unreachable");
        return NULL;
    }

    return type;
}

//////////////////////////// STMTS ////////////////////////////////////////////

// TODO

//////////////////////////// ITEMS ////////////////////////////////////////////

struct Item *parse_item_pound_include(struct Lexer *lxr)
{
    if (!lexer_accept(lxr, TOK_POUND))
        return NULL;

    if (!lexer_expect(lxr, TOK_INCLUDE))
        return NULL;

    if (!(lexer_accept(lxr, TOK_ANGLE_BRACK_FILENAME) || lexer_accept(lxr, TOK_LITERAL_STRING)))
        return NULL;

    struct Item *item = malloc(sizeof(*item));
    item->tag = ITEM_POUND_INCLUDE;
    item->as.pound_include.filename = lxr->token;

    if (lxr->tok_tag == TOK_ANGLE_BRACK_FILENAME)
        item->as.pound_include.kind = POUND_INCLUDE_ANGLE_BRACKETS;
    else
        item->as.pound_include.kind = POUND_INCLUDE_DOUBLE_QUOTES;

    return item;
}

///////////////////////////////////////////////////////////////////////////////

void test_parse_exprs()
{
    struct Lexer lxr;
    struct Expr *expr;

    printf("\n");
    lxr = lexer_init("-531");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  '\\n'   ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  \"adfkjlhksjdfh \\n kajdfjahdfs adf a.\"   ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  true ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  false ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  my_dog->birth_date[100].year++ ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  !*&*ptr_to_bool  ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");
}

void test_parse_types()
{
    struct Lexer lxr;
    struct Type *type;

    printf("\n");
    lxr = lexer_init("  int  ");
    if ((type = parse_atomic_type(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  bool  ");
    if ((type = parse_atomic_type(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  char  ");
    if ((type = parse_atomic_type(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  void  ");
    if ((type = parse_atomic_type(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");
}

void test_parse_stmts()
{
}

void test_parse_items()
{
    struct Lexer lxr;
    struct Item *item;

    printf("\n");
    lxr = lexer_init("  #include <stdio.h>  ");
    if ((item = parse_item_pound_include(&lxr)))
        display_item(stdout, item);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  #include \"my_file.h\"  ");
    if ((item = parse_item_pound_include(&lxr)))
        display_item(stdout, item);
    else
        perror("TEST FAILED");
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
