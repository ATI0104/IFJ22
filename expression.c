#include "expression.h"

expr *expression;
bool endingFunctionBracket = false;

const precendencyOperation table[16][16] = {

    //|+-|*/|.| r | (| )| i| $ | Input        r - relational operators (<, <=,
    //>, >=, ===, !==)

    //                             Stack
    {R, S, R, R, S, R, S, R},  // +-
    {R, R, R, R, S, R, S, R},  // */
    {S, S, S, R, S, R, S, R},  // . 
    {S, S, S, R, S, R, S, R},  // r
    {S, S, S, S, S, E, S, N},  // (
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

precedencyInput GetExpressionType(expr *exp) {
if(exp->op){
  if(*exp->op == _plus || *exp->op == _minus){
    return EXPR_PLUS_MINUS;
  }else if(*exp->op == _multiply || *exp->op == _divide){
    return EXPR_MULDIV;
  }else if (*exp->op == _dot){
    return EXPR_CONCAT;
  }else if(*exp->op == _lessthan || *exp->op == _lessthanoreq || 
        *exp->op == _greaterthan ||* exp->op == _greaterthanoreq || 
        *exp->op == _typecheck ||* exp->op == _not_typecheck){
          return EXPR_REL;
  }else if(*exp->op == _left_parenthesis){
    return EXPR_L_BRACKET;
  }else if(*exp->op == _right_parenthesis){
    return EXPR_R_BRACKET;
  }
}
  else if(exp->fl || exp->num || exp->str!=NULL || exp->var!=NULL || exp->typekeywords != NULL || exp->func != NULL ){
    return EXPR_ID;
  } 
  else{
    return EXPR_ERROR;
  }
return EXPR_ERROR;
}

void insertAfter(expr *exp) {
  expr *elem;
   maloc(elem, sizeof(expr));
  if (elem == NULL) {
    exit(99);  
  }
  elem->first = exp->first;
  elem->next = NULL;
  if (expression == NULL) {
    expression = elem;
  } else {
    expr *tmp = expression;
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
    free(possibleRule[0]->exp);
    free(possibleRule[2]->exp);
    deleteNearestItem(stack, EXPR_R_BRACKET);
    deleteNearestItem(stack, EXPR_L_BRACKET);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_PLUS_MINUS &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->exp);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_PLUS_MINUS);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_MULDIV &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->exp);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_MULDIV);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_REL &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->exp);
    deleteNearestItem(stack, EXPR_NON_TERMINAL);
    deleteNearestItem(stack, EXPR_REL);
    deleteNearestItem(stack, EXPR_HANDLE);
    return true;
  } else if (possibleRule[0]->item == EXPR_NON_TERMINAL &&
             possibleRule[1]->item == EXPR_CONCAT &&
             possibleRule[2]->item == EXPR_NON_TERMINAL) {
    insertAfter(possibleRule[1]->exp);
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
      insertAfter(possibleRule[0]->exp);
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

bool parseExpression(expr *exp) {
  endingFunctionBracket = false;
  bool error = false;
  bool precedencyOk;
  expression = NULL;
  TStack stack;
  stackInit(&stack);
  while (exp != NULL) { 
    precedencyInput input = GetExpressionType(exp);
    precedencyInput opOnStack = stackTopTerminal(&stack);
    precendencyOperation op = getOperationFromTable(opOnStack, input);
    if (op == S) {
      stackInsertHandle(&stack);
      stackPush(&stack, input, exp);
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
        stackPush(&stack, input, exp);
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
        stackPush(&stack, input, exp);
      }
    } else if (op == E) {
      reduce(&stack);
      stackPush(&stack, input, exp);
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
    exp = exp->next;
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
  precedencyOk = !error;
  if (endingFunctionBracket) {
    precedencyOk = true;
  }
  return precedencyOk;
}
/******** EXPRESSION STACK OPERATIONS ******/
void stackInit(TStack *stack) {
  stack->first = NULL;
  stack->totalAmount = 0;
  stackPush(stack, EXPR_DOLLAR, NULL);
}

void stackPush(TStack *stack, precedencyInput input, expr* exp) {
  StackItem *elem ; 
  maloc(elem, sizeof(StackItem));
  if (elem == NULL) {
    exit(99);
  }
  elem->item = input;
  elem->next = NULL;
  elem->exp = exp;
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
    if (stack->totalAmount == 1) { 
      stack->first = NULL;
      stack->totalAmount = 0;
    } else {
      stack->first = stack->first->next;
      stack->totalAmount--;
    }
    free(tmp);
  }
}

void stackInsertHandle(TStack *stack) {
  StackItem *tmp = stack->first;
  if (tmp->item != EXPR_NON_TERMINAL) {
    stackPush(stack, EXPR_HANDLE, NULL);
  } else {
    StackItem *elem;
    maloc(elem, sizeof(StackItem));
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
      free(tmp2);
      stack->totalAmount--;
    }
    stack->first = NULL;
  }
}
