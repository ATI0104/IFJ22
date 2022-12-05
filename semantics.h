#ifndef _semantics
#define _semantics
#include "ast.h"
#include "macros.h"
#include "parser.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"
bool Check_AST(AST* a, function_table* f);
bool Check_code(code* c, function_table* f, var_table* localVars,
                string funname);
bool check_return_type(expr* ret, function_table* f, string funname,
                       varlist* v);
int get_expression_type(expr* e, varlist* v, function_table* f);
void Check_expression(expr* e, function_table* f, varlist* v);
bool call_check(call* c, function_table* f, varlist* v);
#endif