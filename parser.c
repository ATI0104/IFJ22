#include "parser.h"
my_favorites fav;  //♥ This is the only global variable in the program let's
                   // keep it that way :)
int check_precedence(int* op) {
  if (op == NULL) return -1;
  switch (*op) {
    case _multiply:
    case _divide:
      return 4;
    case _plus:
    case _minus:
    case _dot:
      return 3;
      break;
    case _lessthan:
    case _greaterthan:
    case _lessthanoreq:
    case _greaterthanoreq:
      return 2;
    case _typecheck:
    case _not_typecheck:
      return 1;
    case _left_parenthesis:
      return 0;
      break;
    default:
      return -1;
      break;
  }
  eprint("Invalid operator.\n");
  exit(LEXICAL_ERROR);  // Not sure if this is the right error code
}
void expr_toprefix(expr** e) {
  if (e == NULL) return;
  expr* tmp;
  tmp = NULL;
  int* j;
  int top = 0;
  expr* a = *e;
  Stack* op_stack = NULL;
  Stack_Init(&op_stack);

  while (true) {
    a = expr_pop(e);
    if (a == NULL) break;
    if (a->op == NULL) {
      expr_add(&tmp, a->type, a->str, a->num, a->fl, a->op, a->var, a->func,
               a->typekeywords);
      free(a);
      continue;
    }
    if (a->op[0] == _left_parenthesis) {
      Stack_Push(&op_stack, a->op[0]);
    } else if (a->op[0] == _right_parenthesis) {
      while (!Stack_IsEmpty(op_stack)) {  // Going through the stack until it's
                                          // empty or '(' is found
        Stack_Top(op_stack,
                  &top);      // Getting the character form the top of the stack
        Stack_Pop(op_stack);  // Removing the top character

        if (top == _left_parenthesis) {  //'(' is reached we remove it from the
                                         // stack and return
          break;
        }
        maloc(j, sizeof(int));
        *j = top;
        expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL, NULL);
      }
      if (top != _left_parenthesis) {
        *e = NULL;
        return;  // replace with the right exit code
      }
    } else if (a->op[0] == _equals) {
      while (!Stack_IsEmpty(op_stack)) {
        int k = 0;
        Stack_Top(op_stack, &k);
        Stack_Pop(op_stack);
        maloc(j, sizeof(int));
        *j = k;
        expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL, NULL);
      }
      break;
    } else {
      Stack_Top(op_stack, &top);
      while (!Stack_IsEmpty(op_stack) &&
             check_precedence(a->op) <= check_precedence(&top)) {
        maloc(j, sizeof(int));
        *j = top;
        expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL, NULL);
        Stack_Pop(op_stack);
        Stack_Top(op_stack, &top);
      }
      Stack_Push(&op_stack, a->op[0]);
    }
  }
  while (!Stack_IsEmpty(op_stack)) {
    int k = 0;
    Stack_Top(op_stack, &k);
    Stack_Pop(op_stack);
    maloc(j, sizeof(int));
    *j = k;
    expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL, NULL);
  }
  a = *e;
  *e = tmp;
  expr_destroy(&a, false);
  return;
}

