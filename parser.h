
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
expr* read_expression(tlist* t, int* c, int brackets);
expr* add_parenthesis(expr* e);

tlist* create_floats(tlist* t);
tlist* create_negative(tlist* t);
tlist* process_exponent(tlist* t);

void add_func(function_table** tree, tlist* t);
var_table* load_variables(tlist* t, input_param_list* input);
call* load_function_call(tlist* t, function_table* f, int* skip);

bool check_syntax(tlist* t, function_table* f);
bool prog();
bool prog2();
bool function_definition();
bool params();
bool def_params();
bool type();
bool body();
bool if_statement();
bool else_statement();
bool while_statement();
bool return_statement();
bool var_set();
bool function_call();
bool expression_check(int brackets);
tlist* move_tokens(tlist* t);
int function_skip(tlist* tokens);
tlist* tlist_add(tlist* t, token tok);

AST* ConvertToAst(tlist* functions, tlist* mainfunction);
code* ConvertToCode(tlist* t);
#endif
