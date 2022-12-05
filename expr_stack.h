#ifndef _expr_stack
#define _expr_stack

#include <stdio.h>
#include <stdlib.h>

#include "expression.h"
#include "scanner.h"
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

#endif