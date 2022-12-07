/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Attila Kovács (xkovac60)
  * @file main.c
*/
#include "ast.h"
#include "generator.h"
#include "macros.h"
#include "parser.h"
#include "scanner.h"
#include "semantics.h"
#include "string.h"
#include "symtable.h"
int main(int argc, char* argv[]) {
  if (argc > 1) {
    eprint(
        "Help:\n This a program which takes IFJ22 code(PHP derivative) and "
        "converts it to IFJcode22.\n This program reads from the standard "
        "input and writes to the standard output, so no input arguments are "
        "needed.\n Usage: \"%s < inputfile > outputfile\"\n",
        argv[0]);
    exit(0);
  }
  // Reading in the input and converting it to tokens then these tokens are
  // returned as a list.
  tlist* t = create_tlist();
  // This is the symbol table
  function_table* f = NULL;
  function_table_init(&f);
  // Reading in user defined functions and variables
  add_func(&f, t);
  // Checking the syntax of the input
  if (check_syntax(t, f)) {
    tlist* mainfunction;
    var_table* globalvariables;
    // Moving the tokens which are not part of any function to the main function
    // variable
    t = move_tokens(t, &mainfunction, &globalvariables);
    // Converting the tokens into an abstract syntax tree
    AST* abstractsyntaxtree = ConvertToAst(t, mainfunction, globalvariables);
    // Checking the semantics of the abstract syntax tree
    if (Check_AST(abstractsyntaxtree, f)) {
      // Generating IFJcode22
     // generate(abstractsyntaxtree, f);
    } else {
      semantic_error(-1, "");
    }
  } else
    syntaxerror(-1);
}
