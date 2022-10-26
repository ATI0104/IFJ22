#include "symtable.h"

void function_table_init(function_table **tree) {
  *tree = NULL;
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