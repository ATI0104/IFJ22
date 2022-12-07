/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Attila Kovács (xkovac60)
  * @file stack.h
*/
#ifndef _stack
#define _stack
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
typedef struct opstack {
  int* array;
  int topIndex;
  int size;
} Stack;
/**
* @brief Initializes stack
*/
void Stack_Init(Stack**);
/**
* @brief Checks if stack is empty
*/
int Stack_IsEmpty(const Stack*);
/**
* @brief Checks if stack is full
*/
int Stack_IsFull(const Stack*);
/**
* @brief Gets the top pointer of the stack
*/
void Stack_Top(const Stack*, int*);
/**
 * Pops a pointer from the stack.
 */
void Stack_Pop(Stack*);
/**
 * Adds a data to the stack.
 */
void Stack_Push(Stack**, int);
/**
 * Removes the entire stack
 */
void Stack_Destroy(Stack** stack);
#endif
