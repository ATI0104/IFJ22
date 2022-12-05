
#ifndef _parser
#define _parser
#include <math.h>

#include "ast.h"
#include "macros.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"
#include "expression.h"
expr* read_expression(int brackets, bool expriif, string* fname);
void add_func(function_table** tree, tlist* t);
var_table* load_variables(tlist* t, input_param_list* input);
call* load_function_call();
tlist* move_tokens(tlist* t, tlist** mainfunction, var_table** globalvariables);
int function_skip(tlist* tokens);
tlist* tlist_add(tlist* t, token tok);
AST* ConvertToAst(tlist* functions, tlist* mainfunction, var_table* globalvars);
code* ConvertToCode();
void combinevartables(function_table** root);
bool isin(int i, int l[]);

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
bool expression_check(int brackets, bool exprinif);
tlist* create_floats(tlist* t);
tlist* create_negative(tlist* t);
tlist* process_exponent(tlist* t);
expr* add_parenthesis(expr* e);
tlist* create_tlist();
void varlist_set(varlist** v, string* name, int type);
int varlist_get(varlist* v, string* name);
expr* expression_flip(expr* e);
void varlist_set(varlist** v, string* name, int type);
int varlist_get(varlist* v, string* name);
#endif
