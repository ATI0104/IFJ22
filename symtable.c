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
