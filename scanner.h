#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "string.h"
#ifndef _token
#define _token
#define LEXICAL_ERROR 1
#define eprint(...)               \
  do {                            \
    fprintf(stderr, __VA_ARGS__); \
  } while (0)

#define error                                    \
  do {                                           \
    eprint("Lexical Error on line %d", linenum); \
    string_destroy(&word);                       \
    exit(LEXICAL_ERROR);                         \
  } while (0)

// Token types
#define _EOF 100                // 'EOF'
#define _prolog 101             // 'prolog'
#define _function 102           // 'function'
#define _if 103                 // 'if'
#define _else 104               // 'else'
#define _while 105              // 'while'
#define _return 106             // 'return'
#define _int 107                // 'int'
#define _string 108             // 'string'
#define _null 109               // 'null'
#define _void 110               // 'void'
#define _dot 111                // '.'
#define _e 112                  // '[eE]1'
#define _variable 113           // '$[a-zA-z_]+
#define _left_parenthesis 114   // '('
#define _right_parenthesis 115  // ')'
#define _semicolon 116          // ';'
#define _colon 117              // ':'
#define _equals 118             // '='
#define _plus 119               // '+'
#define _minus 120              // '-'
#define _multiply 121           // '*'
#define _divide 122             // '/'
#define _lessthan 123           // '<'
#define _greaterthan 124        // '>'
#define _lessthanoreq 125       // '<='
#define _greaterthaneq 126      // '>='
#define _typecheck 127          // '==='
#define _not_typecheck 128      // '!=='
#define _array 129              // '"'
#define _idenftificator 130     //[a-zA-z_]+

typedef struct token {
  int type;
  string *str;
  int *i_val;
  int *linenum;
} token;
token get_token();
int is_whitespace(char c, bool *one_line_comment, bool *multi_line_comment,
                  int *linenum);
#endif