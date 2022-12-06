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
#endif