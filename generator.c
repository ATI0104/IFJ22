#include "generator.h"

my_favorites fav;

void generate(AST* a, function_table* f) {
  printf(".IFJcode22\n");
  while (strcmp(a->funname->txt, "m@in") != 0) {
    a = a->next;
  }
  generate_code(a->Code, f, NULL);
  a = a->first;
  while (strcmp(a->funname->txt, "m@in") != 0) {
    generate_code(a->Code, f, a->funname);
    a = a->next;
  }
}

void generate_code(code* c, function_table* f, string* name) {
  varlist* v = NULL;
  if (name != NULL) {
    printf("DPRINT string@%s", name->txt);
    printf("LABEL %s", name->txt);
  } else {
    printf("DPRINT string@main");
  }
  while (c != NULL) {
    if (c->i) {
      generate_if(c->i, c->e, c->expression, f, name);
    } else if (c->loop) {
      generate_while(c->loop, c->expression, f, name);
    } else if (c->var) {
      generate_assignment(c->var, c->expression, f, name, &v);
    } else if (c->ret) {
      generate_return(c->expression, f, name, v);
    } else if (c->expression) {
      generate_expression(c->expression, f, name, v);
    } else if (c->jmp) {
      generate_call(c->jmp, f, name, v);
    }
    c = c->next;
  }
}

void generate_if(code* ifpart, code* elsepart, expr* e, function_table* f,
                 string* functionname) {}

void generate_while(code* loop, expr* e, function_table* f,
                    string* functionname) {}

void generate_assignment(string* var, expr* e, function_table* f,
                         string* functionname, varlist** v) {
  printf("DEFVAR LF@%s", var->txt);
  varlist_set(v, var, get_expression_type(e, *v, f));
  evaluate_expression(e, f, functionname, *v, var);
}

void generate_return(expr* e, function_table* f, string* functionname,
                     varlist* v) {}

void evaluate_expression(expr* e, function_table* f, string* functionname,
                         varlist* v, string* tovar) {
  Stack* s;
  Stack_Init(&s);
  while (e != NULL) {
    if (e->num) {
      printf("PUSHS int@%d\n", *(e->num));
      Stack_Push(&s, _int);
    } else if (e->str) {
      printf("PUSHS string@%s, e->str->txt");
      Stack_Push(&s, _string);
    } else if (e->fl) {
      printf("PUSHS float@%a", *(e->fl));
      Stack_Push(&s, _float);
    } else if (e->var) {
      int type = varlist_get(v, e->var);
      switch (type) {
        case _int:
          printf("PUSHS LF@%s\n", e->var->txt);
          Stack_Push(&s, _int);
          break;
        case _float:
          printf("PUSHS LF@%s\n", e->var->txt);
          Stack_Push(&s, _float);
          break;
        case _string:
          printf("PUSHS LF@%s\n", e->var->txt);
          Stack_Push(&s, _string);
          break;
        case _void:
          printf("PUSHS LF@%s\n", e->var->txt);
          Stack_Push(&s, _null);
          break;
        case _bool:
          printf("PUSHS LF@%s\n", e->var->txt);
          Stack_Push(&s, _bool);
          break;
        default:
          break;
      }
    } else if (e->op) {  // TODO add all operators & verify that both of the
                         // operands are of the same type if not convert them
      switch (*(e->op)) {
        case _plus:
          Stack_Pop(s);
          Stack_Pop(s);
          printf("ADDS\n");
        case _minus:
          Stack_Pop(s);
          Stack_Pop(s);
          printf("SUBS\n");
          break;
        case _multiply:
          Stack_Pop(s);
          Stack_Pop(s);
          printf("MULS\n");
          break;
        case _divide:
          Stack_Pop(s);
          Stack_Pop(s);
          printf("DIVS\n");
          break;
        default:
          break;
      }
    }
    e = e->next;
  }
  if (!Stack_IsEmpty(s)) {
    Stack_Pop(s);
    printf("POPS LF@%s\n", tovar->txt);
  }
}