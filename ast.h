#ifndef _ast
#define _ast

#include "macros.h"
#include "scanner.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"
#define output input

typedef struct input {
  int* i;               // constant positive integer
  double* f;            // constant decimal number or negative integer
  string* s;            // constant string
  string* var;          // variable
  bool* null;           // null
  bool* question_mark;  // ?
  struct input* first;
  struct input* next;
} input;
typedef struct call {
  string* function_name;  // name of the function
  input* in;              // input
  output* out;            // Not used
} call;
typedef struct expr {
  int type;           // Estimated data type of the result
  string* str;        // constant string
  int* num;           // constant positive integer
  double* fl;         // constant decimal number or negative integer
  int* op;            // operator (+, -, *, /, <, >, <=, >=, ===, !==,...)
  string* var;        // variable
  call* func;         // function call
  int* typekeywords;  // type keywords (int,float,string,void)
  struct expr* next;
  struct expr* first;
} expr;
typedef struct c {
  int line_num;
  struct c* i;       // if (cond) {} (cond == expression)
  struct c* e;       // else {}
  struct c* loop;    // while (cond)
  bool ret;          // return expression;
  string* var;       // variable = expression;
  call* jmp;         // function call with no return value
  expr* expression;  // expression for example 5; or ((5+5)*5);
  struct c* first;
  struct c* next;
} code;
typedef struct AST {
  string* funname;    // function name
  code* Code;         // function body
  struct AST* next;   // next function
  struct AST* first;  // first function
} AST;

typedef struct tlist {
  token t;
  struct tlist* next;
} tlist;

typedef struct varlist {
  int current_type;
  string* name;
  struct varlist* next;
} varlist;

typedef struct my_favorites {
  function_table* f;  // function table
  //var_table* v;       // current local variable state
  tlist* t;           // token list
  AST* a;             // abstract syntax tree
} my_favorites;
void AST_init(AST** a);
void AST_add(AST** a, string* f_name, code* code);
void AST_destroy(AST** a);
void code_init(code** c);
void code_add(code** c, int lnum, code* i, code* e, code* loop, expr* exp,
              call* jmp, string* var, bool ret);
void code_destroy(code** c);
void expr_init(expr** e);
void expr_add(expr** e, int type, string* str, int* num, double* fl, int* op,
              string* var, call* func, int* keywords);
void expr_destroy(expr** e, bool rec);
expr* expr_pop(expr** e);
void expr_toprefix(expr** e);
void expr_reverse(expr** e);
void call_init(call** c);
void call_create(call** c, string* f_name, input* in, output* out);
void call_destroy(call** c);
void input_init(input** in);
void input_add(input** in, int* i, double* f, string* s, string* var);
void input_destroy(input** in);

#endif
