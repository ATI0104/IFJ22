#include <stdbool.h>

#include "string.h"
#ifndef _symtable
#define _symtable
typedef struct symtable {
  int type;
  string name;
  int i_value;
  double f_value;
  string s_value;
  bool null;
  struct symtable *left;
  struct symtable *right;
} symtable;

void symtable_init(symtable **tree);
void symtable_add(symtable **tree, symtable *leaf);
symtable *symtable_get(symtable **tree, string name);
void symtable_destroy(symtable **tree);
void symtable_delete(symtable **tree, string name);
void symtable_replace_by_rightmost(symtable *target, symtable **tree);
symtable *allocate_leaf();
#endif