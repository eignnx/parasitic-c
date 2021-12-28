#include <stdio.h>   // printf, perror
#include <stdlib.h>  // exit
#include <ctype.h>   // isalpha, isalnum, isspace, isdigit
#include <string.h>  // strncpy_s
#include <stdbool.h> // true, false, bool

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
int TOK_AMPERSAND = 111;

// LITERALS
int TOK_LITERAL_INT = 200;
int TOK_LITERAL_CHAR = 201;
int TOK_LITERAL_STRING = 202;

// KEYWORDS
int TOK_INT = 300;
int TOK_CHAR = 301;
int TOK_BOOL = 302;
int TOK_VOID = 303;
int TOK_INCLUDE = 304;
int TOK_RETURN = 305;
int TOK_IF = 306;
int TOK_ELSE = 307;
int TOK_WHILE = 308;
int TOK_FOR = 309;
int TOK_BREAK = 310;
int TOK_CONTINUE = 311;
int TOK_TRUE = 312;
int TOK_FALSE = 313;
int TOK_STRUCT = 314;

// IDENTIFIER
int TOK_IDENT = 400;
int TOK_ANGLE_BRACK_FILENAME = 401;

///////////////// </TOKEN> //////////////////////////

bool starts_with(char *input, char *target, char **new_input)
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
bool expect_space(char *input, char **new_input)
{
    bool found_space = false;
    bool inside_comment = false;

    while (true)
    {
        if (*input == '\0')
        {
            break;
        }
        else if (isspace(*input))
        {
            found_space = true;
            input++;
        }
        else if (input[0] == '/' && input[1] == '/') // Comments
        {
            while (*input != '\n' && *input != '\0')
            {
                input++;
            }
        }
        else
        {
            break;
        }
    }

    *new_input = input;
    return found_space || *input == '\0';
}

// Returns `true` if at end of input.
bool allow_whitespace(char *input, char **new_input)
{
    bool _ = expect_space(input, new_input);
    return **new_input == '\0';
}

bool expect_ident(char *input, char **new_input, char **out_ident)
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

