#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc
#include "lex.h"    // lex, lex_all_input

// Parasitic C

enum AstTag
{
    // Literals
    AST_LITERAL_INT,
    AST_LITERAL_CHAR,
    AST_LITERAL_STRING,
    AST_LITERAL_BOOL,

    // Top-level Items
    AST_POUND_INCLUDE,
};

char *ast_tag_names[] = {
    "AST_LITERAL_INT",
    "AST_LITERAL_CHAR",
    "AST_LITERAL_STRING",
    "AST_LITERAL_BOOL",
    "AST_POUND_INCLUDE",
};

// Example use:
// int x = my_ast_node.as.literal_int.value;
struct AstNode
{
    enum AstTag tag;
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
            char *filename;
            enum
            {
                ANGLE_BRACKET_POUND_INCLUDE,
                DOUBLE_QUOTE_POUND_INCLUDE,
            } kind;
        } pound_include;

    } as;
};

int display_ast_node(FILE *out, struct AstNode *node)
{
    switch (node->tag)
    {
    case AST_LITERAL_INT:
        return fprintf_s(out, "%d", node->as.literal_int.value);
    case AST_LITERAL_CHAR:
        return fprintf_s(out, "'%s'", node->as.literal_char.value);
    case AST_LITERAL_STRING:
        return fprintf_s(out, "\"%s\"", node->as.literal_string.value);
    case AST_LITERAL_BOOL:
        if (node->as.literal_bool.value)
        {
            return fprintf_s(out, "true");
        }
        else
        {
            return fprintf_s(out, "false");
        }
    case AST_POUND_INCLUDE:
        fprintf_s(out, "#include ");
        if (node->as.pound_include.kind == ANGLE_BRACKET_POUND_INCLUDE)
        {
            return fprintf_s(out, "<%s>", node->as.pound_include.filename);
        }
        else
        {
            return fprintf_s(out, "\"%s\"", node->as.pound_include.filename);
        }

    default:
        perror("Invalid AstTag");
        exit(1);
    }
}

int dbg_ast_node(FILE *out, struct AstNode *node)
{
    fprintf_s(out, "%s: ", ast_tag_names[node->tag]);
    return display_ast_node(out, node);
}

bool parse_literal_int(char *input, struct AstNode **node, char **new_input)
{
    int tok_typ;
    char *token;

    bool result = lex(input, &tok_typ, &token, new_input);

    if (!result || tok_typ != TOK_LITERAL_INT)
        return false;

    *node = malloc(sizeof(**node));
    (*node)->tag = AST_LITERAL_INT;
    (*node)->as.literal_int.value = atoi(token);
    return true;
}

bool parse_literal_char(char *input, struct AstNode **node, char **new_input)
{
    int tok_typ;
    char *token;

    bool result = lex(input, &tok_typ, &token, new_input);

    if (!result || tok_typ != TOK_LITERAL_CHAR)
        return false;

    *node = malloc(sizeof(**node));
    (*node)->tag = AST_LITERAL_CHAR;
    (*node)->as.literal_char.value = token;
    return true;
}

bool parse_literal_string(char *input, struct AstNode **node, char **new_input)
{
    int tok_tag;
    char *token;

    bool result = lex(input, &tok_tag, &token, new_input);

    if (!result || tok_tag != TOK_LITERAL_STRING)
        return false;

    *node = malloc(sizeof(**node));
    (*node)->tag = AST_LITERAL_STRING;
    (*node)->as.literal_string.value = token;
    return true;
}

bool parse_literal_bool(char *input, struct AstNode **node, char **new_input)
{
    int tok_tag;
    char *token;

    bool result = lex(input, &tok_tag, &token, new_input);

    if (result && (tok_tag == TOK_TRUE || tok_tag == TOK_FALSE))
    {
        *node = malloc(sizeof(**node));
        (*node)->tag = AST_LITERAL_BOOL;

        if (tok_tag == TOK_TRUE)
        {
            (*node)->as.literal_bool.value = true;
        }
        else
        {
            (*node)->as.literal_bool.value = false;
        }

        return true;
    }

    return false;
}

bool parse_pound_include(char *input, struct AstNode **node, char **new_input)
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
        *node = malloc(sizeof(**node));
        (*node)->tag = AST_POUND_INCLUDE;
        (*node)->as.pound_include.filename = token;

        if (tok_tag == TOK_ANGLE_BRACK_FILENAME)
            (*node)->as.pound_include.kind = ANGLE_BRACKET_POUND_INCLUDE;
        else
            (*node)->as.pound_include.kind = DOUBLE_QUOTE_POUND_INCLUDE;

        return true;
    }

    return false;
}

int main()
{
    char *new_input;
    struct AstNode *node;

    if (parse_literal_int("-531", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n");

    if (parse_literal_char("'\\n'", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n");

    if (parse_literal_string("\"aldjsfhkjasd kfjh\"", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n");

    if (parse_literal_bool("true", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n");

    if (parse_literal_bool("false", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n");

    if (parse_pound_include("#include <stdio.h>", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n");

    if (parse_pound_include("#include \"stdio.h\"", &node, &new_input))
        dbg_ast_node(stdout, node);

    printf("\n%s\n", "DONE!");
    return 0;
}