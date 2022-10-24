#ifndef _mymacros
#define _mymacros
#define maloc(pointer, size)    \
  do {                          \
    pointer = malloc(size);     \
    if (pointer == NULL) {      \
      eprint("Malloc Error\n"); \
      exit(99);                 \
    }                           \
  } while (0)

#define eprint(...)               \
  do {                            \
    fprintf(stderr, __VA_ARGS__); \
  } while (0)

#define lexerror                                 \
  do {                                           \
    eprint("Lexical Error on line %d", linenum); \
    string_destroy(word);                        \
    exit(LEXICAL_ERROR);                         \
  } while (0)

#endif