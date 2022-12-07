/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Attila Kovács (xkovac60)
  * @file parser.h
*/
#ifndef _parser
#define _parser
#include <math.h>

#include "ast.h"
#include "macros.h"
#include "stack.h"
#include "string.h"
#include "symtable.h"
#include "expression.h"
/**
 * @brief Reads the expression 
 */
expr* read_expression(int brackets, bool expriif, string* fname);
/**
 * @brief Adds user defined function to function_table
 */
void add_func(function_table** tree, tlist* t);
/**
 * @brief Adds declared variable to the var_table
 */
var_table* load_variables(tlist* t, input_param_list* input);
/**
* @brief Returns the called function
*/
call* load_function_call();
/**
* @brief Moves tokens which are not part of a function to the end of the token list
*/
tlist* move_tokens(tlist* t, tlist** mainfunction, var_table** globalvariables);
/**
* @brief Skips the function definition
*/
int function_skip(tlist* tokens);
/**
* @brief Adds token to the token list
*/
tlist* tlist_add(tlist* t, token tok);

/**
* @brief AST creation including semantics analysis
*/
AST* ConvertToAst(tlist* functions, tlist* mainfunction, var_table* globalvars);
/**
* @brief Convert AST to code and checks expressions
*/
code* ConvertToCode();
/**
* @brief Combines var_tables into one
*/
void combinevartables(function_table** root);
bool isin(int i, int l[]);
/**   Checks correct syntax  **/
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
/**
* @brief Checks expression if its valid
*/
bool expression_check(int brackets, bool exprinif);
/**
* @brief Creates float numbers from tokens
*/
tlist* create_floats(tlist* t);
/**
* @brief Creates negative numbers
*/
tlist* create_negative(tlist* t);
tlist* process_exponent(tlist* t);
/**
* @brief Adds parenthesis before and after expression
*/
expr* add_parenthesis(expr* e);
/**
* @brief Creates list of tokens
*/
tlist* create_tlist();
void varlist_set(varlist** v, string* name, int type);
int varlist_get(varlist* v, string* name);
expr* expression_flip(expr* e);
void varlist_set(varlist** v, string* name, int type);
int varlist_get(varlist* v, string* name);
#endif
