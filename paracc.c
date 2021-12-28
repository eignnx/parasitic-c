#include "stdio.h"  // printf, perror
#include "stdlib.h" // exit
#include "ctype.h"  // isalpha, isalnum, isspace
#include "string.h" // strncpy_s

// Parasitic C

////////////////// <TOKEN> //////////////////////////

// SYMBOLS
int TOK_OPEN_PAREN = 100;
int TOK_CLOSE_PAREN = 101;
int TOK_OPEN_BRACE = 102;
int TOK_CLOSE_BRACE = 103;
int TOK_OPEN_BRACK = 104;
int TOK_CLOSE_BRACK = 105;
int TOK_POUND = 106;
int TOK_SEMI = 107;
int TOK_EQUAL = 108;
int TOK_STAR = 109;
int TOK_COMMA = 110;

// LITERALS
int TOK_STRING_LITERAL = 200;
int TOK_INT_LITERAL = 201;

// KEYWORDS
int TOK_INT = 300;
int TOK_CHAR = 301;
int TOK_VOID = 302;

// IDENTIFIER
int TOK_IDENT = 400;

///////////////// </TOKEN> //////////////////////////

int MAX_IDENT_LEN = 257;

int true = 1;
int false = 0;

int starts_with(char *input, char *target, char **new_input)
{
    int i = 0;
    while (true)
    {
        if (target[i] == '\0')
        {
            *new_input = input + i;
            return true;
        }

        if (input[i] != target[i])
        {
            *new_input = input;
            return false;
        }

        i++;
    }
    return false; // unreachable
}

// EXAMPLES:
// "   \t\n   qwerty" --> true
// "     " --> true
// "qwerty" --> false
// "" --> true (OK because EOF is fine)
int expect_space(char *input, char **new_input)
{

    int found_space = false;

    while (isspace(*input))
    {
        found_space = true;
        input++;
    }

    *new_input = input;
    return found_space || *input == '\0';
}

int expect_ident(char *input, char **new_input, char **out_ident)
{
    char *old_input = input;
    int i = 0;
    if (isalpha(input[i]) || input[i] == '_')
    {
        i++;
        while (isalnum(input[i]) || input[i] == '_')
        {
            i++;
        }
        *new_input = &input[i];
        *out_ident = malloc(i + 1); // Add 1 for the '\0'.
        strncpy_s(*out_ident, i + 1, old_input, i);
        return true;
    }
    *new_input = old_input;
    return false;
}

int expect_symbol(
    char *input,
    char *expected_symbol,
    int expected_tok_typ,
    int *out_tok_typ,
    char **new_input)
{
    if (starts_with(input, expected_symbol, &input))
    {
        *out_tok_typ = expected_tok_typ;
        *new_input = input;
        return true;
    }
    return false;
}

int expect_keyword(
    char *input,
    char *expected_kw,
    int expected_tok_typ,
    int *out_tok_typ,
    char *old_input,
    char **new_input)
{
    if (starts_with(input, expected_kw, &input))
    {
        if (expect_space(input, &input))
        {
            *new_input = input;
            *out_tok_typ = expected_tok_typ;
            return true;
        }
        else
        {
            *new_input = old_input;
            return false;
        }
    }
    *new_input = old_input;
    return false;
}

// `lex` accepts a string, chomps the next token, and returns the token type AND
// the new string position.
void lex(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    char *old_input = input;

    if (expect_symbol(input, "(", TOK_OPEN_PAREN, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, ")", TOK_CLOSE_PAREN, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "{", TOK_OPEN_BRACE, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "}", TOK_CLOSE_BRACE, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "[", TOK_OPEN_BRACK, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "]", TOK_CLOSE_BRACK, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "#", TOK_POUND, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, ";", TOK_SEMI, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "=", TOK_EQUAL, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, "*", TOK_STAR, out_tok_typ, new_input))
        return;

    if (expect_symbol(input, ",", TOK_COMMA, out_tok_typ, new_input))
        return;

    if (expect_keyword(input, "int", TOK_INT, out_tok_typ, old_input, new_input))
        return;

    if (expect_keyword(input, "char", TOK_CHAR, out_tok_typ, old_input, new_input))
        return;

    if (expect_keyword(input, "void", TOK_VOID, out_tok_typ, old_input, new_input))
        return;

    if (expect_ident(input, &input, out_token))
    {
        *out_tok_typ = TOK_IDENT;
        *new_input = input;
        return;
    }

    // Fallthrough
    perror("Unrecognized token");
    exit(1);
}

int main()
{
    char *input =
        "#asjkdfghls123 main()\n"
        "{\n"
        "\n"
        "}";

    int out_tok_typ = -123214;
    char *new_input = "<EMPTY INPUT>";
    char *token = "<EMPTY TOKEN>";

    lex(input, &out_tok_typ, &token, &new_input);
    printf("tok_typ = %d, token = \"%s\", new_input = \"%s\"\n", out_tok_typ, token, new_input);
}