bool expect_symbol(
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

bool expect_keyword(
    char *input,
    char *expected_kw,
    int expected_tok_typ,
    int *out_tok_typ,
    char *old_input,
    char **new_input)
{
    if (starts_with(input, expected_kw, &input))
    {
        if (!isalnum(*input))
        {
            *new_input = input;
            *out_tok_typ = expected_tok_typ;
            return true;
        }
        else
        {
            // `*input` IS alphanumeric, so `expected_kw` is a strict subsequence of
            // `old_input`.
            // EXAMPLE:
            //   old_input   = "javascript";
            //   expected_kw = "java";
            *new_input = old_input;
            return false;
        }
    }
    *new_input = old_input;
    return false;
}

bool lex_literal_int(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    int i = 0;

    if (input[i] == '-')
        i++;

    while (isdigit(input[i]))
        i++;

    if (i > 1 || (i == 1 && input[0] != '-'))
    {
        *new_input = &input[i];
        *out_token = malloc(i + 1); // Need extra space for '\0'.
        *out_tok_typ = TOK_LITERAL_INT;
        strncpy_s(*out_token, i + 1, input, i);
        return true;
    }
    else
    {
        *new_input = input;
        return false;
    }
}

bool lex_literal_char(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    *new_input = input;
    int i = 0;

    if (input[i] != '\'') // Start quote mark.
        return false;
    i++;

    if (input[i] == '\\') // Allow a backslash, but just copy it on through.
        i++;

    i++; // Advance past the character.

    if (input[i] != '\'') // End quote mark.
        return false;
    i++;

    *out_tok_typ = TOK_LITERAL_CHAR;
    *out_token = malloc(i + 1 - 2); // Add 1 for the '\0', subtract 2 to drop quotes.
    *new_input = &input[i];
    strncpy_s(*out_token, i + 1 - 2, &input[1], i - 2);
    return true;
}

bool lex_literal_string(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    *new_input = input;
    int i = 0;

    if (input[i] != '"') // Start quote mark.
        return false;
    i++;

    while (input[i] != '"' && input[i] != '\0')
    {
        if (input[i] == '\\')
        {
            i++;
            if (input[i] == '\0') // In case there's a backslash at the EOF.
            {
                return false;
            }
        }
        i++;
    }

    if (input[i] != '"') // End quote mark.
        return false;
    i++;

    *out_tok_typ = TOK_LITERAL_STRING;
    *out_token = malloc(i + 1 - 2); // Add 1 for the '\0', subtract 2 to drop quotes.
    *new_input = &input[i];
    strncpy_s(*out_token, i + 1 - 2, &input[1], i - 2);
    return true;
}

bool lex_angle_bracket_filename(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    *new_input = input;
    int i = 0;

    if (input[i] != '<') // Start marker.
        return false;
    i++;

    while (input[i] != '>' && input[i] != '\0')
    {
        if (input[i] == '\\')
        {
            return false;
        }
        i++;
    }

    if (input[i] != '>') // End marker.
        return false;
    i++;

    *out_tok_typ = TOK_ANGLE_BRACK_FILENAME;
    *out_token = malloc(i + 1 - 2); // Add 1 for the '\0', subtract 2 to drop quotes.
    *new_input = &input[i];
    strncpy_s(*out_token, i + 1 - 2, &input[1], i - 2);
    return true;
}

// `lex` accepts a string, chomps the next token, and returns the token type AND
// the new string position.
// Returns `false` if at end of input, `true` otherwise.
bool lex(char *input, int *out_tok_typ, char **out_token, char **new_input)
{
    if (allow_whitespace(input, &input))
    {
        *new_input = input;
        return false;
    }

    char *old_input = input;

    if (lex_literal_int(input, out_tok_typ, out_token, new_input))
        return true;

    if (lex_literal_char(input, out_tok_typ, out_token, new_input))
        return true;

    if (lex_literal_string(input, out_tok_typ, out_token, new_input))
        return true;

    if (lex_angle_bracket_filename(input, out_tok_typ, out_token, new_input))
        return true;

    if (expect_symbol(input, "(", TOK_OPEN_PAREN, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, ")", TOK_CLOSE_PAREN, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "{", TOK_OPEN_BRACE, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "}", TOK_CLOSE_BRACE, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "[", TOK_OPEN_BRACK, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "]", TOK_CLOSE_BRACK, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "#", TOK_POUND, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, ";", TOK_SEMI, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "=", TOK_EQUAL, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "*", TOK_STAR, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, ",", TOK_COMMA, out_tok_typ, new_input))
        return true;

    if (expect_symbol(input, "&", TOK_AMPERSAND, out_tok_typ, new_input))
        return true;

    if (expect_keyword(input, "int", TOK_INT, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "char", TOK_CHAR, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "bool", TOK_BOOL, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "void", TOK_VOID, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "include", TOK_INCLUDE, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "return", TOK_RETURN, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "if", TOK_IF, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "else", TOK_ELSE, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "while", TOK_WHILE, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "for", TOK_FOR, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "break", TOK_BREAK, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "continue", TOK_CONTINUE, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "true", TOK_TRUE, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "false", TOK_FALSE, out_tok_typ, old_input, new_input))
        return true;

    if (expect_keyword(input, "struct", TOK_STRUCT, out_tok_typ, old_input, new_input))
        return true;

    if (expect_ident(input, &input, out_token))
    {
        *out_tok_typ = TOK_IDENT;
        *new_input = input;
        return true;
    }

    // Fallthrough
    perror("LEX ERROR");
    printf("\tUnrecognized token starting with '%c'", input[0]);
    exit(1);
}

void lex_all_input(char *input)
{
    int out_tok_typ = -123214;
    char *new_input = "<EMPTY INPUT>";
    char *token = "<EMPTY TOKEN>";

    while (lex(input, &out_tok_typ, &token, &input))
    {
        if (out_tok_typ == TOK_IDENT ||
            out_tok_typ == TOK_LITERAL_INT ||
            out_tok_typ == TOK_LITERAL_CHAR ||
            out_tok_typ == TOK_LITERAL_STRING ||
            out_tok_typ == TOK_ANGLE_BRACK_FILENAME)
        {
            printf("Got Token #%d:\t\"%s\"\n", out_tok_typ, token);
        }
        else
        {
            printf("Got Token #%d\n", out_tok_typ);
        }
    }
    printf("End of input\n");
}

int main()
{
    char *input =
        "#include <stdio.h>\n"
        "#include \"my_file.h\"\n"
        "\n"
        "struct my_type {\n"
        "    int x;\n"
        "}\n"
        "\n"
        "void my_func123()\n"
        "{ // this is a comment! \n"
        "    bool my_bool = true;\n"
        "    int my_int = -1234;\n"
        "    char my_char1 = '\\n';\n"
        "    char my_char2 = 'A';\n"
        "    char my_char3 = '\\'';\n"
        "    char *my_str = \"qwea sdf sgd \\\" \\n rty\";\n"
        "}\n";

    lex_all_input(input);

    // int out_tok_typ = -123214;
    // char *new_input = "<EMPTY INPUT>";
    // char *token = "<EMPTY TOKEN>";

    // lex(input, &out_tok_typ, &token, &new_input);
    // printf("tok_typ = %d, token = \"%s\", new_input = \"%s\"\n", out_tok_typ, token, new_input);
}