#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc
#include "lex.h"    // lex, lex_all_input

// Parasitic C

struct AstNode
{
    int tok_typ;
};

int AST_LITERAL_INT = 100;
struct LiteralIntNode
{
    struct AstNode ast;
    int value;
};

int dbg_literal_int(FILE *out, struct LiteralIntNode *node)
{
    return fprintf_s(out, "%d", node->value);
}

bool parse_literal_int(char *input, struct LiteralIntNode **node, char **new_input)
{
    int tok_typ;
    char *token;
    bool result = lex(input, &tok_typ, &token, new_input);

    if (!result || tok_typ != TOK_LITERAL_INT)
        return false;

    *node = malloc(sizeof(**node));
    (*node)->value = atoi(token);
    (*node)->ast.tok_typ = AST_LITERAL_INT;
    return true;
}

int AST_LITERAL_CHAR = 101;
struct LiteralCharNode
{
    struct AstNode ast;
    char *value;
};

int dbg_literal_char(FILE *out, struct LiteralCharNode *node)
{
    return fprintf_s(out, "'%s'", node->value);
}

bool parse_literal_char(char *input, struct LiteralCharNode **node, char **new_input)
{
    int tok_typ;
    char *token;
    bool result = lex(input, &tok_typ, &token, new_input);

    if (!result || tok_typ != TOK_LITERAL_CHAR)
        return false;

    *node = malloc(sizeof(**node));
    (*node)->value = token;
    (*node)->ast.tok_typ = AST_LITERAL_CHAR;
    return true;
}

int main()
{
    char *new_input;

    {
        struct LiteralIntNode *node;
        if (parse_literal_int("-531", &node, &new_input))
        {
            bool _ = dbg_literal_int(stdout, node);
        }
    }

    printf("\n");

    {
        struct LiteralCharNode *node;
        if (parse_literal_char("'\\n'", &node, &new_input))
        {
            bool _ = dbg_literal_char(stdout, node);
        }
    }
    printf("\n%s\n", "DONE!");
}