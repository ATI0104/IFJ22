#include "symtable.h"

void function_table_init(function_table **tree) {
  *tree = NULL;
 /*Built-in functions*/

  // reads()
  function_table *reads_ft;
  maloc(reads_ft, sizeof(function_table));
  reads_ft->input_type = NULL;
  string_set(&(*reads_ft).name, "reads");
  reads_ft->output_type = _string;
  function_table_add(tree, reads_ft);

  // readi()
  function_table *readi_ft;
  maloc(readi_ft, sizeof(function_table));
  readi_ft->input_type = NULL;
  string_set(&(*readi_ft).name, "readi");
  readi_ft->output_type = _int;
  function_table_add(tree, readi_ft);

  // readf()
  function_table *readf_ft;
  maloc(readf_ft, sizeof(function_table));
  readf_ft->input_type = NULL;
  string_set(&(*readf_ft).name, "readf");
  readf_ft->output_type = _float;
  function_table_add(&(*tree), readf_ft);
  /*SPEC FUNCTION*/

  // write(term)
  function_table *write_ft;
  maloc(write_ft, sizeof(function_table));
  write_ft->input_type = NULL;
  string_set(&(write_ft->name), "write");
  write_ft->output_type = _null;
  function_table_add(&(*tree), write_ft);

  // floatval(term) SPEC FUNCTION
  function_table *floatval_ft;
  maloc(floatval_ft, sizeof(function_table));
  // input_param_list params;
  // input can be null/int/float/bool/string
  floatval_ft->input_type = NULL;
  string_set(&(*floatval_ft).name, "floatval");
  floatval_ft->output_type = _float;
  function_table_add(&(*tree), floatval_ft);

  // intval(term)
  function_table *intval_ft;
  maloc(intval_ft, sizeof(function_table));
  // input_param_list params;
  // input can be null/int/float/bool/string
  intval_ft->input_type = NULL;
  string_set(&(*intval_ft).name, "intval");
  intval_ft->output_type = _int;
  function_table_add(&(*tree), intval_ft);

  // strval(term)
  function_table *strval_ft;
  maloc(strval_ft, sizeof(function_table));
  // input can be string/null
  strval_ft->input_type = NULL;
  string_set(&(*intval_ft).name, "strval");
  strval_ft->output_type = _string; 
  function_table_add(&(*tree), strval_ft);

  // strlen()
  function_table *strlen_ft;
  input_param_list *params = NULL;
  maloc(strlen_ft, sizeof(function_table));
  string s;
  string_set(&s, "s");
  params = insert_top(s, _string, params);
  string_set(&(*strlen_ft).name, "strlen");
  strlen_ft->input_type = params;
  strlen_ft->output_type = _int;  //_int
  function_table_add(&(*tree), strlen_ft);

  // substring(string s, int id, int id2)
  function_table *substring_ft;
  maloc(substring_ft, sizeof(function_table));
  input_param_list *param_substr = NULL;
  string_set(&s, "s");
  param_substr = insert_top(s, _string, param_substr);
  string_set(&s, "i");
  param_substr = insert_top(s, _int, param_substr);
  string_set(&s, "j");
  param_substr = insert_top(s, _int, param_substr);
  substring_ft->input_type = param_substr;
  string_set(&(*substring_ft).name, "substring");
  substring_ft->output_type = _string;
  function_table_add(&(*tree), substring_ft);

  // ord(string c)
  function_table *ord_ft;
  maloc(ord_ft, sizeof(function_table));
  input_param_list *param_ord = NULL;
  string_set(&s, "c");
  param_ord = insert_top(s, _string, param_ord);
  ord_ft->input_type = param_ord;
  string_set(&(*ord_ft).name, "ord");
  ord_ft->output_type = _int;
  function_table_add(&(*tree), ord_ft);

  // chr(int i)
  function_table *chr_ft;
  maloc(chr_ft, sizeof(function_table));
  input_param_list *param_chr = NULL;
  string_set(&s, "i");
  param_chr = insert_top(s, _int, param_chr);
  chr_ft->input_type = param_chr;
  string_set(&(*chr_ft).name, "chr");
  chr_ft->output_type = _string;
  function_table_add(tree, chr_ft);
  return;
}
void function_table_add(function_table **tree, function_table *leaf) {
  if (*tree == NULL) {
    *tree = leaf;
    return;
  }
  if (strcmp((*tree)->name.txt, leaf->name.txt) > 0) {
    function_table_add(&(*tree)->right_func, leaf);
  } else
    function_table_add(&(*tree)->left_func, leaf);
}
function_table *function_table_get(function_table **tree, string name) {
  if (*tree == NULL) return NULL;
  if (strcmp((*tree)->name.txt, name.txt) == 0) return *tree;
  if (strcmp((*tree)->name.txt, name.txt) > 0) {
    return function_table_get(&(*tree)->right_func, name);
  } else
    return function_table_get(&(*tree)->left_func, name);
}
void function_table_destroy(function_table **tree) {
  if (*tree == NULL) return;
  function_table_destroy(&((*tree)->left_func));
  function_table_destroy(&((*tree)->right_func));
  free((*tree)->name.txt);
  free(*tree);
  *tree = NULL;
}
void function_table_delete(function_table **tree, string name) {
  if (*tree == NULL) return;
  if (strcmp((*tree)->name.txt, name.txt) < 0) {
    function_table_delete(&(*tree)->left_func, name);
    return;
  } else if (strcmp((*tree)->name.txt, name.txt) > 0) {
    function_table_delete(&(*tree)->right_func, name);
    return;
  } else {
    if ((*tree)->left_func == NULL && (*tree)->right_func == NULL) {
      free(*tree);
      *tree = NULL;
    } else {
      if ((*tree)->left_func != NULL && (*tree)->right_func != NULL) {
        if ((*tree)->left_func->right_func != NULL) {
          func_table_replace_by_rightmost(*tree, &((*tree)->left_func));
        }
      } else {
        if ((*tree)->left_func == NULL) {
          function_table *tmp = *tree;
          *tree = (*tree)->right_func;
          free(tmp->name.txt);
          free(tmp);
        }
        if ((*tree)->right_func == NULL) {
          function_table *tmp = *tree;
          *tree = (*tree)->left_func;
          free(tmp->name.txt);
          free(tmp);
        }
      }
    }
  }
}

