#include <stdio.h>   // printf, perror, fprintf_s, FILE
#include <stdlib.h>  // exit
#include <ctype.h>   // isalpha, isalnum, isspace, isdigit
#include <string.h>  // strncpy_s
#include <stdbool.h> // true, false, bool
#include "cheats.h"  // cstr_arr, fn, fndecl, global

enum TokTag
{
    // SYMBOLS
    TOK_OPEN_PAREN,
    TOK_CLOSE_PAREN,
    TOK_OPEN_BRACE,
    TOK_CLOSE_BRACE,
    TOK_OPEN_BRACK,
    TOK_CLOSE_BRACK,
    TOK_POUND,
    TOK_SEMI,
    TOK_COLON,
    TOK_EQUAL,
    TOK_STAR,
    TOK_AMPERSAND,
    TOK_COMMA,       // ,
    TOK_DOT,         // .
    TOK_ARROW,       // ->
    TOK_PLUS_PLUS,   // ++
    TOK_EQUAL_EQUAL, // ==
    TOK_NOT_EQUAL,   // !=
    TOK_AND,         // &&
    TOK_OR,          // ||
    TOK_BANG,        // !
    TOK_GT,          // >
    TOK_GTE,         // >=
    TOK_LT,          // <
    TOK_LTE,         // <=
    TOK_PLUS,        // +
    TOK_MINUS,       // -

    // LITERALS
    TOK_LITERAL_INT,
    TOK_LITERAL_CHAR,
    TOK_LITERAL_STRING,
    TOK_ANGLE_BRACK_FILENAME,

    // KEYWORDS
    TOK_INT,
    TOK_CHAR,
    TOK_BOOL,
    TOK_VOID,
    TOK_CSTR_ARR,
    TOK_FILE, // The `FILE` type.

    TOK_INCLUDE,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_TRUE,
    TOK_FALSE,
    TOK_SIZEOF,
    TOK_STRUCT,
    TOK_ENUM,
    TOK_UNION,

    // CHEAT KEYWORDS
    TOK_FN,
    TOK_FNDECL,
    TOK_GLOBAL,

    // IDENTIFIER
    TOK_IDENT,

    // Special
    TOK_END_OF_INPUT, // $
    TOK_LEX_ERROR,
};

global(cstr_arr tok_tag_names) = {
    // SYMBOLS
    "symbol `(`",
    "symbol `)`",
    "symbol `{`",
    "symbol `}`",
    "symbol `[`",
    "symbol `]`",
    "symbol `#`",
    "symbol `;`",
    "symbol `:`",
    "symbol `=`",
    "symbol `*`",
    "symbol `&`",
    "symbol `,`",
    "symbol `.`",
    "symbol `->`",
    "symbol `++`",
    "symbol `==`",
    "symbol `!=`",
    "symbol `&&`",
    "symbol `||`",
    "symbol `!`",
    "symbol `>`",
    "symbol `>=`",
    "symbol `<`",
    "symbol `<=`",
    "symbol `+`",
    "symbol `-`",

    // LITERALS
    "integer literal",
    "character literal",
    "string literal",
    "angle bracketed filename",

    // KEYWORDS
    "keyword `int`",
    "keyword `char`",
    "keyword `bool`",
    "keyword `void`",
    "keyword `cstr_arr`",
    "keyword `FILE`",

    "keyword `include`",
    "keyword `return`",
    "keyword `if`",
    "keyword `else`",
    "keyword `while`",
    "keyword `switch`",
    "keyword `case`",
    "keyword `default`",
    "keyword `break`",
    "keyword `continue`",
    "keyword `true`",
    "keyword `false`",
    "keyword `sizeof`",
    "keyword `struct`",
    "keyword `enum`",
    "keyword `union`",

    // CHEAT KEYWORDS
    "keyword `fn`",
    "keyword `fndecl`",
    "keyword `global`",

    // IDENTIFIER
    "identifier",

    // Special
    "<end of input>",
    "<tokenization error>",
};

