#include "scanner.h"
#include "string.h"
#include "symtable.h"
typedef struct input {
  int* i;
  double* f;
  string* s;
  string* var;
  struct input* next;
} input;
typedef struct call {
  string* function_name;
  input in;
  input out;
} call;
typedef struct expr {
  int type;
  string* str;
  int* num;
  double* fl;
  int* op;
  string* var;
  call* func;
} expr;
typedef struct code {
  int line_num;
  struct code* i;     // if
  struct code* e;     // else
  struct code* loop;  // while
  expr* expression;
  call* jmp;
  struct code* next;
} code;
typedef struct AST {
  string* funname;
  code* code;
} AST;
