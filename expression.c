#include "expression.h"

tlist *exprlist;
bool endingFunctionBracket = false;

void printStack(TStack *stack) {
  StackItem *tmp = stack->first;
  while (tmp != NULL) {
    printf("%d\n", tmp->item);
    tmp = tmp->next;
  }
  printf("\n\n");
}
const precendencyOperation table[16][16] = {

    //|+-|*/|.| r | (| )| i| $ | Input        r - relational operators (<, <=,
    //>, >=, ===, !==)

    //                             Stack
    {R, S, R, R, S, R, S, R},  // +-
    {R, R, R, R, S, R, S, R},  // */
    {S, S, S, R, S, R, S, R},  // . TODO
    {S, S, S, N, S, R, S, R},  // r
    {S, S, S, S, S, Q, S, N},  // (
    {R, R, R, R, N, R, N, R},  // )
    {R, R, R, R, N, R, N, R},  // i
    {S, S, S, S, S, N, S, N},  // $
};

precendencyOperation getOperationFromTable(precedencyInput onStack,
                                           precedencyInput onInput) {
  if (onStack == EXPR_NON_TERMINAL || onStack == EXPR_HANDLE ||
      onStack == EXPR_ERROR || onInput == EXPR_NON_TERMINAL ||
      onInput == EXPR_HANDLE || onInput == EXPR_ERROR) {
    return N;
  } else {
    return table[onStack][onInput];
  }
}

precedencyInput convertTokenTypeToExpressionType(token *tok) {
  int type = tok->type;
  switch (type) {
    case _minus:
    case _plus:
      return EXPR_PLUS_MINUS;

    case _multiply:
    case _divide:
      return EXPR_MULDIV;

    case _dot:
      return EXPR_CONCAT;

    case _left_parenthesis:
      return EXPR_L_BRACKET;

    case _right_parenthesis:
      return EXPR_R_BRACKET;

    case _lessthan:
    case _lessthanoreq:
    case _greaterthan:
    case _greaterthanoreq:
    case _equals:
    case _typecheck:
    case _not_typecheck:
      return EXPR_REL;

    case _identificator:
    case _int:
    case _float:
    case _string:
      return EXPR_ID;

    default:
      return EXPR_ERROR;
  }
}
/*
bool endOfExpression(token *tok) {
  if  {
    return true;
  }
  return false;
}
*/
void insertAfter(token *tok) {
  tlist *elem = malloc(sizeof(tlist));
  if (elem == NULL) {
    exit(99);  //?
  }
  elem->t = *tok;
  elem->next = NULL;
  if (exprlist == NULL) {
    exprlist = elem;
  } else {
    tlist *tmp = exprlist;
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = elem;
  }
}

bool reduceDyadicOperation(TStack *stack,
                           StackItem **possibleRule) {  //(a+b, a*b, (a), ...)
  if (possibleRule[0]->item == EXPR_R_BRACKET &&
      possibleRule[1]->item == EXPR_NON_TERMINAL &&
      possibleRule[2]->item == EXPR_L_BRACKET) {
    free(possibleRule[0]->tok);
    free(possibleRule[2]->tok);
    deleteNearestItem(stack, EXPR_R_BRACKET);
    deleteNearestItem(stack, EXPR_L_BRACKET);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_PLUS_MINUS &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->tok);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_PLUS_MINUS);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_MULDIV &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->tok);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_MULDIV);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_REL &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->tok);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_REL);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_CONCAT &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->tok);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_CONCAT);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else {
    return false;
  }
}

bool reduceIdentifier(TStack *stack, StackItem **possibleRule) {
  if (possibleRule[0]->item == EXPR_ID ||
      possibleRule[0]->item == EXPR_NON_TERMINAL) {
    if (possibleRule[0]->item == EXPR_ID) {
      insertAfter(possibleRule[0]->tok);
    }
    StackItem *tmp = stack->first;
    while (tmp->next->item != EXPR_HANDLE) {
      if (tmp == NULL) {
        return false;
      }
      tmp = tmp->next;
    }
    tmp->item = EXPR_NON_TERMINAL;
    deleteNearestItem(stack, EXPR_HANDLE);
  } else {
    return false;
  }
  return true;
}

