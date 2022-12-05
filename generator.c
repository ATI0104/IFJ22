#include "generator.h"

#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "semantics.h"

void generate(AST* a, function_table* f) {
  printf(".IFJcode22\n");
  while (strcmp(a->funname->txt, "m@in") != 0) {
    a = a->next;
  }
}