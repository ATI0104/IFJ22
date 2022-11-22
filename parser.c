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

void expr_topostfix(expr** e) {
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
      expr_add(&tmp, a->type, a->str, a->num, a->fl, a->op, a->var, a->func);
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
        expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL);
      }
      if (top != _left_parenthesis) {
        eprint("Invalid Expression\n");
        exit(LEXICAL_ERROR);  // replace with the right exit code
      }
    } else if (a->op[0] == _equals) {
      while (!Stack_IsEmpty(op_stack)) {
        int k = 0;
        Stack_Top(op_stack, &k);
        Stack_Pop(op_stack);
        maloc(j, sizeof(int));
        *j = k;
        expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL);
      }
      break;
    } else {
      Stack_Top(op_stack, &top);
      while (!Stack_IsEmpty(op_stack) &&
             check_precedence(a->op) <= check_precedence(&top)) {
        maloc(j, sizeof(int));
        *j = top;
        expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL);
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
    expr_add(&tmp, 0, NULL, NULL, NULL, j, NULL, NULL);
  }
  a = *e;
  *e = tmp;
  expr_destroy(&a, false);
  return;
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

expr* read_expression(tlist* t, int* c) {
  if (t == NULL || c == NULL) return NULL;
  *c = 0;
  expr* tmp;
  expr_init(&tmp);
  int brackets = 0;
  while (t != NULL) {
    if (t == NULL) {
      eprint("Incorrect expression\n");
      exit(4);  // Add correct exit code
    }
    if ((t->t.type >= _plus && t->t.type <= _not_typecheck) ||
        t->t.type == _dot) {
      expr_add(&tmp, 0, NULL, NULL, NULL, &(t->t.type), NULL, NULL);
      t = t->next;
      (*c)++;
      continue;
    }
    if (t->t.type == _left_parenthesis) {
      expr_add(&tmp, 0, NULL, NULL, NULL, &(t->t.type), NULL, NULL);
      t = t->next;
      (*c)++;
      brackets++;
      continue;
    }
    if (t->t.type == _right_parenthesis) {
      expr_add(&tmp, 0, NULL, NULL, NULL, &(t->t.type), NULL, NULL);
      brackets--;
      t = t->next;
      (*c)++;
      continue;
    }
    if (t->t.type == _semicolon || t->t.type == _left_curly_bracket) {
      if (brackets == 0) {
        return tmp;
      }
      eprint("Syntax error\n");
      exit(4);  // Add correct exit code
    }
    switch (t->t.type) {
      case _number:
        expr_add(&tmp, 0, NULL, t->t.i_val, NULL, NULL, NULL, NULL);
        t = t->next;
        (*c)++;
        break;
      case _decimalnumber:
        expr_add(&tmp, 0, NULL, NULL, t->t.f_val, NULL, NULL, NULL);
        t = t->next;
        (*c)++;
        break;
      case _variable:
        expr_add(&tmp, 0, NULL, NULL, NULL, NULL, t->t.str, NULL);
        t = t->next;
        (*c)++;
        break;
      case _array:
        expr_add(&tmp, 0, t->t.str, NULL, NULL, NULL, NULL, NULL);
        t = t->next;
        (*c)++;
        break;
      case _identificator:;
        int p;
        call* cal = load_function_call(t, fav.f, fav.v, &p);
        expr_add(&tmp, 0, NULL, NULL, NULL, NULL, NULL, cal);
        while (p != 0) {
          t = t->next;
          (*c)++;
          p--;
        }
        t = t->next;
        (*c)++;
        break;
      default:
        eprint("Incorrect expression\n");
        exit(4);  // Add correct exit code
        break;
    }
  }
  return NULL;
}

