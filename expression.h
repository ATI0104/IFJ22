#ifndef _expression
#define _expression

#include <stdbool.h>

#include "ast.h"
#include "scanner.h"
#include "stack.h"

typedef enum {  
  EXPR_PLUS_MINUS,
  EXPR_MULDIV,
  EXPR_CONCAT,
  EXPR_REL,
  EXPR_L_BRACKET,
  EXPR_R_BRACKET,
  EXPR_ID,  // Variable, string, float, integer
  EXPR_DOLLAR,
  EXPR_STRLEN,
  EXPR_NON_TERMINAL,
  EXPR_HANDLE,
  EXPR_ERROR
} precedencyInput;

typedef enum {
  S,  // shift
  R,  // reduce
  E,  // equals 
  N   // error
} precendencyOperation;


typedef struct stackIt {
  precedencyInput item;
  expr *exp;
  struct stackIt *next;
} StackItem;

typedef struct stack {
  StackItem *first;
  int totalAmount;
} TStack;


precedencyInput GetExpressionType(expr *exp);
precendencyOperation getOperationFromTable(precedencyInput onStack, precedencyInput onInput);
void insertAfter(expr *exp);
bool reduceDyadicOperation(TStack *stack, StackItem **possibleRule);
bool reduceIdentifier(TStack *stack, StackItem **possibleRule);
bool reduce(TStack *stack);
bool parseExpression(expr *exp);


/****EXPRESSION STACK OPERATIONS ***/
void stackInit(TStack *stack);
void stackPush(TStack *stack, precedencyInput input, expr *exp);
void stackPop(TStack *stack);
void stackInsertHandle(TStack *stack);
precedencyInput stackTopTerminal(TStack *stack);
void deleteNearestItem(TStack *stack, precedencyInput item);
void stackDispose(TStack *stack);
#endif
