#include "generator.h"

my_favorites fav;
int count = 0;
void generate(AST* a, function_table* f) {
  printf(".IFJcode22\n");
  generate_exit_codes();
  prinf("DEFVAR GF@-_-returnvalue");
  while (strcmp(a->funname->txt, "m@in") != 0) {
    a = a->next;
  }
  define_variables(function_table_get(f, *a->funname));
  generate_code(a->Code, f, NULL, true, NULL);
  a = a->first;
  while (strcmp(a->funname->txt, "m@in") != 0) {
    define_variables(function_table_get(f, *a->funname));
    generate_code(a->Code, f, a->funname, true, NULL);
    a = a->next;
  }
}
void generate_exit_codes() {
  printf("LABEL -_-exit3");
  printf("EXIT int@3");
  printf("LABEL -_-exit4");
  printf("EXIT int@4");
  printf("LABEL -_-exit5");
  printf("EXIT int@5");
  printf("LABEL -_-exit6");
  printf("EXIT int@6");
  printf("LABEL -_-exit7");
  printf("EXIT int@7");
  printf("LABEL -_-exit8");
  printf("EXIT int@8");
}
void define_variables(function_table* f) { rec_define_variables(f->variable); }
void rec_define_variables(var_table* v) {
  if (v == NULL) {
    return;
  }
  rec_define_variables(v->left_var);
  rec_define_variables(v->right_var);
  printf("DEFVAR LF@%s\n", v->name.txt);
}
void generate_code(code* c, function_table* f, string* name, bool newfunction,
                   varlist* localvars) {
  varlist* v;
  if (newfunction)
    v = NULL;
  else
    v = localvars;

  if (name != NULL) {
    printf("LABEL %s", name->txt);
    printf("DPRINT string@%s", name->txt);

  } else {
    printf("DPRINT string@main");
  }
  while (c != NULL) {
    if (c->i) {
      generate_if(c->i, c->e, c->expression, f, name, v);
    } else if (c->loop) {
      generate_while(c->loop, c->expression, f, name, v);
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
                 string* functionname, varlist* v) {
  string countstr;
  string_init(&countstr);
  strinng_inttostr(&countstr, count++);
  string labelend;
  string_set(&labelend, functionname->txt);
  string_append(&labelend, "_end");
  string_append(&labelend, countstr.txt);
  string label;
  string_set(&label, functionname->txt);
  string_append(&label, "_else");
  string_append(&label, countstr.txt);
  printf("DEFVAR LF@%s", countstr.txt);
  evaluate_expression(e, f, functionname, v, &countstr);
  printf("JUMPIFEQ %s LF@%s nil@nil ", label, countstr);
  generate_code(ifpart, f, functionname, false, v);
  printf("JUMP %s", labelend);
  printf("LABEL %s", label);
  generate_code(elsepart, f, functionname, false, v);
  printf("LABEL %s", labelend);
  return;
}

void generate_while(code* loop, expr* e, function_table* f,
                    string* functionname, varlist* v) {
  string countstr;
  string_init(&countstr);
  strinng_inttostr(&countstr, count++);
  string labelwhile;
  string_set(&labelwhile, functionname->txt);
  string_append(&labelwhile, "_while");
  string_append(&labelwhile, countstr.txt);
  string exprtype;
  string_init(&exprtype);
  string_set(&exprtype, "whileexpr");
  string_append(&exprtype, countstr.txt);
  printf("DEFVAR LF@%s", exprtype);
  printf("LABEL %s", labelwhile);
  evaluate_expression(e, f, functionname, v, &exprtype);
  printf("JUMPIFEQ %s LF@%s nil@nil", countstr, exprtype);
  generate_code(loop, f, functionname, false, v);
  printf("JUMP %s", labelwhile);
  printf("LABEL %s", countstr);
}

void generate_assignment(string* var, expr* e, function_table* f,
                         string* functionname, varlist** v) {
  int type = evaluate_expression(e, f, functionname, *v, var);
  varlist_set(v, var, type);
}

void generate_return(expr* e, function_table* f, string* functionname,
                     varlist* v) {
  string countstr;
  string_init(&countstr);
  strinng_inttostr(&countstr, count++);
  printf("DEFVAR LF@%s", countstr.txt);
  if (functionname == NULL) {
    evaluate_expression(e, f, functionname, v, &countstr);
    printf("EXIT int@0");
    return;
  }

  evaluate_expression(e, f, functionname, v, &countstr);
  printf("MOVE GF@-_-returnvalue LF@%s", countstr.txt);
  printf("RETURN");
}
// TODO Improve this
int evaluate_expression(expr* e, function_table* f, string* functionname,
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
          printf("PUSHS int@%s\n", e->var->txt);
          Stack_Push(&s, _int);
          break;
        case _float:
          printf("PUSHS float@%s\n", e->var->txt);
          Stack_Push(&s, _float);
          break;
        case _string:
          printf("PUSHS string@%s\n", e->var->txt);
          Stack_Push(&s, _string);
          break;
        case _void:
          printf("PUSHS nil@nil\n", e->var->txt);
          Stack_Push(&s, _null);
          break;
        case _bool:
          printf("PUSHS bool@bool\n", e->var->txt);
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
        case _dot:
          Stack_Pop(s);
          printf("POPS LF@%s\n", tovar->txt);
          Stack_Pop(s);
          printf("DEFVAR LF@%d", count);
          printf("POPS LF@%d\n", count);
          printf("CONCAT LF@%s LF@%d LF@%s\n", tovar->txt, count, tovar->txt);
          if (e->next != NULL) {
            printf("PUSHS LF@%s\n", tovar->txt);
            Stack_Push(&s, _string);
          }
          count++;
          break;

        default:
          break;
      }
    } else if (e->func) {
      // generate_call(e->func, f, functionname, v); TODO
    }

    e = e->next;
  }
  if (!Stack_IsEmpty(s)) {
    int type;
    Stack_Top(s, &type);
    Stack_Pop(s);
    printf("POPS LF@%s\n", tovar->txt);
    return type;
  } else {
    exit(6);
  }
}