expr* add_parenthesis(expr* e) {
  expr* tmp;
  expr_init(&tmp);
  int* lpar;
  maloc(lpar, sizeof(int));
  *lpar = _left_parenthesis;
  expr_add(&tmp, 0, NULL, NULL, NULL, lpar, NULL, NULL);
  tmp->next = e;
  maloc(lpar, sizeof(int));
  *lpar = _right_parenthesis;
  expr_add(&tmp, 0, NULL, NULL, NULL, lpar, NULL, NULL);
  return tmp;
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
      } else if ((t_1->t.type == _string || t_1->t.type == _variable) &&
                 (t_3->t.type == _string || t_3->t.type == _variable))
        ;
      else
        exit(1);
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
input_param_list* load_input_params(tlist* t, int* skip) {
  *skip = 0;
  input_param_list* l;
  maloc(l, sizeof(input_param_list));
  input_param_list* tmp;
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
          tmp->name = *(t->t.str);
        } else {
          eprint("Error: Expected variable name after int\n");
          exit(4);  // Replace with correct error code
        }
        break;
      case _float:;
        tmp->type = _float;
        t = t->next;
        *skip = *skip + 1;
        if (t->t.type == _variable) {
          tmp->name = *(t->t.str);
        } else {
          eprint("Error: Expected variable name after float\n");
          exit(4);  // Replace with correct error code
        }
        break;
      case _string:

        tmp->type = _string;
        t = t->next;
        *skip = *skip + 1;
        if (t->t.type == _variable) {
          tmp->name = *(t->t.str);
        } else {
          eprint("Error: Expected variable name after string\n");
          exit(4);  // Replace with correct error code
        }
        break;
      case _null:

        tmp->type = _null;
        break;
      default:
        eprint("Error: Expected type after comma\n");
        exit(4);  // Replace with correct error code
        break;
    }
    tmp = tmp->next;
    *skip = *skip + 1;
  }
  if (*skip == 0) {  // Satisfying the compiler
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
          if (t->t.type == _right_parenthesis) {
            t = t->next;
            if (t->t.type == _colon) {
              t = t->next;
              if (t->t.type == _question_mark)
                t = t->next;  // Should we deal
                              // with this?
              if (t->t.type == _int) {
                tmp->output_type = _int;
              }
              if (t->t.type == _float) {
                tmp->output_type = _float;
              }
              if (t->t.type == _string) {
                tmp->output_type = _string;
              }
              if (t->t.type == _null) {
                tmp->output_type = _null;
              }
            } else {
              tmp->output_type = _null;
            }
          }
        }
      }
      function_table_add(tree, tmp);
    }
    t = t->next;
  }
}
var_table* load_variables(tlist* t) {
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
      tlist* temp = t->next;
      if (temp->t.type == _equals) {
        maloc(leaf, sizeof(var_table));
        leaf->name = *(t->t.str);
        var_table_add(&root, leaf);
      } else {
        if (var_table_get(&root, *(t->t.str)) != NULL) {
          eprint("Variable %s is not declared\n", t->t.str->txt);
          exit(4);  // Replace with correct exit code
        }
      }
      t = t->next;
    }
  }
  return root;
}

// Returns the called function, the first item in tlist should be the function's
// name
call* load_function_call(tlist* t, function_table* f, var_table* v, int* skip) {
  if (t == NULL || skip == NULL) {
    return NULL;
  }
  *skip = 0;
  if (t->t.type == _identificator) {
    function_table* temp = function_table_get(&f, *(t->t.str));
    if (temp == NULL) {
      eprint("Function %s is not declared\n", t->t.str->txt);
      exit(4);  // Replace with correct exit code
    }
    t = t->next;
    *skip = *skip + 1;
    if (t->t.type != _left_parenthesis) {
      eprint("Expected left parenthesis after function name\n");
      exit(4);  // Replace with correct exit code
    }
    t = t->next;
    *skip = *skip + 1;
    call* c;
    maloc(c, sizeof(call));
    input* i;
    maloc(i, sizeof(input));
    input* tmp = i;
    while (t->t.type != _right_parenthesis) {
      if (t->t.type == _comma) {
        t = t->next;
        *skip = *skip + 1;
        continue;
      }
      switch (t->t.type) {
        case _number:
          tmp->i = t->t.i_val;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          t = t->next;
          *skip = *skip + 1;
          continue;
          break;
        case _decimalnumber:
          tmp->f = t->t.f_val;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          t = t->next;
          *skip = *skip + 1;
          continue;
          break;
        case _variable:
          tmp->var = t->t.str;
          if (var_table_get(&v, *(t->t.str)) == NULL) {
            eprint("Variable %s is not declared\n", t->t.str->txt);
            exit(4);  // Replace with correct exit code
          }
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          t = t->next;
          *skip = *skip + 1;
          continue;
          break;
        case _string:
          tmp->s = t->t.str;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          t = t->next;
          *skip = *skip + 1;
          continue;
          break;
        case _null:
          tmp->null = true;
          maloc(tmp->next, sizeof(input));
          tmp = tmp->next;
          t = t->next;
          *skip = *skip + 1;
          continue;
          break;
        default:
          eprint("Invalid input type\n");
          exit(4);  // Replace with correct exit code
          break;
      }
    }
    return c;
  }
  return NULL;
}