expr* read_expression(int brackets, bool exprinif, string* fname) {
  if (fav.t == NULL) return NULL;
  bool checkvars = false;
  function_table* f = NULL;
  if (fname != NULL) {
    checkvars = true;
    f = function_table_get(&(fav.f), *fname);
  }
  expr* tmp;
  expr_init(&tmp);
  bool operandexpected = true;
  bool operatorexpected = false;
  while (fav.t != NULL) {
    if (fav.t == NULL) {
      eprint("Incorrect expression\n");
      exit(4);  // Add correct exit code
    }
    if ((fav.t->t.type >= _plus && fav.t->t.type <= _not_typecheck) ||
        fav.t->t.type == _dot) {
      if (!operatorexpected) {
        return NULL;
      }
      expr_add(&tmp, 0, NULL, NULL, NULL, &(fav.t->t.type), NULL, NULL, NULL);
      fav.t = fav.t->next;
      operandexpected = true;
      operatorexpected = false;
      continue;
    }
    if (fav.t->t.type == _left_parenthesis) {
      if (!operandexpected) {
        return NULL;
      }
      expr_add(&tmp, 0, NULL, NULL, NULL, &(fav.t->t.type), NULL, NULL, NULL);
      fav.t = fav.t->next;
      brackets++;
      continue;
    }
    if (fav.t->t.type == _right_parenthesis) {
      expr_add(&tmp, 0, NULL, NULL, NULL, &(fav.t->t.type), NULL, NULL, NULL);
      brackets--;
      fav.t = fav.t->next;
      continue;
    }
    if (fav.t->t.type == _semicolon || fav.t->t.type == _left_curly_bracket) {
      if (!operatorexpected) return NULL;
      if (brackets == 0) {
        if (exprinif) {
          if (fav.t->t.type == _left_curly_bracket) {
            return tmp;
          }
        } else {
          if (fav.t->t.type == _semicolon) {
            return tmp;
          }
        }
        return NULL;
      }
      return NULL;
    }
    if (operandexpected) {
      operandexpected = false;
      switch (fav.t->t.type) {
        case _number:
          expr_add(&tmp, -1, NULL, fav.t->t.i_val, NULL, NULL, NULL, NULL,
                   NULL);
          fav.t = fav.t->next;
          operatorexpected = true;
          break;
        case _decimalnumber:
          expr_add(&tmp, -1, NULL, NULL, fav.t->t.f_val, NULL, NULL, NULL,
                   NULL);
          fav.t = fav.t->next;
          operatorexpected = true;
          break;
        case _variable:
          if (checkvars) {
            if (var_table_get(&(f->variable), *(fav.t->t.str)) == NULL) {
              undefined_variable(-1);
            }
          }
          expr_add(&tmp, -1, NULL, NULL, NULL, NULL, fav.t->t.str, NULL, NULL);
          fav.t = fav.t->next;
          operatorexpected = true;
          break;
        case _array:
          expr_add(&tmp, -1, fav.t->t.str, NULL, NULL, NULL, NULL, NULL, NULL);
          fav.t = fav.t->next;
          operatorexpected = true;
          break;
        case _identificator:;
          call* cal = load_function_call();
          expr_add(&tmp, -1, NULL, NULL, NULL, NULL, NULL, cal, NULL);
          fav.t = fav.t->next;
          operatorexpected = true;
          break;
        case _int:
        case _float:
        case _string:
        case _null:;
          int* type;
          maloc(type, sizeof(int));
          *type = fav.t->t.type;
          expr_add(&tmp, 0, NULL, NULL, NULL, NULL, NULL, NULL, type);
          fav.t = fav.t->next;
          operatorexpected = true;
          break;
        default:
          return false;
          break;
      }
    } else
      return NULL;
  }
  return NULL;
}
bool foundininput(input_param_list* list, string* name) {
  if (list == NULL) return false;
  while (list != NULL) {
    if (list->name.txt != NULL)
      if (strcmp(name->txt, list->name.txt) == 0) return true;
    list = list->next;
  }
  return false;
}

input_param_list* load_input_params(tlist* t, int* skip) {
  *skip = 0;
  input_param_list* l;
  maloc(l, sizeof(input_param_list));
  input_param_list* tmp = l;
  while (t->t.type != _right_parenthesis) {
    if (t->t.type == _comma) {
      t = t->next;
      *skip = *skip + 1;
      continue;
    }
    switch (t->t.type) {
      case _int:;
        tmp->type = _int;
        t = t->next;
        *skip = *skip + 1;
        if (t->t.type == _variable) {
          if (foundininput(l, t->t.str)) {
            semantic_error(*t->t.linenum, "Duplicate input parameter name");
          }
          tmp->name = *(t->t.str);
          *skip = *skip + 1;
          t = t->next;
        } else {
          eprint("Error: Expected variable name after int\n");
          syntaxerror(*(t->t.linenum));
        }
        break;
      case _float:;
        tmp->type = _float;
        t = t->next;
        *skip = *skip + 1;
        if (t->t.type == _variable) {
          if (foundininput(l, t->t.str)) {
            semantic_error(*t->t.linenum, "Duplicate input parameter name");
          }
          tmp->name = *(t->t.str);
          *skip = *skip + 1;
          t = t->next;
        } else {
          eprint("Error: Expected variable name after float\n");
          syntaxerror(*(t->t.linenum));
        }
        break;
      case _string:

        tmp->type = _string;
        t = t->next;
        *skip = *skip + 1;
        if (t->t.type == _variable) {
          if (foundininput(l, t->t.str)) {
            semantic_error(*t->t.linenum, "Duplicate input parameter name");
          }
          tmp->name = *(t->t.str);

          t = t->next;
          *skip = *skip + 1;
        } else {
          eprint("Error: Expected variable name after string\n");
          syntaxerror(*(t->t.linenum));
        }
        break;
      case _void:
        tmp->type = _void;
        t = t->next;

        *skip = *skip + 1;
        break;
      case _question_mark:
        tmp->question_mark = true;
        t = t->next;
        *skip = *skip + 1;
        continue;
        break;
      default:
        eprint("Error: Expected type after comma\n");
        syntaxerror(*(t->t.linenum));
        break;
    }
    if (t->t.type != _right_parenthesis) {
      maloc(tmp->next, sizeof(input_param_list));
      tmp = tmp->next;
    }
  }
  if (*skip == 0) {  // no input params
    free(l);
    return NULL;
  }
  return l;
}
void add_func(function_table** tree, tlist* t) {
  if (t == NULL) return;
  while (t->t.type != _EOF) {
    if (t->t.type == _function) {
      function_table* tmp;
      maloc(tmp, sizeof(function_table));
      t = t->next;
      if (t->t.type == _identificator) {
        tmp->name = *(t->t.str);
        t = t->next;
        if (t->t.type == _left_parenthesis) {
          t = t->next;
          int skip = 0;
          tmp->input_type = load_input_params(t, &skip);
          while (skip) {
            t = t->next;
            skip--;
          }
          if (t->t.type == _right_parenthesis) t = t->next;
          if (t->t.type == _colon) {
            t = t->next;
            if (t->t.type == _question_mark) {
              t = t->next;
              tmp->questionmark = true;
            }
            if (t->t.type == _int) {
              tmp->output_type = _int;
              t = t->next;
            }
            if (t->t.type == _float) {
              tmp->output_type = _float;
              t = t->next;
            }
            if (t->t.type == _string) {
              tmp->output_type = _string;
              t = t->next;
            }
            if (t->t.type == _void) {
              tmp->output_type = _void;
            }
          } else {
            tmp->output_type = _null;
          }
          tmp->variable = load_variables(t, tmp->input_type);
          if (function_table_get(tree, tmp->name) != NULL) {
            function_redefinition(-1);
          }
          function_table_add(tree, tmp);
        }
        elsefree(tmp);
      }
      elsefree(tmp);
    }
    t = t->next;
  }
}
var_table* load_variables(tlist* t, input_param_list* input) {
  if (t == NULL) {
    return NULL;
  }
  var_table* root = NULL;
  var_table* leaf = NULL;
  int count = 1;
  bool first = true;
  while (count) {
    if (t->t.type == _left_curly_bracket) {
      if (!first)
        count++;
      else {
        first = false;
      }
    } else if (t->t.type == _right_curly_bracket) {
      count--;
    } else if (t->t.type == _variable) {
      string* varname = t->t.str;
      tlist* temp = t->next;
      if (temp->t.type == _equals) {
        if (var_table_get(&root, *varname) == NULL) {
          maloc(leaf, sizeof(var_table));
          leaf->name = *(t->t.str);
          var_table_add(&root, leaf);
        }
      } else {
        if (var_table_get(&root, *(t->t.str)) == NULL) {
          input_param_list* input_parameters = input;
          bool notfound = true;
          while (input_parameters != NULL) {
            if (strcmp(input_parameters->name.txt, t->t.str->txt) == 0) {
              notfound = false;
              break;
            }
            input_parameters = input_parameters->next;
          }
          if (notfound) {
            eprint("Variable %s is not declared\n", t->t.str->txt);
            undefined_variable(*(t->t.linenum));
          }
        }
      }
    }
    t = t->next;
  }
  return root;
}

