#include "parser.h"
#include "scanner.h"
#include "string.h"
#include "symtable.h"
int main() {
  tlist* m = create_tlist();
  string a;
  string_init(&a);
  // Prints out basic informations about the generated tokens
  token tok;
  while (m != NULL) {
    tok = m->t;
    printf("Token from line %d\n has a type of %d.\n", *(tok.linenum),
           tok.type);
    if (tok.i_val != NULL) {
      printf("It's int value = %d\n", *tok.i_val);
    }
    if (tok.str != NULL) {
      printf("It's string value = \"%s\"\n", tok.str->txt);
    }
    m = m->next;
  }

  return 0;
}