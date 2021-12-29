#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc
#include "lex.h"    // lex, lex_all_input

// Parasitic C

enum AstTag
{
    AST_LITERAL_INT,
    AST_LITERAL_CHAR,
};

char *ast_tag_names[] = {
    "AST_LITERAL_INT",
    "AST_LITERAL_CHAR",
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
    } as;
};

int dbg_ast_node(FILE *out, struct AstNode *node)
{
    switch (node->tag)
    {
    case AST_LITERAL_INT:
        return fprintf_s(out, "%d", node->as.literal_int.value);
    case AST_LITERAL_CHAR:
        return fprintf_s(out, "'%s'", node->as.literal_char.value);
    default:
        perror("Invalid AstTag");
        exit(1);
    }
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

int main()
{
    char *new_input;
    struct AstNode *node;

    if (parse_literal_int("-531", &node, &new_input))
    {
        bool _ = dbg_ast_node(stdout, node);
    }

    printf("\n");

    if (parse_literal_char("'\\n'", &node, &new_input))
    {
        bool _ = dbg_ast_node(stdout, node);
    }
    printf("\n%s\n", "DONE!");
}