bool reduce(TStack *stack) {
  int terminalsBeforeHandle = 0;
  StackItem *possibleRule[3];
  StackItem **possibleRulePointer = possibleRule;
  if (stack->totalAmount > 1) {
    StackItem *tmp = stack->first;
    while (tmp->item != EXPR_HANDLE) {
      terminalsBeforeHandle++;
      if (terminalsBeforeHandle >= 4 || terminalsBeforeHandle == 0) {
        return false;
      }
      possibleRule[terminalsBeforeHandle - 1] = tmp;
      if (tmp == NULL || tmp->next == NULL) {
        return false;
      }
      tmp = tmp->next;
    }
  }
  if (terminalsBeforeHandle == 1) {  // Identifier is reduced to expression
    if (!reduceIdentifier(stack, possibleRulePointer)) {
      return false;
    }
  } else if (terminalsBeforeHandle == 3) {
    if (!reduceDyadicOperation(stack, possibleRulePointer)) {
      return false;
    }
  }
  return true;
}

precedencyAnalysisReturn parseExpression(tlist *tok) {
  endingFunctionBracket = false;
  bool error = false;
  //token *tok = tokens->t;
  precedencyAnalysisReturn precedencyOutput;
  exprlist = NULL;
  TStack stack;
  stackInit(&stack);
  while (tok->t.type != _EOF || tok->t.type != _semicolon) {
    precedencyInput input = convertTokenTypeToExpressionType(&tok->t);
    precedencyInput opOnStack = stackTopTerminal(&stack);
    precendencyOperation op = getOperationFromTable(opOnStack, input);
    if (op == S) {
      stackInsertHandle(&stack);
      stackPush(&stack, input, &tok->t);
    } else if (op == R) {
      while (op == R) {
        if (!reduce(&stack)) {
          error = true;
          break;
        }
        opOnStack = stackTopTerminal(&stack);
        op = getOperationFromTable(opOnStack, input);
      }
      if (input != EXPR_R_BRACKET) {
        stackInsertHandle(&stack);
        stackPush(&stack, input, &tok->t);
      } else if (input == EXPR_R_BRACKET) {
        while (1) {
          if (stack.first->next->item == EXPR_L_BRACKET &&
              stack.first->item == EXPR_NON_TERMINAL) {
            break;
          }
          if (!reduce(&stack)) {
            endingFunctionBracket = true;
            error = true;
            break;
          }
        }
        stackPush(&stack, input, &tok->t);
      }
    } else if (op == Q) {
      reduce(&stack);
      stackPush(&stack, input, &tok->t);
    } else if (op == N) {
      if ((input == EXPR_ID && opOnStack == EXPR_ID) ||
          (input == EXPR_ID && opOnStack == EXPR_R_BRACKET)) {
        break;
      }
      error = true;
    }
    if (endingFunctionBracket) {
      break;
    }
    tok = tok->next;
  }
  while (!error) {
    if (stack.first->item == EXPR_NON_TERMINAL &&
        stack.first->next->item == EXPR_DOLLAR) {
      break;
    }
    if (!reduce(&stack)) {
      error = true;
    }
  }
  stackDispose(&stack);
  precedencyOutput.lastToken = &tok->t;
  precedencyOutput.ok = !error;
  if (endingFunctionBracket) {
    precedencyOutput.ok = true;
  }
  precedencyOutput.tok_list = exprlist;
  return precedencyOutput;
}
//********EXPRESSION STACK OPERATIONS ******
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
  } else {
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
    } else {
      stack->first = stack->first->next;
      stack->totalAmount--;
    }
    // freeToken(tmp->tok);
    free(tmp);
  }
}

void stackInsertHandle(TStack *stack) {
  StackItem *tmp = stack->first;
  if (tmp->item != EXPR_NON_TERMINAL) {
    stackPush(stack, EXPR_HANDLE, NULL);
  } else {
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
  } else {
    while (tmp->item != item) {
      if (tmp == NULL) {
        exit(2);
      }
      prev = tmp;
      tmp = tmp->next;
    }
    if (stack->totalAmount == 1) {
      stackPop(stack);
    } else {
      prev->next = tmp->next;
      // freeToken(tmp->tok);
      free(tmp);
    }
  }
}

precedencyInput stackTopTerminal(TStack *stack) {
  if (stack->totalAmount == 1) {
    return stack->first->item;
  } else {
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
      // freeToken(tmp2->tok);
      free(tmp2);
      stack->totalAmount--;
    }
    stack->first = NULL;
  }
}
