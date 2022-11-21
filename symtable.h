
#ifndef _symtable
#define _symtable
#include <stdbool.h>
#include "scanner.h"
#include "string.h"

typedef struct input_param_list {
  string name;
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

  var_table variable;

  struct function_table *left_func;
  struct function_table *right_func;

} function_table;

void function_table_init(function_table **tree);
void function_table_add(function_table **tree, function_table *leaf);
function_table *function_table_get(function_table **tree, string name);
void function_table_destroy(function_table **tree);
void function_table_delete(function_table **tree, string name);
void func_table_replace_by_rightmost(function_table *target,
                                     function_table **tree);
function_table *allocate_leaf();

//functions for var_table
void var_table_init(var_table **tree);
void var_table_add(var_table **tree, var_table *leaf);
var_table *var_table_get(var_table **tree, string name);
void var_table_destroy(var_table **tree);
void var_table_delete(var_table **tree, string name);
void var_table_replace_by_rightmost(var_table *target,
                                     var_table **tree);
void add_predef_funs(function_table **tree);

input_param_list * insert_top(string name, int type, input_param_list *current);
#endif
