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

int expect_ident(char *input, char **out_token, char **new_input)
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
        *new_input = input + i;
        strncpy_s(*out_token, MAX_IDENT_LEN, old_input, i);
        return true;
    }
    *new_input = old_input;
    return false;
}

// `lex` accepts a string, chomps the next token, and returns the token type AND
// the new string position.
void lex(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    char *old_input = input;

    if (starts_with(input, "#", &input))
    {
        *out_tok_typ = TOK_POUND;
        *new_input = input;
        return;
    }

    if (starts_with(input, "(", &input))
    {
        *out_tok_typ = TOK_OPEN_PAREN;
        *new_input = input;
        return;
    }

    if (starts_with(input, ")", &input))
    {
        *out_tok_typ = TOK_CLOSE_PAREN;
        *new_input = input;
        return;
    }

    if (starts_with(input, "{", &input))
    {
        *out_tok_typ = TOK_OPEN_BRACE;
        *new_input = input;
        return;
    }

    if (starts_with(input, "}", &input))
    {
        *out_tok_typ = TOK_CLOSE_BRACE;
        *new_input = input;
        return;
    }

    if (starts_with(input, "[", &input))
    {
        *out_tok_typ = TOK_OPEN_BRACK;
        *new_input = input;
        return;
    }

    if (starts_with(input, "]", &input))
    {
        *out_tok_typ = TOK_CLOSE_BRACK;
        *new_input = input;
        return;
    }

    if (starts_with(input, ";", &input))
    {
        *out_tok_typ = TOK_SEMI;
        *new_input = input;
        return;
    }

    if (starts_with(input, "=", &input))
    {
        *out_tok_typ = TOK_EQUAL;
        *new_input = input;
        return;
    }

    if (starts_with(input, "*", &input))
    {
        *out_tok_typ = TOK_STAR;
        *new_input = input;
        return;
    }

    if (starts_with(input, "int", &input))
    {
        if (expect_space(input, &input))
        {
            *out_tok_typ = TOK_INT;
            *new_input = input;
            return;
        }
        else
        {
            input = old_input;
        }
    }

    if (starts_with(input, "void", &input))
    {
        if (expect_space(input, &input))
        {
            *out_tok_typ = TOK_VOID;
            *new_input = input;
            return;
        }
        else
        {
            input = old_input;
        }
    }

    if (starts_with(input, "char", &input))
    {
        if (expect_space(input, &input))
        {
            *out_tok_typ = TOK_CHAR;
            *new_input = input;
            return;
        }
        else
        {
            input = old_input;
        }
    }

    if (expect_ident(input, out_token, &input))
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
    int out_tok_typ = -123214;
    char *new_input = "<EMPTY INPUT>";
    char *token_buf = malloc(MAX_IDENT_LEN * sizeof(char));
    strncpy_s(token_buf, MAX_IDENT_LEN, "<EMPTY TOKEN>", 14);

    lex("my_type1234 main() {}", &out_tok_typ, &token_buf, &new_input);
    printf("tok_typ = %d, token = \"%s\", new_input = \"%s\"\n", out_tok_typ, token_buf, new_input);
}