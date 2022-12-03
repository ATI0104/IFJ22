#ifndef _token
#define _token
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "string.h"
#define LEXICAL_ERROR 1
// Token types
#define _EOF 100                  // 'EOF'
#define _prolog 101               // 'prolog'
#define _function 102             // 'function'
#define _if 103                   // 'if'
#define _else 104                 // 'else'
#define _while 105                // 'while'
#define _return 106               // 'return'
#define _int 107                  // 'int'
#define _string 108               // 'string'
#define _null 109                 // 'null'
#define _void 110                 // 'void'
#define _dot 111                  // '.'
#define _e 112                    // '[eE]1'
#define _variable 113             // '$[a-zA-Z_]+
#define _left_parenthesis 114     // '('
#define _right_parenthesis 115    // ')'
#define _semicolon 116            // ';'
#define _colon 117                // ':'
#define _equals 118               // '='
#define _plus 119                 // '+'
#define _minus 120                // '-'
#define _multiply 121             // '*'
#define _divide 122               // '/'
#define _lessthan 123             // '<'
#define _greaterthan 124          // '>'
#define _lessthanoreq 125         // '<='
#define _greaterthanoreq 126      // '>='
#define _typecheck 127            // '==='
#define _not_typecheck 128        // '!=='
#define _array 129                // '"'
#define _identificator 130        //[a-zA-Z_]+
#define _comma 131                // ','
#define _number 132               // [0-9]+
#define _left_curly_bracket 133   // '{'
#define _right_curly_bracket 134  // '}'
#define _question_mark 135        // '?'
#define _float 136                // 'float'
#define _decimalnumber 137        // [0-9]+.1[0-9]+
#define _true 138                 // 'true'
#define _false 139                // 'false'
#define _bool 140                   // 'bool'
typedef struct token {
  int type;
  string *str;
  int *i_val;
  int *linenum;
  double *f_val;
} token;
token get_token();
int is_whitespace(char c, bool *one_line_comment, bool *multi_line_comment,
                  int *linenum);
void get_identificator(string *word, char *re);
bool regex_check(char *txt, char *re);
void slash_decode(string *str);

#endif