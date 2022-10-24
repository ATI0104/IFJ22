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
void Stack_Init(Stack**);
int Stack_IsEmpty(const Stack*);
int Stack_IsFull(const Stack*);
void Stack_Top(const Stack*, int*);
void Stack_Pop(Stack*);
void Stack_Push(Stack**, int);
void Stack_Destroy(Stack** stack);
#endif