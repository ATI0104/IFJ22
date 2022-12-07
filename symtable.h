/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Anna Udvaros (xudvar02)
  * @file symtable.h
*/
#ifndef _symtable
#define _symtable
#include <stdbool.h>

#include "scanner.h"
#include "string.h"

typedef struct input_param_list {
  string name;
  bool question_mark;
  int type;
  struct input_param_list *next;
} input_param_list;

typedef struct var_table {
  int type;  // Declared type
  string name;
  int num;
  double fl;
  string s;
  struct var_table *left_var;
  struct var_table *right_var;
} var_table;

typedef struct function_table {
  string name;
  bool built_in;

  // input param
  input_param_list *input_type;

  // output type
  int output_type;
  bool questionmark;

  var_table *variable;

  struct function_table *left_func;
  struct function_table *right_func;

} function_table;
/**
* @brief Initialize function table
*/
void function_table_init(function_table **tree);
/**
* @brief Adds element to the function table
*/
void function_table_add(function_table **tree, function_table *leaf);
/**
* @brief Get element from the function table based on its name
*/
function_table *function_table_get(function_table **tree, string name);
/**
* @brief Removes the entire function table
*/
void function_table_destroy(function_table **tree);
/**
* @brief Removes one function from the function table based on its name
*/
void function_table_delete(function_table **tree, string name);
/**
* @brief Moves and releases the rightmost node of the left subtree
*/
void func_table_replace_by_rightmost(function_table *target, function_table **tree);
/**
* @brief Allocates leaf
*/
function_table *allocate_leaf();

/** FUNCTIONS FOR VAR_TABLE ***/
/**
* @brief Initialize var_table
*/
void var_table_init(var_table **tree);
/**
* @brief Adds element to the var_table
*/
void var_table_add(var_table **tree, var_table *leaf);
/**
* @brief Gets element from the var_table based on its name
*/
var_table *var_table_get(var_table **tree, string name);
/**
* @brief Removes the entire var_table
*/
void var_table_destroy(var_table **tree);
/**
* @brief Removes one variable from the var_table based on its name
*/
void var_table_delete(var_table **tree, string name);
/**
* @brief Moves and releases the rightmost node of the left subtree
*/
void var_table_replace_by_rightmost(var_table *target, var_table **tree);
/**
* @brief Adds predefined functions to the function_table
*/
void add_predef_funs(function_table **tree);
/**
* @brief Adds parameter to the list of parameters
*/
input_param_list *insert_top(string name, int type, input_param_list *current);
#endif
