#include "macros.h"
#include "scanner.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"
#ifndef _parser
#define _parser
#define output input
typedef struct input {
  int* i;
  double* f;
  string* s;
  string* var;
  struct input* first;
  struct input* next;
} input;
typedef struct call {
  string* function_name;
  input* in;
  output* out;
} call;
typedef struct expr {
  int type;  // not used
  string* str;
  int* num;
  double* fl;
  int* op;
  string* var;
  call* func;
  struct expr* next;
  struct expr* first;
} expr;
typedef struct c {
  int line_num;
  struct c* i;     // if (cond) {}
  struct c* e;     // else {}
  struct c* loop;  // while (cond)
  expr* expression;
  call* jmp;
  struct c* first;
  struct c* next;
} code;
typedef struct AST {
  string* funname;
  code* code;
  struct AST* next;
  struct AST* first;
} AST;

typedef struct tlist {
  token t;
  struct tlist* next;
} tlist;

void AST_init(AST** a);
void AST_add(AST** a, string* f_name, code* code);
void AST_destroy(AST** a);
void code_init(code** c);
void code_add(code** c, int lnum, code* i, code* e, code* loop, expr* exp,
              call* jmp);
void code_destroy(code** c);
void expr_init(expr** e);
void expr_add(expr** e, int type, string* str, int* num, double* fl, int* op,
              string* var, call* func);
void expr_destroy(expr** e, bool rec);
expr* expr_pop(expr** e);
void expr_topostfix(expr** e);
void expr_reverse(expr** e);
void call_init(call** c);
void call_create(call** c, string* f_name, input* in, output* out);
void call_destroy(call** c);
void input_init(input** in);
void input_add(input** in, int* i, double* f, string* s, string* var);
void input_destroy(input** in);
tlist* create_tlist();
expr* read_expression(tlist* t, int* c);
tlist* create_floats(tlist *t);
#endif