struct Lexer
{
    char *filename;           // The name of the file being lexed.
    int line;                 // The current line number.
    enum TokTag tok_tag;      // The TokTag of the current token.
    enum TokTag next_tok_tag; // The TokTag of the next token.
    char *token;              // The text of the current token (if relevent).
    char *next_token;         // The text of the next token (if relevent).
    char *input;              // A pointer to the current location in the source code.
    char *old_input;          // A pointer to the location where the last token was lexed.
    bool expecting_filename;  // If we just lexed `include`, allow `<filename.h>` strings.
};

fndecl(void lexer_advance(struct Lexer *));

fn(struct Lexer lexer_init(char *filename, char *input))
{
    struct Lexer lxr;

    lxr.filename = filename;
    lxr.line = 1;
    lxr.tok_tag = TOK_LEX_ERROR;
    lxr.next_tok_tag = TOK_LEX_ERROR;
    lxr.token = NULL;
    lxr.next_token = NULL;
    lxr.input = input;
    lxr.old_input = input;
    lxr.expecting_filename = false;

    lexer_advance(&lxr);

    return lxr;
}

fn(bool lexer_starts_with(struct Lexer *lxr, char *target))
{
    int i = 0;
    int lines = 0;

    while (true)
    {
        if (target[i] == '\0')
        {
            lxr->input = lxr->input + i;
            lxr->line = lxr->line + lines;
            return true;
        }

        if (lxr->input[i] == '\n')
            lines++;

        if (lxr->input[i] != target[i])
        {
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
fn(bool lexer_accept_space(struct Lexer *lxr))
{
    bool found_space = false;

    while (true)
    {
        if (*lxr->input == '\0')
            break;

        if (isspace(*lxr->input))
        {
            found_space = true;
            if (*lxr->input == '\n')
                lxr->line++;
            lxr->input++;
            continue;
        }

        if (lxr->input[0] == '/' && lxr->input[1] == '/') // Comments
        {
            // Skip the slashes.
            lxr->input++;
            lxr->input++;

            while (*lxr->input != '\n' && *lxr->input != '\0')
                lxr->input++;

            if (*lxr->input == '\n')
            {
                lxr->line++;
                lxr->input++;
            }

            continue;
        }

        break;
    }

    return found_space || *lxr->input == '\0';
}

// Returns `true` if at end of input.
fn(bool lexer_allow_whitespace(struct Lexer *lxr))
{
    lexer_accept_space(lxr);
    return *lxr->input == '\0';
}

fn(bool lexer_accept_ident(struct Lexer *lxr))
{
    char *ident_start = lxr->input;
    int i = 0;
    if (isalpha(lxr->input[i]) || lxr->input[i] == '_')
    {
        i++;
        while (isalnum(lxr->input[i]) || lxr->input[i] == '_')
        {
            i++;
        }
        lxr->next_tok_tag = TOK_IDENT;
        lxr->next_token = malloc(i + 1); // Add 1 for the '\0'.
        strncpy_s(lxr->next_token, i + 1, ident_start, i);
        lxr->input = lxr->input + i;
        return true;
    }

    return false;
}

fn(bool lexer_accept_symbol(struct Lexer *lxr, char *expected_symbol, enum TokTag expected_tok_tag))
{
    if (lexer_starts_with(lxr, expected_symbol))
    {
        lxr->next_tok_tag = expected_tok_tag;
        lxr->next_token = NULL; // The token text is not needed.
        return true;
    }
    return false;
}

fn(bool lexer_accept_keyword(struct Lexer *lxr, char *expected_kw, enum TokTag expected_tok_tag))
{
    char *old_input = lxr->input;
    if (lexer_starts_with(lxr, expected_kw))
    {
        if (!isalnum(lxr->input[0]) && lxr->input[0] != '_')
        {
            lxr->next_tok_tag = expected_tok_tag;
            lxr->next_token = NULL; // The token text is not needed.
            return true;
        }
        else
        {
            // `*input` IS alphanumeric, so `expected_kw` is a strict subsequence of
            // `old_input`.
            // EXAMPLE:
            //   old_input   = "javascript";
            //   expected_kw = "java";
            lxr->input = old_input;
            return false;
        }
    }
    else
    {
        lxr->input = old_input;
        return false;
    }
}

fn(bool lexer_accept_literal_int(struct Lexer *lxr))
{
    int i = 0;

    if (lxr->input[i] == '-')
        i++;

    while (isdigit(lxr->input[i]))
        i++;

    if (i > 1 || (i == 1 && lxr->input[0] != '-'))
    {
        lxr->next_tok_tag = TOK_LITERAL_INT;
        lxr->next_token = malloc(i + 1); // Need extra space for '\0'.
        strncpy_s(lxr->next_token, i + 1, lxr->input, i);
        lxr->input = lxr->input + i;
        return true;
    }
    else
    {
        return false;
    }
}

fn(bool lexer_accept_literal_char(struct Lexer *lxr))
{
    int i = 0;

    if (lxr->input[i] != '\'') // Start quote mark.
        return false;
    i++;

    if (lxr->input[i] == '\\') // Allow a backslash, but just copy it on through.
        i++;

    i++; // Advance past the character.

    if (lxr->input[i] != '\'') // End quote mark.
        return false;
    i++;

    lxr->next_tok_tag = TOK_LITERAL_CHAR;
    lxr->next_token = malloc(i + 1 - 2); // Add 1 for the '\0', subtract 2 to drop quotes.
    strncpy_s(lxr->next_token, i + 1 - 2, &lxr->input[1], i - 2);
    lxr->input = lxr->input + i;
    return true;
}

fn(bool lexer_accept_literal_string(struct Lexer *lxr))
{
    int i = 0;

    if (lxr->input[i] != '"') // Start quote mark.
        return false;
    i++;

    while (lxr->input[i] != '"' && lxr->input[i] != '\0')
    {
        if (lxr->input[i] == '\n')
            lxr->line++;

        if (lxr->input[i] == '\\')
        {
            i++;

            if (lxr->input[i] == '\n')
                lxr->line++;

            if (lxr->input[i] == '\0') // In case there's a backslash at the EOF.
            {
                return false;
            }
        }
        i++;
    }

    if (lxr->input[i] != '"') // End quote mark.
        return false;
    i++;

    lxr->next_tok_tag = TOK_LITERAL_STRING;
    lxr->next_token = malloc(i + 1 - 2); // Add 1 for the '\0', subtract 2 to drop quotes.
    strncpy_s(lxr->next_token, i + 1 - 2, &lxr->input[1], i - 2);
    lxr->input = lxr->input + i;
    return true;
}

fn(bool lexer_accept_angle_bracket_filename(struct Lexer *lxr))
{
    int i = 0;

    if (lxr->input[i] != '<') // Start quote mark.
        return false;
    i++;

    while (lxr->input[i] != '>' && lxr->input[i] != '\0')
    {
        if (lxr->input[i] == '\n')
            lxr->line++;

        if (lxr->input[i] == '\\')
        {
            i++;

            if (lxr->input[i] == '\n')
                lxr->line++;

            if (lxr->input[i] == '\0') // In case there's a backslash at the EOF.
            {
                return false;
            }
        }
        i++;
    }

    if (lxr->input[i] != '>') // End quote mark.
        return false;
    i++;

    lxr->next_tok_tag = TOK_ANGLE_BRACK_FILENAME;
    lxr->next_token = malloc(i + 1 - 2); // Add 1 for the '\0', subtract 2 to drop quotes.
    strncpy_s(lxr->next_token, i + 1 - 2, &lxr->input[1], i - 2);
    lxr->input = lxr->input + i;
    return true;
}

// Returns `false` if at end of input, `true` otherwise.
fn(bool lex(struct Lexer *lxr))
{
    if (lexer_allow_whitespace(lxr))
    {
        lxr->next_tok_tag = TOK_END_OF_INPUT;
        return false;
    }

    if (lexer_accept_literal_int(lxr))
        return true;

    if (lexer_accept_literal_char(lxr))
        return true;

    if (lexer_accept_literal_string(lxr))
    {
        lxr->expecting_filename = false; // We MAY HAVE parsed a filename.
        return true;
    }

    if (lxr->expecting_filename && lexer_accept_angle_bracket_filename(lxr))
    {
        lxr->expecting_filename = false; // We just parsed a filename.
        return true;
    }

    if (lexer_accept_symbol(lxr, "(", TOK_OPEN_PAREN))
        return true;

    if (lexer_accept_symbol(lxr, ")", TOK_CLOSE_PAREN))
        return true;

    if (lexer_accept_symbol(lxr, "{", TOK_OPEN_BRACE))
        return true;

    if (lexer_accept_symbol(lxr, "}", TOK_CLOSE_BRACE))
        return true;

    if (lexer_accept_symbol(lxr, "[", TOK_OPEN_BRACK))
        return true;

    if (lexer_accept_symbol(lxr, "]", TOK_CLOSE_BRACK))
        return true;

    if (lexer_accept_symbol(lxr, "#", TOK_POUND))
        return true;

    if (lexer_accept_symbol(lxr, ";", TOK_SEMI))
        return true;

    if (lexer_accept_symbol(lxr, ":", TOK_COLON))
        return true;

    if (lexer_accept_symbol(lxr, "==", TOK_EQUAL_EQUAL))
        return true;

    if (lexer_accept_symbol(lxr, "=", TOK_EQUAL))
        return true;

    if (lexer_accept_symbol(lxr, "*", TOK_STAR))
        return true;

    if (lexer_accept_symbol(lxr, "&&", TOK_AND))
        return true;

    if (lexer_accept_symbol(lxr, "&", TOK_AMPERSAND))
        return true;

    if (lexer_accept_symbol(lxr, ",", TOK_COMMA))
        return true;

    if (lexer_accept_symbol(lxr, ".", TOK_DOT))
        return true;

    if (lexer_accept_symbol(lxr, "->", TOK_ARROW))
        return true;

    if (lexer_accept_symbol(lxr, "++", TOK_PLUS_PLUS))
        return true;

    if (lexer_accept_symbol(lxr, "!=", TOK_NOT_EQUAL))
        return true;

    if (lexer_accept_symbol(lxr, "||", TOK_OR))
        return true;

    if (lexer_accept_symbol(lxr, "!", TOK_BANG))
        return true;

    if (lexer_accept_symbol(lxr, ">=", TOK_GTE))
        return true;

    if (lexer_accept_symbol(lxr, ">", TOK_GT))
        return true;

    if (lexer_accept_symbol(lxr, "<=", TOK_LTE))
        return true;

    if (lexer_accept_symbol(lxr, "<", TOK_LT))
        return true;

    if (lexer_accept_symbol(lxr, "+", TOK_PLUS))
        return true;

    if (lexer_accept_symbol(lxr, "-", TOK_MINUS))
        return true;

    if (lexer_accept_keyword(lxr, "int", TOK_INT))
        return true;

    if (lexer_accept_keyword(lxr, "char", TOK_CHAR))
        return true;

    if (lexer_accept_keyword(lxr, "bool", TOK_BOOL))
        return true;

    if (lexer_accept_keyword(lxr, "void", TOK_VOID))
        return true;

    if (lexer_accept_keyword(lxr, "cstr_arr", TOK_CSTR_ARR))
        return true;

    if (lexer_accept_keyword(lxr, "FILE", TOK_FILE))
        return true;

    if (lexer_accept_keyword(lxr, "include", TOK_INCLUDE))
    {
        lxr->expecting_filename = true; // Transition state. Now we can parse `<asdf.h>` strings.
        return true;
    }

    if (lexer_accept_keyword(lxr, "return", TOK_RETURN))
        return true;

    if (lexer_accept_keyword(lxr, "if", TOK_IF))
        return true;

    if (lexer_accept_keyword(lxr, "else", TOK_ELSE))
        return true;

    if (lexer_accept_keyword(lxr, "while", TOK_WHILE))
        return true;

    if (lexer_accept_keyword(lxr, "switch", TOK_SWITCH))
        return true;

    if (lexer_accept_keyword(lxr, "case", TOK_CASE))
        return true;

    if (lexer_accept_keyword(lxr, "default", TOK_DEFAULT))
        return true;

    if (lexer_accept_keyword(lxr, "break", TOK_BREAK))
        return true;

    if (lexer_accept_keyword(lxr, "continue", TOK_CONTINUE))
        return true;

    if (lexer_accept_keyword(lxr, "true", TOK_TRUE))
        return true;

    if (lexer_accept_keyword(lxr, "false", TOK_FALSE))
        return true;

    if (lexer_accept_keyword(lxr, "sizeof", TOK_SIZEOF))
        return true;

    if (lexer_accept_keyword(lxr, "struct", TOK_STRUCT))
        return true;

    if (lexer_accept_keyword(lxr, "enum", TOK_ENUM))
        return true;

    if (lexer_accept_keyword(lxr, "union", TOK_UNION))
        return true;

    if (lexer_accept_keyword(lxr, "fndecl", TOK_FNDECL))
        return true;

    if (lexer_accept_keyword(lxr, "fn", TOK_FN))
        return true;

    if (lexer_accept_keyword(lxr, "global", TOK_GLOBAL))
        return true;

    if (lexer_accept_ident(lxr))
        return true;

    // Fallthrough
    fprintf_s(stderr, "TOKENIZATION ERROR:\n");
    fprintf_s(stderr, "| at %s:%d\n", lxr->filename, lxr->line);
    fprintf_s(stderr, "|    Unrecognized token starting with '%c'\n", lxr->input[0]);
    exit(1);
}

///////////////////////////////////////////////////////////////////////////////

// Mutates state of the lexer.
fn(void lexer_advance(struct Lexer *lxr))
{
    lxr->old_input = lxr->input;
    lxr->token = lxr->next_token;
    lxr->tok_tag = lxr->next_tok_tag;
    lex(lxr);
}

// If the next token type is equal to the one provided as argument, advances
// lexer. Otherwise, simply returns false.
fn(bool lexer_accept(struct Lexer *lxr, enum TokTag tag))
{
    // NOTE: we check against `next_tok_tag` (NOT `tok_tag`) in order to set up
    // `tok_tag` for calling code after `lexer_advance` is called here.
    if (lxr->next_tok_tag == tag)
    {
        lexer_advance(lxr);
        return true;
    }
    return false;
}

// Requires that the next token type is equal to the one provided as argument.
// Throws an error and exits if this is not the case.
fn(bool lexer_expect(struct Lexer *lxr, enum TokTag tag))
{
    if (lexer_accept(lxr, tag))
    {
        return true;
    }
    else
    {
        fprintf_s(stderr, "PARSE ERROR:\n");
        fprintf_s(stderr, "| at %s:%d\n", lxr->filename, lxr->line);
        fprintf_s(stderr, "|\n");
        fprintf_s(stderr, "|   expected %s, got %s\n",
                  tok_tag_names[tag],
                  tok_tag_names[lxr->next_tok_tag]);
        fprintf_s(stderr, "|\n");
        fprintf_s(stderr, "|>  `%.40s`\n", lxr->old_input);
        exit(1);
    }
}

fn(int dbg_tok_tag(FILE *out, enum TokTag tok_tag))
{
    return fprintf_s(out, "TOKEN %s", tok_tag_names[tok_tag]);
}

fn(void lex_all_input(char *input))
{
    struct Lexer lxr = lexer_init("<test input>", input);

    while (true)
    {
        lexer_advance(&lxr);
        dbg_tok_tag(stdout, lxr.tok_tag);

        if (lxr.tok_tag == TOK_IDENT ||
            lxr.tok_tag == TOK_LITERAL_INT ||
            lxr.tok_tag == TOK_LITERAL_CHAR ||
            lxr.tok_tag == TOK_LITERAL_STRING ||
            lxr.tok_tag == TOK_ANGLE_BRACK_FILENAME)
            printf("\n\t\"%s\"", lxr.token);

        printf("\n");

        if (lxr.tok_tag == TOK_END_OF_INPUT)
            break;
    }
}

fn(void test_lex(char *input))
{
    printf("--------\n");
    lex_all_input(input);
}

fn(void test_lexer())
{
    printf("LEXER TESTS---------------------\n");
    test_lex("  -123  ");
    test_lex("  \"qwerty pgfdjba\"  ");
    test_lex("  true  ");
    test_lex("  false");
    test_lex("  #include <stdio.h>  ");
    test_lex("  #include \"stdio.h\"  ");
    test_lex("  default // comment\n // blah\n default\n\n default  ");
}
