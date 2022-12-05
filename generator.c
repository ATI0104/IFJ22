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
  if (c->i) {
    generate_if(c->i, c->e, c->expression, f, name);
  } else if (c->loop) {
    generate_while(c->loop, c->expression, f, name);
  } else if (c->var) {
    generate_assignment(c->var, c->expression, f, name, v);
  } else if (c->ret) {
    generate_return(expression, f, name);
  } else if (c->expression) {
    generate_expression(c->expression, f, name, v);
  } else if (c->func) {
    generate_call(c->func, f, name, v);
  }
}

void generate_if(code* ifpart, code* elsepart, expression* e, function_table* f,
                 string* functionname) {}

void generate_while(code* loop, expression* e, function_table* f,
                    string* functionname) {}

void generate_assignment(variable* var, expression* e, function_table* f,
                         string* functionname, varlist* v) {}

void generate_return(expression* e, function_table* f, string* functionname,
                     varlist* v) {}

void evaluate_expression(expression* e, function_table* f, string* functionname,
                         varlist* v, string* tovar) {}