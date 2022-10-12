#include "symtable.h"
//TODO check for memory leaks
void symtable_init(symtable **tree) {
  *tree = NULL;
  return;
}
void symtable_add(symtable **tree, symtable *leaf) {
  if (*tree == NULL) {
    *tree = leaf;
    return;
  }
  if (strcmp((*tree)->name.txt, leaf->name.txt) > 0) {
    symtable_add(&(*tree)->right, leaf);
  } else
    symtable_add(&(*tree)->left, leaf);
}
symtable *symtable_get(symtable **tree, string name) {
  if (*tree == NULL) return NULL;
  if (strcmp((*tree)->name.txt, name.txt) == 0) return *tree;
  if (strcmp((*tree)->name.txt, name.txt) > 0) {
    return symtable_get(&(*tree)->right, name);
  } else
    return symtable_get(&(*tree)->left, name);
}
void symtable_destroy(symtable **tree) {
  if (*tree == NULL) return;
  symtable_destroy(&((*tree)->left));
  symtable_destroy(&((*tree)->right));
  free((*tree)->name.txt);
  free(*tree);
  *tree = NULL;
}
void symtable_delete(symtable **tree, string name) {
  if (*tree == NULL) return;
  if (strcmp((*tree)->name.txt, name.txt) < 0) {
    symtable_delete(&(*tree)->left, name);
    return;
  } else if (strcmp((*tree)->name.txt, name.txt) > 0) {
    symtable_delete(&(*tree)->right, name);
    return;
  } else {
    if ((*tree)->left == NULL && (*tree)->right == NULL) {
      free(*tree);
      *tree = NULL;
    } else {
      if ((*tree)->left != NULL && (*tree)->right != NULL) {
        if ((*tree)->left->right != NULL) {
          symtable_replace_by_rightmost(*tree, &((*tree)->left));
        }
      } else {
        if ((*tree)->left == NULL) {
          symtable *tmp = *tree;
          *tree = (*tree)->right;
          free(tmp->name.txt);
          free(tmp);
        }
        if ((*tree)->right == NULL) {
          symtable *tmp = *tree;
          *tree = (*tree)->left;
          free(tmp->name.txt);
          free(tmp);
        }
      }
    }
  }
}

void symtable_replace_by_rightmost(symtable *target, symtable **tree) {
  if ((*tree)->right == NULL) {
    target->type = (*tree)->type;
    target->name = (*tree)->name;
    target->i_value = (*tree)->i_value;
    target->f_value = (*tree)->f_value;
    target->null = (*tree)->null;
    symtable_delete(tree, target->name);
  } else
    symtable_replace_by_rightmost(target, &(*tree)->right);
}
symtable *allocate_leaf() { return malloc(sizeof(symtable)); }