// Moves tokens which are not part of a function to the end of the token list
tlist* move_tokens(tlist* t, tlist** mainfunction,
                   var_table** globalvariables) {
  if (mainfunction == NULL) {
    eprint("Incorrect Function call");
    exit(Internal_Error);
  }
  if (globalvariables == NULL) {
    eprint("Incorrect Function call");
    exit(Internal_Error);
  }
  tlist* functions = NULL;
  tlist* code = NULL;  //("pseudo main function")
  token codebegin;
  codebegin.type = _left_curly_bracket;
  code = tlist_add(code, codebegin);
  while (t->next !=
         NULL) {  // Looping through all of the tokens except the last one(EOF)
    if (t->t.type == _prolog) {
      functions = tlist_add(functions, t->t);
      t = t->next;
    } else if (t->t.type == _function) {
      int skip = function_skip(t);
      while (skip) {
        functions = tlist_add(functions, t->t);
        t = t->next;
        skip--;
      }
    } else {
      code = tlist_add(code, t->t);
      t = t->next;
    }
  }
  if (code != NULL) {
    token codeend;
    codeend.type = _right_curly_bracket;
    code = tlist_add(code, codeend);
  }
  functions = tlist_add(functions, t->t);  // Adding EOF to the end
  *mainfunction = code;
  *globalvariables = load_variables(code, NULL);
  return functions;
}
// Skips the function definition
// Returns the number of tokens for skipping the function
// Should be called when the _function token is the current
int function_skip(tlist* tokens) {
  int brackets = 0;
  if (tokens->t.type != _function) {
    return -1;
  }
  bool firstbracketfound = false;
  int skip = 0;
  while (true) {
    if (tokens->t.type == _left_curly_bracket) {
      brackets++;
      firstbracketfound = true;
    }
    if (firstbracketfound) {
      if (tokens->t.type == _right_curly_bracket) {
        brackets--;
        if (brackets == 0) {
          tokens = tokens->next;
          return ++skip;
        }
      }
    }
    if (tokens->t.type == _EOF) {
      syntaxerror(*tokens->t.linenum);
    }
    skip++;
    tokens = tokens->next;
  }
}

