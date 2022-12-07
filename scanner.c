/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Attila Kovács (xkovac60)
  * @file scanner.c
*/
#include "scanner.h"

/**
 * @brief regex function
 *
 * @param txt input string
 * @param re regex to match
 * @return true
 * @return false
 */
bool regex_check(char *txt, char *re) {
  if (re == NULL) return false;
  regex_t *regex = calloc(1, sizeof(regex_t));
  if (regex == NULL) {
    eprint("Malloc Error\n");
    exit(99);
  }
  int reti;
  reti = regcomp(regex, re, REG_EXTENDED);
  if (reti) {
    eprint("Regex Compilation error.\n");
    exit(99);
  }
  reti = regexec(regex, txt, 0, NULL, 0);
  regfree(regex);
  free(regex);
  if (!reti) return true;
  return false;
}
/**
 * @brief Generate a token from stdin
 *
 * @return token
 */
token get_token() {
  static int linenum = 1;
  int *lnum;
  maloc(lnum, sizeof(int));
  static bool prolog_found = false;   //<?php
  static bool prolog_found1 = false;  // declare(strict_types=1)
  static bool end = false;            // ?> found
#ifdef _skip_prolog_check
  prolog_found1 = true;
  prolog_found = true;
#endif
  string *word;
  maloc(word, sizeof(string));
  string_init(word);
  token t;
  t.i_val = NULL;
  t.f_val = NULL;
  t.str = NULL;
  int c = getc(stdin);
  int j;
  bool one_line_comment = false;
  bool multi_line_comment = false;
  while (c != EOF) {
    if (c == 13) {  // Compatibility with CRLF files
      c = getc(stdin);
    }
    if (end) {
      if (c == '\n') {
        c = getc(stdin);
        if (c == EOF) {
          t.type = _EOF;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
      }
      eprint("Something is after \'?>\'.\n");
      exit(LEXICAL_ERROR);
    }
    if (!prolog_found) {
      if (c == '<') {
        string_appendc(word, c);
        for (size_t i = 0; i < 4; i++) {
          c = getc(stdin);
          string_appendc(word, c);
        }
        if (strcmp((*word).txt, "<?php") == 0) {
          prolog_found = true;
          string_destroy(word);
          c = getc(stdin);
          continue;
        } else {
          eprint("Incorrect prolog \n Expected: \"<?php\" got \"%s\"\n",
                 (*word).txt);
          exit(LEXICAL_ERROR);
        }
      }
      eprint("Incorrect prolog \n Expected: \"<?php\" got \"%s\"\n", word->txt);
      exit(LEXICAL_ERROR);
    }
    if (one_line_comment) {
      if (c == '\n' || c == EOF) {
        linenum++;
        one_line_comment = false;
      }
      c = getc(stdin);
      continue;
    }
    if (multi_line_comment) {
      if (c == '*') {
        c = getc(stdin);
        if (c == '/') {
          multi_line_comment = false;
        }
      }
      if (c == '\n') {
        linenum++;
        c = getc(stdin);
        continue;
      }
      c = getc(stdin);
      continue;
    }
    if (c == '\n') {
      linenum++;
      c = getc(stdin);
      continue;
    }
    j = is_whitespace(c, &one_line_comment, &multi_line_comment, &linenum);
    switch (j) {  // whitespace/comment skipper
      case 0:
        break;
      case 1:
      case 2:
        c = getc(stdin);
        continue;
        break;
      case 3:
        if (prolog_found1) {
          t.type = _divide;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        } else
          lexerror;
        break;
    }
    if (!prolog_found1) {
      if (c == 'd') {
        string_appendc(word, c);
        for (size_t i = 0; i < 23; i++) {
          c = getc(stdin);
          if (c == 13) {  // Compatibility with CRLF files
            c = getc(stdin);
          }
          if (one_line_comment) {
            if (c == '\n') {
              linenum++;
              one_line_comment = false;
            }

            i--;
            continue;
          }
          if (multi_line_comment) {
            if (c == '*') {
              c = getc(stdin);
              if (c == '/') {
                multi_line_comment = false;
              }
            }
            if (c == '\n') {
              linenum++;

              i--;
              continue;
            }

            i--;
            continue;
          }
          if (c == '\n') {
            linenum++;

            i--;
            continue;
          }
          j = is_whitespace(c, &one_line_comment, &multi_line_comment,
                            &linenum);
          switch (j) {  // whitespace/comment skipper
            case 0:
              break;
            case 1:
            case 2:
              i--;
              continue;
              break;
            case 3:
              if (prolog_found1) {
                t.type = _divide;
                string_destroy(word);
                *lnum = linenum;
                t.linenum = lnum;
                return t;
              } else
                lexerror;
              break;
          }
          string_appendc(word, c);
        }
        if (strcmp(word->txt, "declare(strict_types=1);") == 0) {
          prolog_found1 = true;
          t.type = _prolog;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
      }
      eprint(
          "Incorrect prolog \n Expected: \"declare(strict_types=1);\" got "
          "\"%s\"\n",
          word->txt);
      string_destroy(word);
      exit(LEXICAL_ERROR);
    };
    switch (c) {
      case 'f':;  // function / float
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "function") == 0) {
          t.type = _function;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        } else if (strcmp(word->txt, "float") == 0) {
          t.type = _float;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case 'i':  // if / int
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "if") == 0) {
          t.type = _if;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        if (strcmp(word->txt, "int") == 0) {
          t.type = _int;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case '$':;  // variable
        string varname;
        string_init(&varname);
        c = getc(stdin);
        string_appendc(&varname, c);
        get_identificator(&varname, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        string_set(word, "$");
        string_append(word, varname.txt);
        t.type = _variable;
        t.str = word;
        string_destroy(&varname);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case 'E':
      case 'e':  // else /eE
        c = getc(stdin);
        if ((c >= '0' && c <= '9') || c == '+' || c == '-') {
          t.type = _e;
          ungetc(c, stdin);
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        } else if (c == 'l') {
          string_set(word, "el");
          get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
          if (strcmp(word->txt, "else") == 0) {
            t.type = _else;
            string_destroy(word);
            *lnum = linenum;
            t.linenum = lnum;
            return t;
          }
        } else {
          string_appendc(word, 'e');
          string_appendc(word, c);
          get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        }
        break;
      case 'n':  // null
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "null") == 0) {
          t.type = _null;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case 'r':  // return
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "return") == 0) {
          t.type = _return;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case 's':  // string
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "string") == 0) {
          t.type = _string;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case 'v':  // void
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "void") == 0) {
          t.type = _void;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case 'w':  // while
        string_appendc(word, c);
        get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
        if (strcmp(word->txt, "while") == 0) {
          t.type = _while;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      case '!':  // !==
        string_appendc(word, c);
        c = getc(stdin);
        if (c == '=') {
          c = getc(stdin);
          if (c == '=') {
            t.type = _not_typecheck;
            string_destroy(word);
            *lnum = linenum;
            t.linenum = lnum;
            return t;
          }
          lexerror;
        }
        lexerror;
        break;
      case '=':  // =/===
        c = getc(stdin);
        if (c == '=') {
          c = getc(stdin);
          if (c == '=') {
            t.type = _typecheck;
            string_destroy(word);
            *lnum = linenum;
            t.linenum = lnum;
            return t;
          }
          lexerror;
        }
        ungetc(c, stdin);
        t.type = _equals;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '.':
        t.type = _dot;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case ':':
        t.type = _colon;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case ';':
        t.type = _semicolon;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '>':  //>/>=
        c = getc(stdin);
        if (c == '=') {
          t.type = _greaterthanoreq;
        } else {
          ungetc(c, stdin);
          t.type = _greaterthan;
        }
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '<':  //</<=
        c = getc(stdin);
        if (c == '=') {
          t.type = _lessthanoreq;
        } else {
          ungetc(c, stdin);
          t.type = _lessthan;
        }
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case ')':
      case '(':
        if (c == ')')
          t.type = _right_parenthesis;
        else
          t.type = _left_parenthesis;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '-':
        t.type = _minus;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '+':
        t.type = _plus;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '*':
        t.type = _multiply;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '"':
        c = getc(stdin);
        if (c == '\"') {
          string_set(word, "");
          t.str = word;
          t.type = _array;
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        string_appendc(word, c);
        while (1) {
          c = getc(stdin);
          if (c == '"') {
            if (word->txt[word->size - 1] == '\\' && word->txt[word->size - 2] != '\\') 
              ;
            else
              break;
          }
          if (c == EOF) {
            lexerror;
          }
          string_appendc(word, c);
        }
        slash_decode(word);
        t.str = word;
        t.type = _array;
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case ',':
        t.type = _comma;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
      case '{':
        t.type = _left_curly_bracket;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '}':
        t.type = _right_curly_bracket;
        string_destroy(word);
        *lnum = linenum;
        t.linenum = lnum;
        return t;
        break;
      case '?':
        c = getc(stdin);
        if (c == '>') {
          end = true;
          c = getc(stdin);
          continue;
        } else {
          ungetc(c, stdin);
          t.type = _question_mark;
          string_destroy(word);
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        break;
      default:
        // number or  identificator or error
        string_appendc(word, c);
        if (regex_check(word->txt,
                        "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$")) {  // identificator
          get_identificator(word, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$");
          t.type = _identificator;
          t.str = word;
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        if (regex_check(word->txt, "^[[:digit:]]+$")) {  // number
          get_identificator(word, "^[[:digit:]]+$");
          t.type = _number;
          t.str = word;
          *lnum = linenum;
          t.linenum = lnum;
          return t;
        }
        lexerror;
        break;
    }
    if (is_empty(word)) {
      lexerror;
    }
    if (regex_check(word->txt, "^[a-zA-Z_]{1}[a-zA-Z0-9_]*$")) {
      t.type = _identificator;
      t.str = word;
      *lnum = linenum;
      t.linenum = lnum;
      return t;
    } else {
      lexerror;
    }
  }
  if (prolog_found1 && prolog_found && !multi_line_comment) {
    t.type = _EOF;
    *lnum = linenum;
    t.linenum = lnum;
    return t;
  }
  lexerror;
}

/**
 * @brief Check if c is a whitespace
 *
 * @param c
 * @param one_line_comment
 * @param multi_line_comment
 * @param linenum
 * @return int returns 0 when it's not a whitespace, returns 1 when
 * it's ' ', returns 2 when it's the beginning of a comment, returns 3 when
 * it's division
 */
int is_whitespace(char c, bool *one_line_comment, bool *multi_line_comment,
                  int *linenum) {
  if (c == ' ') return 1;
  if (c == '\t') return 1;
  if (c == '/') {
    c = getc(stdin);
    if (c == '/') {
      *one_line_comment = 1;
      return 2;
    } else if (c == '*') {
      *multi_line_comment = 1;
      return 2;
    } else {
      ungetc(c, stdin);
      return 3;
    }
  }
  if (c == '\n') {
    (*linenum)++;
    return 1;
  }
  return 0;
}

/**
 * @brief Reads from stdin until it finds a character that is not matched by the
 * regex
 *
 * @param word // string where the read characters are appended
 * @param re  // regex
 */
void get_identificator(string *word, char *re) {
  int c;
  bool match = false;
  while (regex_check(word->txt, re)) {
    if (!match) match = true;
    c = getc(stdin);
    string_appendc(word, c);
  }
  if (match) {
    string_deletelast(word);
    ungetc(c, stdin);
  }
}
/**
 * @brief Converts special characters to their IFJ22 representation
 *
 * @param str
 */
void slash_decode(string *str) {
  if (str == NULL) return;
  char *tmp1 = str->txt;
  char *tmp2 = str->txt;
  char tab3[4];
  char tab2[3];
  char a[7];
  int dec;
  dec = 0;
  if (dec == 0) {
    ;
  }
  char *backup = str->txt;
  str->txt = NULL;
  str->size = 0;
  while (*tmp2 != '\0') {
    switch (*tmp2) {
      case '\\':
        switch (*(++tmp2)) {
          case '\\':
            *tmp2 = '\0';
            string_append(str, tmp1);
            strcpy(tab3, "092");
            string_append(str, tab3);
            tmp1 = ++tmp2;
            continue;
            break;
            return;
            break;
          case '"':
            *(tmp2 - 1) = '"';
            *(tmp2) = '\0';
            string_append(str, tmp1);
            tmp1 = ++tmp2;
            continue;
            return;
            break;
          case '$':
            *(tmp2 - 1) = '$';
            *(tmp2) = '\0';
            string_append(str, tmp1);
            tmp1 = ++tmp2;
            continue;
            return;
            break;
          case 't':
            *tmp2 = '\0';
            string_append(str, tmp1);
            strcpy(tab3, "009");
            string_append(str, tab3);
            tmp1 = ++tmp2;
            continue;
            break;
            return;
            break;
          case 'n':
            *tmp2 = '\0';
            string_append(str, tmp1);
            strcpy(tab3, "010");
            string_append(str, tab3);
            tmp1 = ++tmp2;
            continue;
            break;
          case 'x':
            tab2[0] = *(tmp2 + 1);
            tab2[1] = *(tmp2 + 2);
            if (!regex_check(tab2, "^[[:xdigit:]]+$")) {
              *(--tmp2) = '\0';
              string_append(str, tmp1);
              strcpy(a, "\\092");
              string_append(str, a);
              tmp1 = ++tmp2;
              continue;
            }
            int dec = strtol(tab2, NULL, 16);
            if ((dec >= 1 && dec <= 32) || dec == 35 || dec == 92 ||
                dec > 127) {
              sprintf(tab3, "%03d", dec);
              *tmp2 = tab3[0];
              *(tmp2 + 1) = tab3[1];
              *(tmp2 + 2) = tab3[2];
              tmp2 = tmp2 + 3;
              continue;
            } else {
              *(tmp2 - 1) = dec;
              *tmp2 = '\0';
              tmp2 = tmp2 + 2;
            }
            string_append(str, tmp1);
            tmp1 = tmp2;
            continue;
            break;
          case '0':
          case '1':
          case '2':
          case '3':
            tab3[0] = *tmp2;
            tab3[1] = *(tmp2 + 1);
            tab3[2] = *(tmp2 + 2);
            if (!regex_check(tab3 + 1, "^[0-7]{2}$")) {
              *(--tmp2) = '\0';
              string_append(str, tmp1);
              strcpy(a, "\\092");
              string_append(str, a);
              tmp1 = ++tmp2;
              continue;
            }
            dec = strtol(tab3, NULL, 8);
            if ((dec >= 1 && dec <= 32) || dec == 35 || dec == 92 ||
                dec > 127) {
              sprintf(tab3, "%03d", dec);
              *tmp2 = tab3[0];
              *(tmp2 + 1) = tab3[1];
              *(tmp2 + 2) = tab3[2];
              tmp2 = tmp2 + 3;
              continue;
            } else {
              *(tmp2 - 1) = dec;
              *tmp2 = '\0';
              tmp2 = tmp2 + 3;
            }
            string_append(str, tmp1);
            tmp1 = tmp2;
            continue;
            break;
          default:
            *(--tmp2) = '\0';
            string_append(str, tmp1);
            strcpy(a, "\\092");
            string_append(str, a);
            tmp1 = ++tmp2;
            continue;
            break;
        }
        break;
      case ' ':
      case '\t':
      case '\n':
        dec = *tmp2;
        *tmp2 = '\0';
        string_append(str, tmp1);
        sprintf(a, "\\%03d", dec);
        string_append(str, a);
        ++tmp2;
        tmp1 = tmp2;
        continue;
        break;
      case '#':
        dec = *tmp2;
        *tmp2 = '\0';
        string_append(str, tmp1);
        sprintf(a, "\\%03d", dec);
        string_append(str, a);
        ++tmp2;
        tmp1 = tmp2;
        continue;
        break;
        break;
      default:
        if (*tmp2 == '$' && *(tmp2 - 1) != '\\') {
          exit(LEXICAL_ERROR);
        }
        break;
    }
    ++tmp2;
  }
  string_append(str, tmp1);
  free(backup);
  if (str == NULL) return;
  return;
}
