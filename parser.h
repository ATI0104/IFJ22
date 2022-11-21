
#ifndef _parser
#define _parser
#include <math.h>

#include "macros.h"
#include "scanner.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"

#define output input

typedef struct input {
  int* i;       // constant positive integer
  double* f;    // constant floating point number or negative integer
  string* s;    // constant string
  string* var;  // variable
  bool null;    // null
  struct input* first;
  struct input* next;
} input;
typedef struct call {
  string* function_name;
  input* in;
  output* out;  // Not used
} call;
typedef struct expr {
  int type;     // Estimated data type of the result
  string* str;  // constant string
  int* num;     // constant positive integer
  double* fl;   // constant floating point number or negative integer
  int* op;      // operator (+, -, *, /, <, >, <=, >=, ===, !==,...)
  string* var;  // variable
  call* func;   // function call
  struct expr* next;
  struct expr* first;
} expr;
typedef struct c {
  int line_num;
  struct c* i;     // if (cond) {} (cond == expression)
  struct c* e;     // else {}
  struct c* loop;  // while (cond)
  expr* ret;       // return expression;
  string* var;     // variable = expression;
  call* jmp;       // function call with no return value
  expr* expression;
  struct c* first;
  struct c* next;
} code;
typedef struct AST {
  string* funname;  // function name
  code* Code;       // function body
  struct AST* next;
  struct AST* first;
} AST;

typedef struct tlist {
  token t;
  struct tlist* next;
} tlist;
typedef struct my_favorites {
  function_table* f;
  var_table* v;
  tlist* t;
  AST* a;
} my_favorites;
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
expr* add_parenthesis(expr* e);
tlist* create_floats(tlist* t);
tlist* create_negative(tlist* t);
tlist* process_exponent(tlist* t);

void add_func(function_table** tree, tlist* t);
var_table* load_variables(tlist* t);
call* load_function_call(tlist* t, function_table* f, var_table* v, int* skip);
#endif
