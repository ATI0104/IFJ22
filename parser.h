
#ifndef _parser
#define _parser
#include <math.h>

#include "ast.h"
#include "macros.h"
#include "scanner.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"

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
