#ifndef _mymacros
#define _mymacros
#define SyntaxError 2
#define Function_Redefinition 3
#define Function_Undefined 3
#define Incorrect_Type_of_Argument 4
#define Incorrect_Number_of_Arguments 4
#define Undefined_Variable 5
#define No_return 6
#define Type_Mismatch 7
#define Semantic_Error 8
#define Internal_Error 99
#define maloc(pointer, size)    \
  do {                          \
    pointer = calloc(size, 1);  \
    if (pointer == NULL) {      \
      eprint("Malloc Error\n"); \
      exit(Internal_Error);     \
    }                           \
  } while (0)

#define eprint(...)               \
  do {                            \
    fprintf(stderr, __VA_ARGS__); \
  } while (0)

#define lexerror                                   \
  do {                                             \
    eprint("Lexical Error on line %d\n", linenum); \
    string_destroy(word);                          \
    exit(LEXICAL_ERROR);                           \
  } while (0)
#define syntaxerror(t)                        \
  do {                                        \
    if (t > 0)                                \
      eprint("Syntax Error on line %d\n", t); \
    else                                      \
      eprint("Syntax Error\n");               \
    exit(SyntaxError);                        \
  } while (0)
#define function_redefinition(t)                       \
  do {                                                 \
    if (t > 0)                                         \
      eprint("Function redefinition on line %d\n", t); \
    else                                               \
      eprint("Function redefinition\n");               \
    exit(Function_Redefinition);                       \
  } while (0)
#define function_undefined(t)                       \
  do {                                              \
    if (t > 0)                                      \
      eprint("Undefined function on line %d\n", t); \
    else                                            \
      eprint("Undefined function\n");               \
    exit(Function_Undefined);                       \
  } while (0)
#define incorrect_type_of_argument(t)                       \
  do {                                                      \
    if (t > 0)                                              \
      eprint("Incorrect type of argument on line %d\n", t); \
    else                                                    \
      eprint("Incorrect type of argument\n");               \
    exit(Incorrect_Type_of_Argument);                       \
  } while (0)
#define incorrect_number_of_arguments(t)                       \
  do {                                                         \
    if (t > 0)                                                 \
      eprint("Incorrect number of arguments on line %d\n", t); \
    else                                                       \
      eprint("Incorrect number of arguments\n");               \
    exit(Incorrect_Number_of_Arguments);                       \
  } while (0)
#define undefined_variable(t)                       \
  do {                                              \
    if (t > 0)                                      \
      eprint("Undefined variable on line %d\n", t); \
    else                                            \
      eprint("Undefined variable\n");               \
    exit(Undefined_Variable);                       \
  } while (0)
#define no_return(t)                           \
  do {                                         \
    if (t > 0)                                 \
      eprint("Incorrect return type %d\n", t); \
    else                                       \
      eprint("Incorrect return type\n");       \
    exit(No_return);                           \
  } while (0)
#define type_mismatch(t)                       \
  do {                                         \
    if (t > 0)                                 \
      eprint("Type mismatch on line %d\n", t); \
    else                                       \
      eprint("Type mismatch\n");               \
    exit(Type_Mismatch);                       \
  } while (0)
#define semantic_error(t, s)                           \
  do {                                                 \
    if (t > 0)                                         \
      eprint("Semantic error on line %d\n%s\n", t, s); \
    else                                               \
      eprint("Semantic error\n%s\n", s);               \
    exit(Semantic_Error);                              \
  } while (0)
#define elsefree(pointer)       \
  else {                        \
    if (pointer) free(pointer); \
  }
#endif