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
        {
            return fprintf_s(out, "true");
        }
        else
        {
            return fprintf_s(out, "false");
        }

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

struct Expr *parse_expr_literal_int_new(struct Lexer *lxr)
{
    if (lexer_accept(lxr, TOK_LITERAL_INT))
    {
        struct Expr *expr = malloc(sizeof(*expr));
        expr->tag = EXPR_LITERAL_INT;
        expr->as.literal_int.value = atoi(lxr->token);
        return expr;
    }
    return NULL;
}

void test_new_parsing_method()
{
    struct Lexer lxr = lexer_init("   -1234   adslfhir lskdj");
    struct Expr *expr = parse_expr_literal_int_new(&lxr);
    if (expr == NULL)
        printf("NULL expr\n");
    display_expr(stdout, expr);
}

bool parse_expr_literal_int(char *input, struct Expr **expr, char **new_input)
{
    int tok_typ;
    char *token;

    bool result = lex(input, &tok_typ, &token, new_input);

    if (!result || tok_typ != TOK_LITERAL_INT)
        return false;

    *expr = malloc(sizeof(**expr));
    (*expr)->tag = EXPR_LITERAL_INT;
    (*expr)->as.literal_int.value = atoi(token);
    return true;
}

bool parse_expr_literal_char(char *input, struct Expr **expr, char **new_input)
{
    int tok_typ;
    char *token;

    bool result = lex(input, &tok_typ, &token, new_input);

    if (!result || tok_typ != TOK_LITERAL_CHAR)
        return false;

    *expr = malloc(sizeof(**expr));
    (*expr)->tag = EXPR_LITERAL_CHAR;
    (*expr)->as.literal_char.value = token;
    return true;
}

bool parse_expr_literal_string(char *input, struct Expr **expr, char **new_input)
{
    int tok_tag;
    char *token;

    bool result = lex(input, &tok_tag, &token, new_input);

    if (!result || tok_tag != TOK_LITERAL_STRING)
        return false;

    *expr = malloc(sizeof(**expr));
    (*expr)->tag = EXPR_LITERAL_STRING;
    (*expr)->as.literal_string.value = token;
    return true;
}

bool parse_expr_literal_bool(char *input, struct Expr **expr, char **new_input)
{
    int tok_tag;
    char *token;

    bool result = lex(input, &tok_tag, &token, new_input);

    if (result && (tok_tag == TOK_TRUE || tok_tag == TOK_FALSE))
    {
        *expr = malloc(sizeof(**expr));
        (*expr)->tag = EXPR_LITERAL_BOOL;

        if (tok_tag == TOK_TRUE)
        {
            (*expr)->as.literal_bool.value = true;
        }
        else
        {
            (*expr)->as.literal_bool.value = false;
        }

        return true;
    }

    return false;
}

//////////////////////////// TYPES ////////////////////////////////////////////

bool parse_atomic_type(char *input, struct Type **type, char **new_input)
{
    int tok_tag;
    char *token;
    bool result;

    *new_input = input;

    result = lex(*new_input, &tok_tag, &token, new_input);

    if (!result)
        return false;

    *type = malloc(sizeof(**type));

    switch (tok_tag)
    {
    case TOK_INT:
        (*type)->tag = TYPE_INT;
        return true;
    case TOK_BOOL:
        (*type)->tag = TYPE_BOOL;
        return true;
    case TOK_CHAR:
        (*type)->tag = TYPE_CHAR;
        return true;
    case TOK_VOID:
        (*type)->tag = TYPE_VOID;
        return true;

    default:
        free(*type);
        return false;
    }
}

//////////////////////////// STMTS ////////////////////////////////////////////

// TODO

//////////////////////////// ITEMS ////////////////////////////////////////////

bool parse_item_pound_include(char *input, struct Item **item, char **new_input)
{
    int tok_tag;
    char *token;
    bool result;

    *new_input = input;

    result = lex(*new_input, &tok_tag, &token, new_input);

    if (!result || tok_tag != TOK_POUND)
        return false;

    result = lex(*new_input, &tok_tag, &token, new_input);

    if (!result || tok_tag != TOK_INCLUDE)
        return false;

    result = lex(*new_input, &tok_tag, &token, new_input);

    if (result && (tok_tag == TOK_ANGLE_BRACK_FILENAME || tok_tag == TOK_LITERAL_STRING))
    {
        *item = malloc(sizeof(**item));
        (*item)->tag = ITEM_POUND_INCLUDE;
        (*item)->as.pound_include.filename = token;

        if (tok_tag == TOK_ANGLE_BRACK_FILENAME)
            (*item)->as.pound_include.kind = POUND_INCLUDE_ANGLE_BRACKETS;
        else
            (*item)->as.pound_include.kind = POUND_INCLUDE_DOUBLE_QUOTES;

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

void test_parse_expr()
{
    char *new_input;
    struct Expr *expr;

    printf("\n");
    if (parse_expr_literal_int("-531", &expr, &new_input))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_expr_literal_char("'\\n'", &expr, &new_input))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_expr_literal_string("\"asldjfhl ds \\n ajsdf\"", &expr, &new_input))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_expr_literal_bool("true", &expr, &new_input))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_expr_literal_bool("false", &expr, &new_input))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");
}

void test_parse_types()
{
    char *new_input;
    struct Type *type;

    printf("\n");
    if (parse_atomic_type("int", &type, &new_input))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_atomic_type("bool", &type, &new_input))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_atomic_type("char", &type, &new_input))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_atomic_type("void", &type, &new_input))
        display_type(stdout, type);
    else
        perror("TEST FAILED");
}

void test_parse_stmts()
{
}

void test_parse_items()
{
    char *new_input;
    struct Item *item;

    printf("\n");
    if (parse_item_pound_include("#include <stdio.h>", &item, &new_input))
        display_item(stdout, item);
    else
        perror("TEST FAILED");

    printf("\n");
    if (parse_item_pound_include("#include \"stdio.h\"", &item, &new_input))
        display_item(stdout, item);
    else
        perror("TEST FAILED");
}

void test_parser()
{
    printf("%s\n", "Running parser tests...");
    test_parse_expr();
    test_parse_types();
    test_parse_stmts();
    test_parse_items();
    printf("\n\n%s\n", "Tests finished!");
}

int main()
{
    //test_parser();
    // test_lexer();
    test_new_parsing_method();
    return 0;
}
