#include <stdio.h>  // fprintf_s, FILE, stdout
#include <stdlib.h> // atoi, malloc, free
#include "lex.h"    // lex, lex_all_input

// FORWARD DECLARATIONS
struct Type *parse_type_specifier(struct Lexer *);
struct Expr *parse_constant(struct Lexer *);
struct Expr *parse_expression(struct Lexer *);
struct Expr *parse_cast_expression(struct Lexer *);

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

int display_decl(FILE *out, struct Decl *decl)
{
    switch (decl->tag)
    {
    case DECL_POUND_INCLUDE:
        fprintf_s(out, "#include ");
        if (decl->as.pound_include.kind == POUND_INCLUDE_ANGLE_BRACKETS)
        {
            return fprintf_s(out, "<%s>", decl->as.pound_include.filename);
        }
        else
        {
            return fprintf_s(out, "\"%s\"", decl->as.pound_include.filename);
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
    printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__);
    exit(1);
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

// TypedefName <-Identifier #{&TypedefName}

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

int display_stmt(struct Stmt *stmt)
{
    printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__);
    exit(1);
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
    printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__);
    exit(1);
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
        printf("display_expr is not implemented for EXPR tag %d!\n", expr->tag);
        exit(1);
    }
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
//       / LPAR ArgumentExpressionList? RPAR  # :TODO
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
        // TODOOOOOOOOOOOOOOOOOOOOOOOOO

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
//      <- AssignmentExpression (COMMA AssignmentExpression)*
struct Expr *parse_argument_expression_list(struct Lexer *lxr)
{
    printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__);
    exit(1);
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
//     / PLUS
//     / MINUS
//     / TILDA
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
        if ((expr = parse_unary_expression(lxr)))
        {
            // TODOOOOOOOOOOOO
            printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__);
            exit(1);
        }

        struct Type *type;
        if (lexer_expect(lxr, TOK_OPEN_PAREN) &&
            (type = parse_type_name(lxr)) &&
            lexer_expect(lxr, TOK_CLOSE_PAREN))
        {
            // TODOOOOOOOOOOOO
            printf("%s:%d (%s) not yet implemented!\n", __FILE__, __LINE__, __func__);
            exit(1);
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
    // TODO: actually handle cast expressions.
    return parse_unary_expression(lxr);
}

// MultiplicativeExpression
//      <- CastExpression ((STAR / DIV / MOD) CastExpression)*
struct Expr *parse_multiplicative_expression(struct Lexer *lxr)
{
    return parse_cast_expression(lxr);
}

// AdditiveExpression
//      <- MultiplicativeExpression ((PLUS / MINUS) MultiplicativeExpression)*
struct Expr *parse_additive_expression(struct Lexer *lxr)
{
    return parse_multiplicative_expression(lxr);
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
    return parse_shift_expression(lxr);
}

// EqualityExpression
//      <- RelationalExpression ((EQUEQU / BANGEQU) RelationalExpression)*
struct Expr *parse_equality_expression(struct Lexer *lxr)
{
    return parse_relational_expression(lxr);
}

// ANDExpression <- EqualityExpression (AND EqualityExpression)*

// ExclusiveORExpression <- ANDExpression (HAT ANDExpression)*

// InclusiveORExpression <- ExclusiveORExpression (OR ExclusiveORExpression)*

// LogicalANDExpression
//      <- InclusiveORExpression (ANDAND InclusiveORExpression)*
struct Expr *parse_logical_and_expression(struct Lexer *lxr)
{
    return parse_equality_expression(lxr);
}

// LogicalORExpression
//      <- LogicalANDExpression (OROR LogicalANDExpression)*
struct Expr *parse_logical_or_expression(struct Lexer *lxr)
{
    return parse_logical_and_expression(lxr);
}

// ConditionalExpression
//      <- LogicalORExpression (QUERY Expression COLON LogicalORExpression)*
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
//     / STAREQU
//     / DIVEQU
//     / MODEQU
//     / PLUSEQU
//     / MINUSEQU
//     / LEFTEQU
//     / RIGHTEQU
//     / ANDEQU
//     / HATEQU
//     / OREQU
struct Expr *parse_assignment_expression(struct Lexer *lxr)
{
    return parse_conditional_expression(lxr);
}

// Expression
//      <- AssignmentExpression (COMMA AssignmentExpression)*
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