tlist* tlist_add(tlist* t, token tok) {
  tlist* tmp = t;
  if (tmp == NULL) {
    maloc(tmp, sizeof(tlist));
    tmp->t = tok;
    return tmp;
  }
  tlist* tmp2 = tmp;
  while (tmp2->next != NULL) {
    tmp2 = tmp2->next;
  }
  maloc(tmp2->next, sizeof(tlist));
  tmp2->next->t = tok;
  return tmp;
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! UNTIL THIS POINT EVERYTHING SHOULD BE
//! WORKING!!!!!!!!!!!!!!!!!!!!!!!

// Returns the called function, the first item in tlist should be the
// function's name

call* load_function_call() {
  function_table* temp;
  if (fav.t->t.type == _identificator) {
    temp = function_table_get(&fav.f, *(fav.t->t.str));
    if (temp == NULL) {
      function_undefined(*(fav.t->t.linenum));
    }
    fav.t = fav.t->next;
    if (fav.t->t.type != _left_parenthesis) {
      eprint("Expected left parenthesis after function name\n");
      syntaxerror(*(fav.t->t.linenum));
    }
    fav.t = fav.t->next;
    call* c;
    maloc(c, sizeof(call));
    c->function_name = &(temp->name);
    input* i;
    maloc(i, sizeof(input));
    c->in = i;
    input* tmp = i;
    if (fav.t->t.type == _right_parenthesis) {
      free(tmp);
      c->function_name = &(temp->name);
      c->in = NULL;
      return c;
    }
    while (fav.t->t.type != _right_parenthesis) {
      if (fav.t->t.type == _comma) {
        fav.t = fav.t->next;
        continue;
      }
      if (fav.t->t.type == _question_mark) {
        maloc(tmp->question_mark, sizeof(bool));
        fav.t = fav.t->next;
      }
      switch (fav.t->t.type) {
        case _number:
          tmp->i = fav.t->t.i_val;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          fav.t = fav.t->next;
          continue;
          break;
        case _decimalnumber:
          tmp->f = fav.t->t.f_val;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          fav.t = fav.t->next;
          continue;
          break;
        case _variable:
          tmp->var = fav.t->t.str;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          fav.t = fav.t->next;

          continue;
          break;
        case _array:
          tmp->s = fav.t->t.str;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          fav.t = fav.t->next;
          continue;
          break;
        case _null:
          maloc(tmp->null, sizeof(bool));

          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          fav.t = fav.t->next;
          continue;
          break;
        default:
          eprint("Invalid input type\n");
          if (fav.t != NULL && fav.t->t.linenum != NULL)
            syntaxerror(*(fav.t->t.linenum));
          else
            syntaxerror(-1);
          break;
      }
    }
    tmp = i;
    while (tmp->next->next != NULL) {
      tmp = tmp->next;
    }
    input* empty = tmp->next;
    tmp->next = NULL;
    free(empty);
    return c;
  }
  return NULL;
}

/**
 * @brief Reverses a linked list
 *Inspiration taken from:
 *https://www.interviewbit.com/blog/reverse-a-linked-list/
 * @param e Expression to be reversed
 * @return expr* Reversed expression
 */
expr* expression_flip(expr* e) {
  if (e == NULL) return NULL;
  if (e->next == NULL) return e;
  expr* result = expression_flip(e->next);
  e->next->next = e;
  e->next = NULL;
  return result;
}
void combinevartables(function_table** root) {
  if (root == NULL) return;
  function_table* f = *root;
  if (f == NULL) return;
  combinevartables(&f->left_func);
  combinevartables(&f->right_func);
  input_param_list* tmp = f->input_type;
  if (tmp == NULL) return;
  var_table* result = f->variable;
  while (tmp != NULL) {
    var_table* inputleaf;
    maloc(inputleaf, sizeof(var_table));
    inputleaf->name = tmp->name;
    inputleaf->type = tmp->type;
    var_table_add(&result, inputleaf);
    tmp = tmp->next;
  }
  if (f->variable == NULL) f->variable = result;
}

bool isin(int i, int l[]) {
  int j = 0;
  while (l[j] != 0) {
    if (i == l[j]) return true;
    j++;
  }
  return false;
}

/* AST creation including semantics analysis
 * Should be called after the code found
 * outside of functions is moved to the mainfunction list)*/
AST* ConvertToAst(tlist* functions, tlist* mainfunction,
                  var_table* globalvars) {
  AST* tmp;
  AST_init(&tmp);
  combinevartables(&fav.f);
  while (functions != NULL) {
    switch (functions->t.type) {
      case _function:
        functions = functions->next;
        string* fname = functions->t.str;
        while (functions->t.type !=
               _left_curly_bracket) {  // skipping input and output
                                       // parameters(already in symbol table)
          functions = functions->next;
        }
        functions = functions->next;
        fav.t = functions;
        AST_add(&tmp, fname, ConvertToCode(fname));
        functions = fav.t;
        break;
    }
    functions = functions->next;
  }
  string* main;
  maloc(main, sizeof(string));
  string_init(main);
  string_set(main, "m@in");
  function_table* mainf;
  maloc(mainf, sizeof(function_table));
  mainf->name = *main;
  mainf->variable = globalvars;
  function_table_add(&(fav.f), mainf);
  fav.t = mainfunction->next;  // Skipping the first left_curly_bracket
  AST_add(&tmp, main, ConvertToCode(main));
  return tmp;
}
code* ConvertToCode(string* fname) {
  code* result;
  code_init(&result);
  while (fav.t->t.type != _right_curly_bracket) {
    switch (fav.t->t.type) {
      {
        case _variable:;
          string* varname = fav.t->t.str;
          tlist* tmp = fav.t;
          tmp = tmp->next;
          if (tmp->t.type == _equals) {
            tmp = tmp->next;
            fav.t = tmp;
            expr* e = read_expression(0, false, fname);
            e = add_parenthesis(e);
            expr_toprefix(&e);
            fav.t = fav.t->next;
            code_add(&result, 0, NULL, NULL, NULL, e, NULL, varname, NULL);
          } else {
            expr* e = read_expression(0, false, fname);
            e = add_parenthesis(e);
            expr_toprefix(&e);
            if (fav.t->t.type == _semicolon) {
              fav.t = fav.t->next;
            }
            code_add(&result, 0, NULL, NULL, NULL, e, NULL, NULL, NULL);
          }
          break;
        case _if:
          fav.t = fav.t->next;
          expr* e = read_expression(0, true, fname);
          e = add_parenthesis(e);
          expr_toprefix(&e);
          if (fav.t->t.type == _left_curly_bracket) {
            fav.t = fav.t->next;
            code* iftrue = ConvertToCode(fname);
            if (fav.t->t.type == _right_curly_bracket) {
              fav.t = fav.t->next;
              if (fav.t->t.type == _else) {
                fav.t = fav.t->next->next;
                code* iffalse = ConvertToCode(fname);
                code_add(&result, 0, iftrue, iffalse, NULL, e, NULL, NULL,
                         NULL);
                if (fav.t->t.type == _right_curly_bracket) {
                  fav.t = fav.t->next;
                }
              } else {
                eprint("Missing Else.\n");
              }
            } else {
              eprint("Missing {.\n");
            }
          }
          break;
        case _while:
          fav.t = fav.t->next;
          expr* e2 = read_expression(0, true, fname);
          e2 = add_parenthesis(e2);
          expr_toprefix(&e2);
          if (fav.t->t.type == _left_curly_bracket) {
            fav.t = fav.t->next;
            code* whiletrue = ConvertToCode(fname);
            if (fav.t->t.type == _right_curly_bracket) {
              code_add(&result, 0, NULL, NULL, whiletrue, e2, NULL, NULL,
                       false);
              fav.t = fav.t->next;
            }
          }
          break;
        case _return:
          fav.t = fav.t->next;
          expr* e3 = read_expression(0, false, fname);
          e3 = add_parenthesis(e3);
          expr_toprefix(&e3);
          if (fav.t->t.type == _semicolon) {
            fav.t = fav.t->next;
          }
          code_add(&result, 0, NULL, NULL, NULL, e3, NULL, NULL, true);
          break;
        case _identificator:;
          call* c = load_function_call();
          code_add(&result, 0, NULL, NULL, NULL, NULL, c, NULL, NULL);
          fav.t = fav.t->next;
          if (fav.t->t.type == _semicolon) {
            fav.t = fav.t->next;
          }
          break;
        default:;
          expr* e4 = read_expression(0, false, fname);
          if (!e4) {
            eprint("loopde loop!\n");
          }
          e4 = add_parenthesis(e4);
          expr_toprefix(&e4);
          if (fav.t->t.type == _semicolon) {
            fav.t = fav.t->next;
          }
          code_add(&result, 0, NULL, NULL, NULL, e4, NULL, NULL, NULL);
          break;
      }
    }
  }
  return result;
}
void varlist_set(varlist** v, string* name, int type) {
  if (v == NULL) return;
  if (*v == NULL) {
    maloc(*v, sizeof(varlist));
    (*v)->name = name;
    (*v)->current_type = type;
    return;
  } else {
    varlist* last = NULL;
    varlist* tmp = *v;
    bool set = false;
    while (tmp != NULL) {
      if (strcmp(tmp->name->txt, name->txt) == 0) {
        tmp->current_type = type;
        set = true;
      }
      last = tmp;
      tmp = tmp->next;
    }
    if (!set) {
      maloc(last->next, sizeof(varlist));
      last->next->name = name;
      last->next->current_type = type;
    }
  }
}

int varlist_get(varlist* v, string* name) {
  if (v == NULL) return 0;
  varlist* tmp = v;
  while (tmp != NULL) {
    if (strcmp(tmp->name->txt, name->txt) == 0) {
      return tmp->current_type;
    }
    tmp = tmp->next;
  }
  return 0;
}
/*Syntax analysis starts here*/  // TODO add more error checking
bool check_syntax(tlist* t, function_table* f) {
  fav.t = t;
  fav.f = f;
  return prog();
}
bool prog() {
  if (fav.t->t.type == _prolog) {
    fav.t = fav.t->next;
    return prog2();
  }
  return false;
}

bool prog2() {
  if (fav.t->t.type == _EOF) {
    return true;
  }
  if (fav.t->t.type == _function) {
    return function_definition() && prog2();
  } else {
    return body() && prog2();
  }
}

bool function_definition() {
  if (fav.t->t.type == _function) {
    fav.t = fav.t->next;
    if (fav.t->t.type == _identificator) {
      fav.t = fav.t->next;
      if (fav.t->t.type == _left_parenthesis) {
        fav.t = fav.t->next;
        if (def_params()) {
          if (fav.t->t.type == _right_parenthesis) {
            fav.t = fav.t->next;
            if (fav.t->t.type == _colon) {
              fav.t = fav.t->next;
              if (type()) {
                if (fav.t->t.type == _left_curly_bracket) {
                  fav.t = fav.t->next;
                  if (body()) {
                    if (fav.t->t.type == _right_curly_bracket) {
                      fav.t = fav.t->next;
                      return true;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}

bool params() {
  bool comma = false;
  while (fav.t->t.type != _right_parenthesis) {
    if (fav.t->t.type == _variable) {
      comma = false;
      fav.t = fav.t->next;
      if (fav.t->t.type == _comma) {
        comma = true;
        fav.t = fav.t->next;
        continue;
      }
    } else if (fav.t->t.type == _number) {
      comma = false;
      fav.t = fav.t->next;
      if (fav.t->t.type == _comma) {
        comma = true;
        fav.t = fav.t->next;
        continue;
      }
    } else if (fav.t->t.type == _decimalnumber) {
      comma = false;
      fav.t = fav.t->next;
      if (fav.t->t.type == _comma) {
        comma = true;
        fav.t = fav.t->next;
        continue;
      }
    } else if (fav.t->t.type == _array) {
      comma = false;
      fav.t = fav.t->next;
      if (fav.t->t.type == _comma) {
        comma = true;
        fav.t = fav.t->next;
        continue;
      }
    } else if (fav.t->t.type == _null) {
      comma = false;
      fav.t = fav.t->next;
      if (fav.t->t.type == _comma) {
        comma = true;
        fav.t = fav.t->next;
        continue;
      }
    } else if (fav.t->t.type == _right_parenthesis) {
      return true;
    } else
      return false;
  }
  if (fav.t->t.type == _right_parenthesis) {
    if (comma) return false;
    return true;
  }
  return false;
}
bool def_params() {
  if (fav.t->t.type == _right_parenthesis) return true;
  while (true) {
    if (type()) {
      if (fav.t->t.type == _variable) {
        fav.t = fav.t->next;
        if (fav.t->t.type == _comma) {
          fav.t = fav.t->next;
          continue;
        } else if (fav.t->t.type == _right_parenthesis) {
          return true;
        }

      } else if (fav.t->t.type == _number) {
        fav.t = fav.t->next;
        if (fav.t->t.type == _comma) {
          fav.t = fav.t->next;
          continue;
        } else if (fav.t->t.type == _right_parenthesis) {
          return true;
        }
      } else if (fav.t->t.type == _decimalnumber) {
        fav.t = fav.t->next;
        if (fav.t->t.type == _comma) {
          fav.t = fav.t->next;
          continue;
        } else if (fav.t->t.type == _right_parenthesis) {
          return true;
        }
      } else if (fav.t->t.type == _string) {
        fav.t = fav.t->next;
        if (fav.t->t.type == _comma) {
          fav.t = fav.t->next;
          continue;
        } else if (fav.t->t.type == _right_parenthesis) {
          return true;
        }
      } else if (fav.t->t.type == _null) {
        fav.t = fav.t->next;
        if (fav.t->t.type == _comma) {
          fav.t = fav.t->next;
          continue;
        } else if (fav.t->t.type == _right_parenthesis) {
          return true;
        }
      }
    }
    return false;
  }
  if (fav.t->t.type == _right_parenthesis) {
    return true;
  }
  return false;
}
bool type() {
  if (fav.t->t.type == _question_mark) {
    fav.t = fav.t->next;
  }
  if (fav.t->t.type == _int || fav.t->t.type == _float ||
      fav.t->t.type == _string || fav.t->t.type == _void) {
    fav.t = fav.t->next;
    return true;
  }
  return false;
}

bool body() {
  switch (fav.t->t.type) {
    case _if:
      if (if_statement()) {
        return body();
      }
      return false;
      break;
    case _while:
      if (while_statement()) {
        return body();
      }
      return false;
      break;
    case _return:
      if (return_statement()) {
        if (fav.t->t.type == _semicolon) {
          fav.t = fav.t->next;
          return body();
        }
      }
      return false;
      break;
    case _identificator:
      if (function_call()) {
        if (fav.t->t.type == _semicolon) {
          fav.t = fav.t->next;
          return body();
        }
      }
      return false;
      break;
    case _variable:;
      tlist* backup = fav.t;
      if (var_set()) {
        if (fav.t->t.type == _semicolon) {
          fav.t = fav.t->next;
          return body();
        }
      } else {
        fav.t = backup;
        if (expression_check(0, false)) {
          if (fav.t->t.type == _semicolon) {
            fav.t = fav.t->next;
            return body();
          }
        }
      }
      return false;
      break;
    case _semicolon:
      syntaxerror(*(fav.t->t.linenum));
      break;
    case _right_curly_bracket:
      return true;
      break;
    case _function:
      return prog2();
      break;
    case _EOF:
      return prog2();
      break;
    case _number:
    case _decimalnumber:
    case _string:
    case _null:
    case _int:
    case _float:
    case _array:
    case _left_parenthesis:
      if (expression_check(0, false)) {
        if (fav.t->t.type == _semicolon) {
          fav.t = fav.t->next;
          return body();
        }
      }
      return false;
      break;
    default:
      return false;
      break;
  }
}

bool if_statement() {
  if (fav.t->t.type == _if) {
    fav.t = fav.t->next;
    if (fav.t->t.type == _left_parenthesis) {
      fav.t = fav.t->next;
      if (expression_check(1, true)) {
        if (fav.t->t.type == _left_curly_bracket) {
          fav.t = fav.t->next;
          if (body()) {
            if (fav.t->t.type == _right_curly_bracket) {
              fav.t = fav.t->next;
              if (else_statement()) {
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}

bool else_statement() {
  if (fav.t->t.type == _else) {
    fav.t = fav.t->next;
    if (fav.t->t.type == _left_curly_bracket) {
      fav.t = fav.t->next;
      if (body()) {
        if (fav.t->t.type == _right_curly_bracket) {
          fav.t = fav.t->next;
          return true;
        }
      }
    }
  }
  return true;
}

bool while_statement() {
  if (fav.t->t.type == _while) {
    fav.t = fav.t->next;
    if (fav.t->t.type == _left_parenthesis) {
      fav.t = fav.t->next;
      if (expression_check(1, true)) {
        if (fav.t->t.type == _left_curly_bracket) {
          fav.t = fav.t->next;
          if (body()) {
            if (fav.t->t.type == _right_curly_bracket) {
              fav.t = fav.t->next;
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

bool return_statement() {
  if (fav.t->t.type == _return) {
    fav.t = fav.t->next;
    if (fav.t->t.type == _semicolon) return true;
    if (expression_check(0, false)) {
      return true;
    } else if (fav.t->t.type == _semicolon) {
      return true;
    }
  }
  return false;
}

bool var_set() {
  if (fav.t->t.type == _variable) {
    fav.t = fav.t->next;
    if (fav.t->t.type == _equals) {
      fav.t = fav.t->next;
      if (expression_check(0, false)) {
        return true;
      }
    }
  }
  return false;
}

bool function_call() {
  if (fav.t->t.type == _identificator) {
    if (function_table_get(&(fav.f), *(fav.t->t.str)) == NULL) {
      function_undefined(*(fav.t->t.linenum));
    }
    fav.t = fav.t->next;
    if (fav.t->t.type == _left_parenthesis) {
      fav.t = fav.t->next;
      if (params()) {
        if (fav.t->t.type == _right_parenthesis) {
          fav.t = fav.t->next;
          return true;
        }
      }
    }
  }
  return false;
}
bool expression_check(int brackets, bool exprinif) {
  expr* e = read_expression(brackets, exprinif, NULL);
  if (e == NULL) {
    return false;
  } else {
    e = add_parenthesis(e);
    expr_toprefix(&e);
    if (e) {
      return true;
    }
    return false;
  }
}

expr* add_parenthesis(expr* e) {
  expr* tmp;
  expr_init(&tmp);
  int* lpar;
  maloc(lpar, sizeof(int));
  *lpar = _left_parenthesis;
  expr_add(&tmp, 0, NULL, NULL, NULL, lpar, NULL, NULL, NULL);
  tmp->next = e;
  maloc(lpar, sizeof(int));
  *lpar = _right_parenthesis;
  expr_add(&tmp, 0, NULL, NULL, NULL, lpar, NULL, NULL, NULL);
  return tmp;
}

tlist* create_tlist() {
  tlist* t;
  maloc(t, sizeof(tlist));
  tlist* tmp = t;
  token tok = get_token();
  while (1) {
    tmp->t = tok;
    maloc(tmp->next, sizeof(tlist));
    tmp = tmp->next;
    tok = get_token();
    if (tok.type == _EOF) {
      tmp->t = tok;
      tmp->next = NULL;
      break;
    }
  }
  return create_floats(t);
}

tlist* create_floats(tlist* t) {
  if (t == NULL) return NULL;
  tlist* t_1 = t;
  tlist* t_2 = t_1->next;
  if (t_2 == NULL) return t;
  tlist* t_3 = t_2->next;
  if (t_3 == NULL) return t;
  string* tmp;
  maloc(tmp, sizeof(string));
  string_init(tmp);
  while (t_1 != NULL) {
    if (t_2 != NULL && t_2->t.type == _dot) {
      if (t_1->t.type == _number && t_3->t.type == _number) {
        string_append(tmp, t_1->t.str->txt);
        string_appendc(tmp, '.');
        string_append(tmp, t_3->t.str->txt);
        t_1->t.type = _decimalnumber;
        string_destroy(t_1->t.str);
        t_1->t.str = NULL;
        maloc(t_1->t.f_val, sizeof(double));
        *(t_1->t.f_val) = atof(tmp->txt);
        string_destroy(tmp);
        t_1->next = t_3->next;
        string_destroy(t_3->t.str);
        free(t_2);
        free(t_3);
      } else if (t_3 != NULL && t_3->t.type == _semicolon) {
        eprint("Lexical error\n");
        exit(LEXICAL_ERROR);
      }
    } else if (t_1->t.type == _number) {
      maloc(t_1->t.i_val, sizeof(int));
      *(t_1->t.i_val) = atoi(t_1->t.str->txt);
      string_destroy(t_1->t.str);
      t_1->t.str = NULL;
    }
    t_1 = t_1->next;
    if (t_1 != NULL) t_2 = t_1->next;
    if (t_1 != NULL && t_2 != NULL) t_3 = t_2->next;
  }
  return create_negative(t);
}
tlist* create_negative(tlist* t) {
  if (t == NULL) return NULL;
  tlist* t_1 = t;
  tlist* t_2 = t_1->next;
  if (t_2 == NULL) return t;
  tlist* t_3 = t_2->next;
  if (t_3 == NULL || t_3->t.type == _right_parenthesis ||
      t_3->t.type == _semicolon) {
    if (t_1->t.type == _minus && t_2->t.type == _decimalnumber) {
      *(t_2->t.f_val) *= -1;
      free(t_1->t.f_val);
      free(t_1);
      return t_2;
    }
    return t;
  }
  while (t_3 != NULL) {
    if (t_1->t.type >= _equals && t_1->t.type <= _not_typecheck) {
      if (t_2->t.type == _minus && t_3->t.type == _decimalnumber) {
        *(t_3->t.f_val) *= -1;
        t_1->next = t_3;
        free(t_2->t.f_val);
        free(t_2);
      }
    }
    t_1 = t_1->next;
    if (t_1 != NULL) t_2 = t_1->next;
    if (t_1 != NULL && t_2 != NULL) t_3 = t_2->next;
  }
  return process_exponent(t);
}

tlist* process_exponent(tlist* t) {
  if (t == NULL) return NULL;
  tlist* t_1 = t;
  tlist* t_2 = t_1->next;
  if (t_2 == NULL) return t;
  tlist* t_3 = t_2->next;
  if (t_3 == NULL) return t;
  while (t_3 != NULL) {
    if (t_2->t.type == _e) {
      if (t_1->t.type == _number) {
        if (t_3->t.type == _number) {
          maloc(t_1->t.f_val, sizeof(double));
          *(t_1->t.f_val) =
              pow((double)*(t_1->t.i_val), (double)*(t_3->t.i_val));
          free(t_1->t.i_val);
          t_1->t.i_val = NULL;
          t_1->next = t_3->next;
          free(t_2);
          t_2 = NULL;
          free(t_3->t.i_val);
          free(t_3);
          t_3 = NULL;
          t_1->t.type = _decimalnumber;
        }
      } else if (t_1->t.type == _decimalnumber) {
        if (t_3->t.type == _plus) {
          t_3 = t_3->next;
          if (t_3 != NULL && t_3->t.type != _number) break;
        }
        if (t_3->t.type == _number) {
          double* tmp;
          maloc(tmp, sizeof(double));
          *tmp = pow(*(t_1->t.f_val), (double)*(t_3->t.i_val));
          free(t_1->t.f_val);
          t_1->t.f_val = tmp;
          t_1->next = t_3->next;
          free(t_2);
          t_2 = NULL;
          free(t_3->t.i_val);
          free(t_3);
          t_3 = NULL;
        }
      }
    }
    t_1 = t_1->next;
    if (t_1 != NULL) t_2 = t_1->next;
    if (t_1 != NULL && t_2 != NULL) t_3 = t_2->next;
  }
  return t;
}

bool semantic_check(AST* a, function_table* f) {
  if (f == NULL && a != NULL) return true;
  if (a == NULL) return true;
  return false;
}
