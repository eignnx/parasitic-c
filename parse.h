#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc, free
#include "lex.h"    // lex, lex_all_input

// DEVELOPMENT MACROS, SAFE TO DELETE WHEN BOOTSTRAPPING
#define unimplemented                                                          \
    printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__); \
    exit(1)

// FORWARD DECLARATIONS
struct Type *parse_type_specifier(struct Lexer *);
struct Expr *parse_constant(struct Lexer *);
struct Expr *parse_expression(struct Lexer *);
struct Expr *parse_cast_expression(struct Lexer *);
struct Expr *parse_assignment_expression(struct Lexer *);
bool display_expr(FILE *, struct Expr *);
struct ArgList *parse_argument_expression_list(struct Lexer *);
bool display_arglist(FILE *, struct ArgList *);

// #-------------------------------------------------------------------------
// #  A.2.4  External definitions
// #-------------------------------------------------------------------------

// TranslationUnit <- Spacing ( ExternalDeclaration / SEMI ) * EOT

// ExternalDeclaration <- FunctionDefinition / Declaration

// FunctionDefinition <- DeclarationSpecifiers Declarator DeclarationList? CompoundStatement

// DeclarationList <- Declaration+

// #-------------------------------------------------------------------------
// #  A.2.2  Declarations
// #-------------------------------------------------------------------------

struct Decl
{
    enum
    {
        DECL_POUND_INCLUDE,
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

bool display_decl(FILE *out, struct Decl *decl)
{
    switch (decl->tag)
    {
    case DECL_POUND_INCLUDE:
        if (decl->as.pound_include.kind == POUND_INCLUDE_ANGLE_BRACKETS)
        {
            return fprintf_s(out, "#include <%s>", decl->as.pound_include.filename) >= 0;
        }
        else
        {
            return fprintf_s(out, "#include \"%s\"", decl->as.pound_include.filename) >= 0;
        }
    default:
        perror("Invalid Decl tag");
        exit(1);
    }
}

// Declaration <- DeclarationSpecifiers InitDeclaratorList? SEMI

// DeclarationSpecifiers
//    <- (( StorageClassSpecifier
//        / TypeQualifier
//        / FunctionSpecifier
//        )*
//        TypedefName
//        ( StorageClassSpecifier
//        / TypeQualifier
//        / FunctionSpecifier
//        )*
//       )
//     / ( StorageClassSpecifier
//       / TypeSpecifier
//       / TypeQualifier
//       / FunctionSpecifier
//       )+

// DeclarationPoundInclude       # :NEW
//      <- POUND 'include' (angle_bracket_string / double_quoted_string)
struct Decl *parse_decl_pound_include(struct Lexer *lxr)
{
    if (!lexer_accept(lxr, TOK_POUND))
        return NULL;

    if (!lexer_expect(lxr, TOK_INCLUDE))
        return NULL;

    if (!(lexer_accept(lxr, TOK_ANGLE_BRACK_FILENAME) || lexer_accept(lxr, TOK_LITERAL_STRING)))
        return NULL;

    struct Decl *decl = malloc(sizeof(*decl));
    decl->tag = DECL_POUND_INCLUDE;
    decl->as.pound_include.filename = lxr->token;

    if (lxr->tok_tag == TOK_ANGLE_BRACK_FILENAME)
        decl->as.pound_include.kind = POUND_INCLUDE_ANGLE_BRACKETS;
    else
        decl->as.pound_include.kind = POUND_INCLUDE_DOUBLE_QUOTES;

    return decl;
}

// InitDeclaratorList <- InitDeclarator (COMMA InitDeclarator)*

// InitDeclarator <- Declarator (EQU Initializer)?

// StorageClassSpecifier
//    <- TYPEDEF
//     / EXTERN
//     / STATIC
//     / AUTO
//     / REGISTER
//     / ATTRIBUTE LPAR LPAR (!RPAR .)* RPAR RPAR

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

// TypeSpecifier
//    <- VOID
//     / CHAR
//     / SHORT # :IGNORED
//     / INT
//     / LONG # :IGNORED
//     / FLOAT # :IGNORED
//     / DOUBLE # :IGNORED
//     / SIGNED # :IGNORED
//     / UNSIGNED # :IGNORED
//     / BOOL
//     / COMPLEX # :IGNORED
//     / StructOrUnionSpecifier # :TODO
//     / EnumSpecifier # :TODO
struct Type *parse_type_specifier(struct Lexer *lxr)
{
    if (!(lexer_accept(lxr, TOK_VOID) ||
          lexer_accept(lxr, TOK_CHAR) ||
          lexer_accept(lxr, TOK_INT) ||
          lexer_accept(lxr, TOK_BOOL)))
        return NULL;

