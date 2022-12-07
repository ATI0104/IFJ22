/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Attila Kovács (xkovac60)
  * @file stack.c
*/
#include "stack.h"
//Heavy inspiration taken from my IAL project
void Stack_Init(Stack** stack) {
  if (stack == NULL) return;
  maloc(*stack, sizeof(Stack));
  (*stack)->topIndex = -1;  // topIndex = -1 marks empty stack
  (*stack)->size = 0;
  return;
}
int Stack_IsEmpty(const Stack* stack) {
  return stack->topIndex == -1;  //-1 indicates an empty stack
}
void Stack_Top(const Stack* stack, int* dataPtr) {
  if (Stack_IsEmpty(stack) != 0) {  // Error checking
    eprint("Stack_Top used on empty stack\n");
    exit(99);
    return;
  }
  *dataPtr = stack->array[stack->topIndex];  // Setting dataPtr
  return;
}
void Stack_Pop(Stack* stack) {
  if (!Stack_IsEmpty(
          stack)) {  // popping an empty stack would cause a segmentation fault
                     // when trying to push data into the stack
    stack->array[stack->topIndex--] = 0;  // Clears the top of the stack and
                                          // decrements topIndex
  }
}
void Stack_Push(Stack** stack, int data) {
  if ((*stack)->size == 0) {
    maloc((*stack)->array, sizeof(int));
    (*stack)->size = 1;
  } else if ((*stack)->size < (*stack)->topIndex + 1) {
    (*stack)->array =
        realloc((*stack)->array, (++(*stack)->size) * sizeof(int));
  }
  (*stack)->array[++(*stack)->topIndex] = data;  // Pushing data into the stack
  return;
}
void Stack_Destroy(Stack** stack) { free((*stack)->array); }
