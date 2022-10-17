#include "parser.h"

void AST_init(AST** a) {
  if (a == NULL) return;
  *a = NULL;
}
void AST_add(AST** a, string* f_name, code* code) {
  if (a == NULL) return;
  AST* tmp = *a;
  if (*a == NULL) {
    tmp = malloc(sizeof(AST));
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = malloc(sizeof(AST));
    tmp = tmp->next;
  }
  tmp->funname = f_name;
  tmp->code = code;
  tmp->next = NULL;
  tmp->first = *a;
}
void AST_destroy(AST** a) {
  if (a == NULL) return;
  AST* tmp = *a;
  if (tmp == NULL) return;
  AST* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    string_destroy(tmp->funname);
    code_destroy(&(tmp->code));
    free(tmp);
    *a = NULL;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  string_destroy(tmp_n->funname);
  code_destroy(&(tmp_n->code));
  free(tmp_n);
  tmp->next = NULL;
  AST_destroy(a);
}
void code_init(code** c) {
  if (c == NULL) return;
  *c = NULL;
}
void code_add(code** c, int lnum, code* i, code* e, code* loop, expr* exp,
              call* jmp) {
  if (c == NULL) return;
  code* tmp = *c;
  if (*c == NULL) {
    tmp = malloc(sizeof(code));
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = malloc(sizeof(code));
    tmp = tmp->next;
  }
  tmp->line_num = lnum;
  tmp->i = i;
  tmp->e = e;
  tmp->loop = loop;
  tmp->expression = exp;
  tmp->jmp = jmp;
  tmp->first = *c;
  tmp->next = NULL;
}

void code_destroy(code** c) {
  if (c == NULL) return;
  code* tmp = *c;
  if (tmp == NULL) return;
  code* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    code_destroy(&(tmp->i));
    code_destroy(&(tmp->e));
    code_destroy(&(tmp->loop));
    expr_destroy(&(tmp->expression));
    call_destroy(&(tmp->jmp));
    free(tmp);
    *c = NULL;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  code_destroy(&(tmp_n->i));
  code_destroy(&(tmp_n->e));
  code_destroy(&(tmp_n->loop));
  expr_destroy(&(tmp_n->expression));
  call_destroy(&(tmp_n->jmp));
  free(tmp_n);
  tmp->next = NULL;
  code_destroy(c);
}
void expr_init(expr** e) {
  if (e == NULL) return;
  *e = NULL;
}
void expr_add(expr** e, int type, string* str, int* num, double* fl, int* op,
              string* var, call* func) {
  if (e == NULL) return;
  expr* tmp = *e;
  if (*e == NULL) {
    tmp = malloc(sizeof(expr));
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = malloc(sizeof(expr));
    tmp = tmp->next;
  }
  tmp->type = type;
  tmp->str = str;
  tmp->num = num;
  tmp->fl = fl;
  tmp->op = op;
  tmp->var = var;
  tmp->func = func;
  tmp->first = *e;
  tmp->next = NULL;
}

void expr_destroy(expr** e) {
  if (e == NULL) return;
  expr* tmp = *e;
  if (tmp == NULL) return;
  expr* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    string_destroy(tmp->str);
    string_destroy(tmp->var);
    if (tmp->num != NULL) free(tmp->num);
    if (tmp->fl != NULL) free(tmp->fl);
    if (tmp->op != NULL) free(tmp->op);
    call_destroy(&(tmp->func));
    free(tmp);
    *e = NULL;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  string_destroy(tmp_n->str);
  string_destroy(tmp_n->var);
  if (tmp_n->num != NULL) free(tmp_n->num);
  if (tmp_n->fl != NULL) free(tmp_n->fl);
  if (tmp_n->op != NULL) free(tmp_n->op);
  call_destroy(&(tmp_n->func));
  free(tmp_n);
  tmp->next = NULL;
  expr_destroy(e);
}

void call_init(call** c) {
  if (c == NULL) return;
  *c = NULL;
}
void call_create(call** c, string* f_name, input* in, output* out) {
  if (c == NULL) return;
  call* tmp = *c;
  if (tmp == NULL) tmp = malloc(sizeof(call));
  tmp->function_name = f_name;
  tmp->in = in;
  tmp->out = out;
}
void call_destroy(call** c) {
  if (c == NULL) return;
  if (*c == NULL) return;
  call* tmp = *c;
  string_destroy(tmp->function_name);
  input_destroy(&(tmp->in));
  input_destroy(&(tmp->out));
  free(tmp);
  *c = NULL;
}
void input_init(input** in) {
  if (in == NULL) return;
  *in = NULL;
}
void input_add(input** in, int* i, double* f, string* s, string* var) {
  if (in == NULL) return;
  input* tmp = *in;
  if (*in == NULL) {
    tmp = malloc(sizeof(input));
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = malloc(sizeof(input));
    tmp = tmp->next;
  }
  tmp->i = i;
  tmp->f = f;
  tmp->s = s;
  tmp->var = var;
  tmp->next = NULL;
  tmp->first = *in;
}
void input_destroy(input** in) {
  if (in == NULL) return;
  input* tmp = *in;
  if (tmp == NULL) return;
  input* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    if (tmp->i != NULL) free(tmp->i);
    if (tmp->f != NULL) free(tmp->f);
    string_destroy(tmp->s);
    string_destroy(tmp->var);
    free(tmp);
    *in = NULL;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  if (tmp_n->i != NULL) free(tmp_n->i);
  if (tmp_n->f != NULL) free(tmp_n->f);
  string_destroy(tmp_n->s);
  string_destroy(tmp_n->var);
  free(tmp_n);
  tmp->next = NULL;
  input_destroy(in);
}
expr* expr_pop(expr** e) {
  if (e == NULL) return NULL;
  if (*e == NULL) return NULL;
  expr* tmp = *e;
  *e = (*e)->next;
  tmp->next = NULL;
  return tmp;
}
int check_precedence(int* op) {
  switch (*op) {
    case _multiply:
    case _divide:
      return 4;
    case _plus:
    case _minus:
    case _dot:
      return 3;
      break;
    case _lessthan:
    case _greaterthan:
    case _lessthanoreq:
    case _greaterthanoreq:
      return 2;
    case _typecheck:
    case _not_typecheck:
      return 1;
    default:
      eprint("Invalid operator.\n");
      break;
  }
  eprint("Invalid operator.\n");
  exit(LEXICAL_ERROR);  // Not sure if this is the right error code
}

void expr_topostfix(expr** e) {
  if (e == NULL) return;
  // TODO
}
/*
void expr_reverse(expr** e) {
  if (e == NULL) return;
  if (*e == NULL) return;
  expr* prev = NULL;
  expr* curr = *e;
  expr* next = NULL;
  while (curr != NULL) {
    if (curr->op != NULL) {
      switch (*(curr->op)) {
        case _left_parenthesis:
          *(curr->op) = _right_parenthesis;
          break;
        case _right_parenthesis:
          *(curr->op) = _left_parenthesis;
      }
    }
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  *e = prev;
}*/