    struct Type *type = malloc(sizeof(*type));

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

    default:
        puts("unknown type token!\n");
        exit(1);
    }

    return type;
}

// StructOrUnionSpecifier
//    <- StructOrUnion
//       ( Identifier? LWING StructDeclaration* RWING
//       / Identifier
//       )

// StructOrUnion <- STRUCT / UNION

// StructDeclaration <- ( SpecifierQualifierList StructDeclaratorList? )? SEMI

// SpecifierQualifierList
//    <- ( TypeQualifier*
//         TypedefName
//         TypeQualifier*
//       )
//     / ( TypeSpecifier
//       / TypeQualifier
//       )+
struct Type *parse_specifier_qualifier_list(struct Lexer *lxr)
{
    // TODO: finish impl
    return parse_type_specifier(lxr);
}

// StructDeclaratorList <- StructDeclarator (COMMA StructDeclarator)*

// StructDeclarator
//    <- Declarator? COLON ConstantExpression
//     / Declarator

// EnumSpecifier
//     <- ENUM
//       ( Identifier? LWING EnumeratorList COMMA? RWING
//       / Identifier
//       )

// EnumeratorList <- Enumerator (COMMA Enumerator)*

// Enumerator <- EnumerationConstant (EQU ConstantExpression)?

// TypeQualifier
//    <- CONST
//     / RESTRICT
//     / VOLATILE
//     / DECLSPEC LPAR Identifier RPAR

// FunctionSpecifier <- INLINE / STDCALL

// Declarator <- Pointer? DirectDeclarator

// DirectDeclarator
//    <- ( Identifier
//       / LPAR Declarator RPAR
//       )
//       ( LBRK TypeQualifier* AssignmentExpression? RBRK
//       / LBRK STATIC TypeQualifier* AssignmentExpression RBRK
//       / LBRK TypeQualifier+ STATIC AssignmentExpression RBRK
//       / LBRK TypeQualifier* STAR RBRK
//       / LPAR ParameterTypeList RPAR
//       / LPAR IdentifierList? RPAR
//       )*

// Pointer <- ( STAR TypeQualifier* )+

// ParameterTypeList <- ParameterList (COMMA ELLIPSIS)?

// ParameterList <- ParameterDeclaration (COMMA ParameterDeclaration)*

// ParameterDeclaration
//    <- DeclarationSpecifiers
//       ( Declarator
//       / AbstractDeclarator
//       )?

// IdentifierList <- Identifier (COMMA Identifier)*

// TypeName
//      <- SpecifierQualifierList AbstractDeclarator?
struct Type *parse_type_name(struct Lexer *lxr)
{
    return parse_specifier_qualifier_list(lxr);
}

// AbstractDeclarator
//    <- Pointer? DirectAbstractDeclarator
//     / Pointer

// DirectAbstractDeclarator
//    <- ( LPAR AbstractDeclarator RPAR
//       / LBRK (AssignmentExpression / STAR)? RBRK
//       / LPAR ParameterTypeList? RPAR
//       )
//       ( LBRK (AssignmentExpression / STAR)? RBRK
//       / LPAR ParameterTypeList? RPAR
//       )*

// TypedefName <- Identifier #{&TypedefName}

// Initializer
//    <- AssignmentExpression
//     / LWING InitializerList COMMA? RWING

// InitializerList <- Designation? Initializer (COMMA Designation? Initializer)*

// Designation <- Designator+ EQU

// Designator
//    <- LBRK ConstantExpression RBRK
//     / DOT Identifier

// #-------------------------------------------------------------------------
// #  A.2.3  Statements
// #-------------------------------------------------------------------------

struct Stmt
{
    int placeholder;
};

bool display_stmt(struct Stmt *stmt)
{
    unimplemented;
}

// Statement
//    <- LabeledStatement
//     / CompoundStatement
//     / ExpressionStatement
//     / SelectionStatement
//     / IterationStatement
//     / JumpStatement
struct Stmt *parse_statement(struct Lexer *lxr)
{
    unimplemented;
}

// LabeledStatement
//    <- Identifier COLON Statement
//     / CASE ConstantExpression COLON Statement
//     / DEFAULT COLON Statement

// CompoundStatement <- LWING ( Declaration / Statement )* RWING

// ExpressionStatement <- Expression? SEMI

// SelectionStatement
//    <- IF LPAR Expression RPAR Statement (ELSE Statement)?
//     / SWITCH LPAR Expression RPAR Statement

// IterationStatement
//    <- WHILE LPAR Expression RPAR Statement
//     / DO Statement WHILE LPAR Expression RPAR SEMI
//     / FOR LPAR Expression? SEMI Expression? SEMI Expression? RPAR Statement
//     / FOR LPAR Declaration Expression? SEMI Expression? RPAR Statement

// JumpStatement
//    <- GOTO Identifier SEMI
//     / CONTINUE SEMI
//     / BREAK SEMI
//     / RETURN Expression? SEMI

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
            struct ArgList *args;
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
               display_arglist(out, expr->as.call.args) &&
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

struct ArgList
{
    struct Expr *expr;
    struct ArgList *next;
};

bool display_arglist(FILE *out, struct ArgList *list)
{
    if (!list)
        return true;

    if (!list->next)
        return display_expr(out, list->expr);

    return display_expr(out, list->expr) &&
           fprintf_s(out, ", ") >= 0 &&
           display_arglist(out, list->next);
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
            struct ArgList *arg_list = parse_argument_expression_list(lxr);
            if (!lexer_expect(lxr, TOK_CLOSE_PAREN))
                return NULL;

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
struct ArgList *parse_argument_expression_list(struct Lexer *lxr)
{
    struct ArgList *first, *last;
    struct Expr *expr;

    if (!(expr = parse_assignment_expression(lxr)))
        return (struct ArgList *)NULL;

    first = malloc(sizeof(*first));
    first->expr = expr;
    first->next = NULL;

    last = first;

    while (lexer_accept(lxr, TOK_COMMA))
    {
        struct ArgList *new_args;
        if ((expr = parse_assignment_expression(lxr)))
        {
            new_args = malloc(sizeof(*new_args));
            new_args->expr = expr;
            new_args->next = NULL;

            last->next = new_args;
            last = new_args;
            continue;
        }
        break;
    }

    return first;
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
            struct Lexer saved_lxr = *lxr; // TODO: find a fix for this problem
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
            (type = parse_type_name(lxr)) &&
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
struct Expr *parse_cast_expression(struct Lexer *lxr)
{
    {
        struct Lexer saved_lxr = *lxr; // HACK: is there a better way?
        {
            struct Type *type;
            struct Expr *expr;
            if (lexer_accept(lxr, TOK_OPEN_PAREN) &&
                (type = parse_type_name(lxr)) &&
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
    lxr = lexer_init("  my_func(1, 2, 3) ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  my_func(1, 2, 3, ) ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  my_func() ");
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

    printf("\n");
    lxr = lexer_init("  sizeof(*expr)  ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  sizeof(void)  ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  (int) true ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  1 + 2 - 3 + 4 ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  100 >= 0 ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  100 != 0 ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  100 != 0 && -4 < -3  ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  100 != 0 || -4 < -3  ");
    if ((expr = parse_expression(&lxr)))
        display_expr(stdout, expr);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  **ptr_ptr = *ptr = 123  ");
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
    if ((type = parse_type_specifier(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  bool  ");
    if ((type = parse_type_specifier(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  char  ");
    if ((type = parse_type_specifier(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  void  ");
    if ((type = parse_type_specifier(&lxr)))
        display_type(stdout, type);
    else
        perror("TEST FAILED");
}

void test_parse_stmts()
{
}

void test_parse_decls()
{
    struct Lexer lxr;
    struct Decl *decl;

    printf("\n");
    lxr = lexer_init("  #include <stdio.h>  ");
    if ((decl = parse_decl_pound_include(&lxr)))
        display_decl(stdout, decl);
    else
        perror("TEST FAILED");

    printf("\n");
    lxr = lexer_init("  #include \"my_file.h\"  ");
    if ((decl = parse_decl_pound_include(&lxr)))
        display_decl(stdout, decl);
    else
        perror("TEST FAILED");
}

void test_parser()
{
    printf("%s\n", "Running parser tests...");
    test_parse_exprs();
    test_parse_types();
    test_parse_stmts();
    test_parse_decls();
    printf("\n\n%s\n", "Tests finished!");
}
