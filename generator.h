#ifndef _generator_h
#define _generator_h
#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "semantics.h"
#include "string.h"
#include "symtable.h"
void evaluate_expression(expr* e, function_table* f, string* functionname,
                         varlist* v, string* tovar);
void generate_return(expr* e, function_table* f, string* functionname,
                     varlist* v);
void generate_assignment(string* var, expr* e, function_table* f,
                         string* functionname, varlist** v);
void generate_while(code* loop, expr* e, function_table* f,
                    string* functionname);
void generate_if(code* ifpart, code* elsepart, expr* e, function_table* f,
                 string* functionname);
#endif