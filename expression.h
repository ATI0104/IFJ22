#ifndef _expression
#define _expression

#include "expr_stack.h"
#include "ast.h"
//#include "stack.h"

#include "scanner.h"

#include <stdbool.h>

typedef enum { // Don't change the order of the elements !!!
    EXPR_PLUS_MINUS,
    EXPR_MULDIV,
    EXPR_CONCAT,
    EXPR_REL,
    EXPR_L_BRACKET,
    EXPR_R_BRACKET,
    EXPR_ID, // Identifier, string, number
    EXPR_DOLLAR,
    EXPR_NON_TERMINAL,
    EXPR_HANDLE,
    EXPR_ERROR
}   precedencyInput;

typedef enum {
    S,  // shift
    R,  // reduce
    Q,  // equals (for brackets)
    N   // error
} precendencyOperation;

typedef struct {
  bool ok;
  tlist *tok_list;
  token *lastToken;
} precedencyAnalysisReturn;

typedef struct stackIt {
    precedencyInput item;
    token *tok;
    struct stackIt *next;
} StackItem;

typedef struct stack {
    StackItem *first;
    int totalAmount;
} TStack;

precendencyOperation getOperationFromTable(precedencyInput onStack, precedencyInput onInput);
precedencyInput convertTokenTypeToExpressionType(token *tok);
bool endOfExpression(token *tok);
void insertAfter(token *tok);
bool reduceDyadicOperation(TStack *stack, StackItem **possibleRule);
bool reduceIdentifier(TStack *stack, StackItem **possibleRule);
bool reduce(TStack *stack);
#endif