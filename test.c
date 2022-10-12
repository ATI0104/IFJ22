#include "scanner.h"
#include "string.h"
#include "symtable.h"
int main() {
  token tok;
  string a;
  string_init(&a);
  // Prints out basic informations about the generated tokens
  while (tok.type != 100) {
    tok = get_token();
    printf("Token from line %d\n has a type of %d.\n", *(tok.linenum),
           tok.type);
    if (tok.i_val != NULL) {
      printf("It's int value = %d\n", *tok.i_val);
    }
    if (tok.str != NULL) {
      printf("It's string value = \"%s\"\n", tok.str->txt);
    }
  }

  return 0;
}