#ifndef _expression
#define _expression

#include <stdbool.h>

#include "ast.h"
#include "scanner.h"
#include "stack.h"

typedef enum {  // Don't change the order of the elements !!!
  EXPR_PLUS_MINUS,
  EXPR_MULDIV,
  EXPR_CONCAT,
  EXPR_REL,
  EXPR_L_BRACKET,
  EXPR_R_BRACKET,
  EXPR_ID,  // Identifier, string, number
  EXPR_DOLLAR,
  EXPR_NON_TERMINAL,
  EXPR_HANDLE,
  EXPR_ERROR
} precedencyInput;

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

/**
 * @brief Initialize stack
 *
 * @param stack Stack to initialize
 */

void stackInit(TStack *stack);

/**
 * @brief Pushes an element to the stack
 *
 * @param stack Stack to push to
 * @param input Value to push
 * @param tok Current token to store
 */
void stackPush(TStack *stack, precedencyInput input, token *tok);

/**
 * @brief Pops an element from stack
 *
 * @param stack Stack to pop from
 */

void stackPop(TStack *stack);

/**
 * @brief Inserts handle in front of the first terminal it finds
 *
 * @param stack Stack to insert to
 */
void stackInsertHandle(TStack *stack);

/**
 * @brief Returns the first terminal that occurs in the stack
 *
 * @param stack Stack where terminal should be found
 * @return Value of the first terminal that is found on the stack
 */
precedencyInput stackTopTerminal(TStack *stack);

/**
 * @brief Removes the nearest item from stack
 *
 * @param stack Stack to remove handle from
 * @param item Item to delete
 */
void deleteNearestItem(TStack *stack, precedencyInput item);

/**
 * @brief Removes all of the items from stack
 *
 * @param stack Stack to remove items from
 */
void stackDispose(TStack *stack);

precendencyOperation getOperationFromTable(precedencyInput onStack,
                                           precedencyInput onInput);
precedencyInput convertTokenTypeToExpressionType(token *tok);
bool endOfExpression(token *tok);
void insertAfter(token *tok);
bool reduceDyadicOperation(TStack *stack, StackItem **possibleRule);
bool reduceIdentifier(TStack *stack, StackItem **possibleRule);
bool reduce(TStack *stack);
#endif