void func_table_replace_by_rightmost(function_table *target, function_table **tree) {
  if ((*tree)->right_func == NULL) {
    target->name = (*tree)->name;
    target->input_type = (*tree)->input_type;
    target->output_type = (*tree)->output_type;
    target->variable = (*tree)->variable;
    function_table_delete(tree, target->name);
  } else
    func_table_replace_by_rightmost(target, &(*tree)->right_func);
}
function_table *allocate_leaf() { return malloc(sizeof(function_table)); }

void var_table_init(var_table **tree) {
  *tree = NULL;
  return;
}

void var_table_add(var_table **tree, var_table *var) {
  if (*tree == NULL) {
    *tree = var;
    return;
  }
  if (strcmp((*tree)->name.txt, var->name.txt) > 0) {
    var_table_add(&(*tree)->right_var, var);
  } else
    var_table_add(&(*tree)->left_var, var);
}

var_table *var_table_get(var_table **tree, string name) {
  if (*tree == NULL) return NULL;
  if (strcmp((*tree)->name.txt, name.txt) == 0) return *tree;
  if (strcmp((*tree)->name.txt, name.txt) > 0) {
    return var_table_get(&(*tree)->right_var, name);
  } else
    return var_table_get(&(*tree)->right_var, name);
}

void var_table_destroy(var_table **tree) {
  if (*tree == NULL) return;
  var_table_destroy(&((*tree)->left_var));
  var_table_destroy(&((*tree)->right_var));
  free((*tree)->name.txt);
  free(*tree);
  *tree = NULL;
}

void var_table_delete(var_table **tree, string name) {
  if (*tree == NULL) return;
  if (strcmp((*tree)->name.txt, name.txt) < 0) {
    var_table_delete(&(*tree)->left_var, name);
    return;
  } else if (strcmp((*tree)->name.txt, name.txt) > 0) {
    var_table_delete(&(*tree)->right_var, name);
    return;
  } else {
    if ((*tree)->left_var == NULL && (*tree)->right_var == NULL) {
      free(*tree);
      *tree = NULL;
    } else {
      if ((*tree)->left_var != NULL && (*tree)->right_var != NULL) {
        if ((*tree)->left_var->right_var != NULL) {
          var_table_replace_by_rightmost(*tree, &((*tree)->left_var));
        }
      } else {
        if ((*tree)->left_var == NULL) {
          var_table *tmp = *tree;
          *tree = (*tree)->right_var;
          
          free(tmp);
        }
        if ((*tree)->right_var == NULL) {
          var_table *tmp = *tree;
          *tree = (*tree)->left_var;
          free(tmp);
        }
      }
    }
  }
}

void var_table_replace_by_rightmost(var_table *target, var_table **tree) {
  if ((*tree)->right_var == NULL) {
    target->name = (*tree)->name;
    target->type = (*tree)->type;
    target->num = (*tree)->num;
    target->fl = (*tree)->fl;
    target->s = (*tree)->s;
    
    var_table_delete(tree, target->name);
  } else
    var_table_replace_by_rightmost(target, &(*tree)->right_var);
}
void add_predef_funs(function_table **tree){
 if(tree == NULL) return; 
}
