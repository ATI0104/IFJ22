/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Anna Udvaros (xudvar02)
  * @file expression.h
*/
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

/**
 * @brief Returns expression type
 */
precedencyInput GetExpressionType(expr *exp);
/**
 * @brief Get the operation from precedency table
 */ 
precendencyOperation getOperationFromTable(precedencyInput onStack, precedencyInput onInput);
/**
 * @brief Inserts expression to the expression structure
 */
void insertAfter(expr *exp);
/**
 * @brief Reduces operations (a+b, a*b, (a),...)
 */
bool reduceOperation(TStack *stack, StackItem **possibleRule);
/**
 * @brief Reduces the identifier (Variable, string, float, integer)
 */
bool reduceIdentifier(TStack *stack, StackItem **possibleRule);
/**
 * @brief Reduces the expression
 */
bool reduce(TStack *stack);
/**
 * @brief Parses current expression
 */
bool parseExpression(expr *exp);


/****EXPRESSION STACK OPERATIONS ***/
/*
 * @brief Initialize stack
 */
void stackInit(TStack *stack);
/*
 * @brief Pushes an element to the stack
 */
void stackPush(TStack *stack, precedencyInput input, expr *exp);
/*
 * @brief Pops an element from stack
 */
void stackPop(TStack *stack);
/*
 * @brief Inserts handle in front of the first terminal it finds
 */
void stackInsertHandle(TStack *stack);
/*
 * @brief Returns the first terminal that occurs in the stack
 */
precedencyInput stackTopTerminal(TStack *stack);
/*
 * @brief Removes the nearest item from stack
 */
void deleteNearestItem(TStack *stack, precedencyInput item);
/*
 * @brief Removes all of the items from stack
 */
void stackDispose(TStack *stack);
#endif
