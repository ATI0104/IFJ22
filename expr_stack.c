#include "expr_stack.h"



void stackInit(TStack *stack) {
    stack->first = NULL;
    stack->totalAmount = 0;
    stackPush(stack, EXPR_DOLLAR, NULL);
}

void stackPush(TStack *stack, precedencyInput input, token *tok) {
    StackItem *elem = malloc(sizeof(StackItem));
    if (elem == NULL) {
        exit(99);
    }
    elem->item = input;
    elem->next = NULL;
    elem->tok = tok;
    if (stack->totalAmount == 0) {
        stack->first = elem;
    }
    else {
        elem->next = stack->first;
        stack->first = elem;
    }
    stack->totalAmount++;

}

void stackPop(TStack *stack) {
    StackItem *tmp;
    if (stack->first != NULL) {
        tmp = stack->first;
        if (stack->totalAmount == 1) {  // Only one item is in stack
            stack->first = NULL;
            stack->totalAmount = 0;
        }
        else {
            stack->first = stack->first->next;
            stack->totalAmount--;
        }
        //freeToken(tmp->tok);
        free(tmp);
    }
}

void stackInsertHandle(TStack *stack) {
    StackItem *tmp = stack->first;
    if (tmp->item != EXPR_NON_TERMINAL) {
        stackPush(stack, EXPR_HANDLE, NULL);
    }
    else {
        StackItem *elem = malloc(sizeof(StackItem));
        if (elem == NULL) {
            exit(99);
        }
        elem->item = EXPR_HANDLE;
        elem->next = NULL;
        while (tmp->next->item == EXPR_NON_TERMINAL) {
            tmp = tmp->next;
        }
        elem->next = tmp->next;
        tmp->next = elem;
        stack->totalAmount++;
    }
}

void deleteNearestItem(TStack *stack, precedencyInput item) {
    StackItem *tmp = stack->first;
    StackItem *prev = NULL;
    if (tmp->item == item) {
        stackPop(stack);
    }
    else {
        while (tmp->item != item) {
            if (tmp == NULL) {
                exit(2);
            }
            prev = tmp;
            tmp = tmp->next;
        }
        if (stack->totalAmount == 1) {
            stackPop(stack);
        }
        else {
            prev->next = tmp->next;
            //freeToken(tmp->tok);
            free(tmp);
        }
    }
}

precedencyInput stackTopTerminal(TStack *stack) {
    if (stack->totalAmount == 1) {
        return stack->first->item;
    }
    else {
        StackItem *tmp = stack->first;
        while (tmp->next != NULL) {
            if (tmp->item != EXPR_NON_TERMINAL && tmp->item != EXPR_HANDLE) {
                return tmp->item;
            }
            tmp = tmp->next;
        }
        return EXPR_ERROR;
    }
}

void stackDispose(TStack *stack) {
    if (stack->first != NULL) {
        StackItem *tmp = stack->first;
        StackItem *tmp2 = NULL;
        while (tmp != NULL) {
            tmp2 = tmp;
            tmp = tmp->next;
            //freeToken(tmp2->tok);
            free(tmp2);
            stack->totalAmount--;
        }
        stack->first = NULL;
    }
}