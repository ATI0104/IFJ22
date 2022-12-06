#ifndef _generator_h
#define _generator_h
#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "semantics.h"
#include "string.h"
#include "symtable.h"
void generate(AST* a, function_table* f);
void generate_code(code* c, function_table* f, string* name, bool newfunction,
                   varlist* localvars);
void generate_exit_codes();
void define_variables(function_table* f);
void generate_code(code* c, function_table* f, string* name, bool newfunction,
                   varlist* localvars);
void generate_if(code* ifpart, code* elsepart, expr* e, function_table* f,
                 string* functionname, varlist* v);
void generate_while(code* loop, expr* e, function_table* f,
                    string* functionname, varlist* v);
void generate_assignment(string* var, expr* e, function_table* f,
                         string* functionname, varlist** v);
void generate_return(expr* e, function_table* f, string* functionname,
                     varlist* v);
int evaluate_expression(expr* e, function_table* f, string* functionname,
                        varlist* v, string* tovar);
#endif