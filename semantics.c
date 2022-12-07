/**
  * Implementace překladače imperativního jazyka IFJ22
  * @author Attila Kovács (xkovac60)
  * @file semantics.c
*/
#include "semantics.h"

bool Check_AST(AST* a, function_table* f) {
  while (a != NULL) {
    {
      function_table* thisfunction = function_table_get(&f, *(a->funname));
      varlist* v = NULL;
      loadpredefvars(&v, thisfunction);
      if (Check_code(a->Code, f, thisfunction->variable, thisfunction->name,
                     v)) {
        a = a->next;
      } else
        return false;
    }
  }
  return true;
}
void loadpredefvars(varlist** v, function_table* thisfunction) {
  if (v == NULL) return;
  if (thisfunction == NULL) return;
  if (thisfunction->input_type == NULL) return;
  input_param_list* in = thisfunction->input_type;
  while (in != NULL) {
    varlist_set(v, &(in->name), in->type);
    in = in->next;
  }
}
bool Check_code(code* c, function_table* f, var_table* localVars,
                string funname, varlist* v) {
  while (c != NULL) {
    if (c->i) {
      Check_expression(c->expression, f, v);
      if (Check_code(c->i, f, localVars, funname, v)) {
        if (Check_code(c->e, f, localVars, funname, v)) {
          c = c->next;
          continue;
        } else
          return false;
      }
      return false;
    } else if (c->loop) {
      Check_expression(c->expression, f, v);
      if (Check_code(c->loop, f, localVars, funname, v)) {
        c = c->next;
        continue;
      }
      return false;
    } else if (c->ret) {
      Check_expression(c->expression, f, v);
      if (check_return_type(c->expression, f, funname, v)) {
        c = c->next;
        continue;
      }
    } else if (c->jmp) {
      if (call_check(c->jmp, f, v)) {
        c = c->next;
        continue;
      }
    } else if (c->var) {
      Check_expression(c->expression, f, v);
      if (var_table_get(&localVars, *(c->var)) == NULL) {
        undefined_variable(-1);
      }
      int type = get_expression_type(c->expression, v, f);
      varlist_set(&v, c->var, type);
      c = c->next;
      continue;
    } else if (c->expression) {
      Check_expression(c->expression, f, v);
      c = c->next;
      continue;
    }
    return false;
  }
  return true;
}

bool check_return_type(expr* ret, function_table* f, string funname,
                       varlist* v) {
  function_table* thisfunction = function_table_get(&f, funname);
  if (thisfunction->output_type == 0) {  // m@in
    return true;
  }
  int type = get_expression_type(ret, v, f);
  if (thisfunction->output_type == _void) {
    if (type == -1) {
      return true;
    }
    no_return(-1);
  } else if (thisfunction->output_type == type) {
    return true;
  } else if (thisfunction->questionmark) {
    if (type == -1) {
      return true;
    } else if (type == _null) {
      return true;
    }
  } else if (thisfunction->output_type != _void) {
    if (type == -1) no_return(-1);
  }
  incorrect_type_of_argument(-1);
}

bool Special_Function_Check(call* c, varlist* v) {
  string* s = c->function_name;
  if (strcmp(s->txt, "write") == 0) {
    return true;
  } else if (strcmp(s->txt, "floatval") == 0 || strcmp(s->txt, "intval") == 0) {
    if (c->in->next == NULL) {
      if (c->in->f || c->in->i || c->in->var || c->in->null) {
        if (c->in->var) {
          int type = varlist_get(v, c->in->var);
          if (type == _int || type == _float) {
            return true;
          } else {
            incorrect_type_of_argument(-1);
          }
        }
        return true;
      }
    } else {
      incorrect_number_of_arguments(-1);
    }
  } else if (strcmp(s->txt, "strval") == 0) {
    if (c->in->next == NULL) {
      if (c->in->null || c->in->s || c->in->var) {
        if (c->in->var) {
          int type = varlist_get(v, c->in->var);
          if (type == _string || type == _null) {
            return true;
          } else {
            incorrect_type_of_argument(-1);
          }
        }
        return true;
      }
    } else {
      incorrect_number_of_arguments(-1);
    }
  }
  incorrect_number_of_arguments(-1);
}
bool call_check(call* c, function_table* f, varlist* v) {
  if (c == NULL) return false;
  function_table* calledfunc = function_table_get(&f, *(c->function_name));
  if (calledfunc == NULL) function_undefined(-1);
  input* i = c->in;
  input_param_list* funparams = calledfunc->input_type;
  while (i != NULL) {
    if (funparams == NULL) {
      return Special_Function_Check(c, v);
    }
    switch (funparams->type) {
      case _int:
        if (i->i) {
          break;
        } else if (i->var) {
          if (varlist_get(v, i->var) == _int) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        } else if (funparams->question_mark) {
          if (i->null) break;
        }
        incorrect_type_of_argument(-1);
        break;
      case _float:
        if (i->f) {
          break;
        } else if (i->var) {
          if (varlist_get(v, i->var) == _float) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        } else if (funparams->question_mark) {
          if (i->null) break;
        }
        incorrect_type_of_argument(-1);
        break;
      case _string:
        if (i->s) {
          break;
        } else if (i->var) {
          if (varlist_get(v, i->var) == _string) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        } else if (funparams->question_mark) {
          if (i->null) break;
        }
        incorrect_type_of_argument(-1);
        break;
      case _void:
        if (i->null) {
          break;
        } else if (i->var) {
          if (varlist_get(v, i->var) == _void) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        }
        incorrect_type_of_argument(-1);
        break;
      default:
        eprint("Something's wrong.\n");
        exit(99);
        break;
    }
    i = i->next;
    funparams = funparams->next;
  }
  if (funparams != NULL) {
    incorrect_number_of_arguments(-1);
  }
  return true;
}

void Check_expression(expr* e, function_table* f, varlist* v) {
  get_expression_type(e, v, f);
  while (e != NULL) {
    if (e->var) {
      if (varlist_get(v, e->var) == 0) {
        undefined_variable(-1);
      }
    } else if (e->func) {
      call_check(e->func, f, v);
    }
    e = e->next;
  }
}
/**
 * @brief Get the resulting type of an expression (should be called after
 * converting the expression to prefix)
 * Inspiration taken from:
 * https://www.geeksforgeeks.org/stack-set-4-evaluation-postfix-expression/
 * @param e expression
 * @param v function's variable table where the expression is located
 * @param f function table
 * @return int expression type
 */
int get_expression_type(expr* e, varlist* v, function_table* f) {
  if (e == NULL) return -1;
  Stack* s;
  Stack_Init(&s);
  expr* tmp = e;
  int pmm[] = {_plus, _minus, _multiply, 0};
  int in[] = {_int, _null, 0};
  int ifn[] = {_int, _float, _null, 0};
  int infl[] = {_int, _float, 0};
  int lglg[] = {_lessthan, _greaterthan, _lessthanoreq, _greaterthanoreq, 0};
  int tnt[] = {_typecheck, _not_typecheck, 0};
  int sn[] = {_string, _null, 0};
  while (tmp != NULL) {
    if (tmp->num) {
      Stack_Push(&s, _int);
    } else if (tmp->fl) {
      Stack_Push(&s, _float);
    } else if (tmp->str) {
      Stack_Push(&s, _string);
    } else if (tmp->var) {
      int var = varlist_get(v, tmp->var);
      if (var == 0) {
        undefined_variable(-1);
      }
      Stack_Push(&s, var);
    } else if (tmp->func) {
      function_table* func =
          function_table_get(&f, *(tmp->func->function_name));
      if (func == NULL) {
        function_undefined(-1);
      }
      Stack_Push(&s, func->output_type);
    } else if (tmp->typekeywords) {
      Stack_Push(&s, *(tmp->typekeywords));
    } else if (tmp->op) {
      int first;
      Stack_Top(s, &first);
      Stack_Pop(s);
      int second;
      Stack_Top(s, &second);
      Stack_Pop(s);
      if (isin(*(tmp->op), pmm)) {
        if (first == _float && isin(second, ifn)) {
          Stack_Push(&s, _float);
        } else if (first == _int && isin(second, in)) {
          Stack_Push(&s, _int);
        } else if (first == _null && isin(second, ifn)) {
          if (second != _null)
            Stack_Push(&s, second);
          else
            Stack_Push(&s, _int);
        } else if (first == _int && second == _float) {
          Stack_Push(&s, _float);
        } else {
          type_mismatch(-1);
        }
      } else if (*(tmp->op) == _divide) {
        if (isin(first, ifn) && isin(second, ifn)) {
          Stack_Push(&s, _float);
        } else {
          type_mismatch(-1);
        }
      } else if (isin(*(tmp->op), lglg)) {
        if (first == _string && isin(second, infl)) {
          type_mismatch(-1);
        } else if (second == _string && isin(first, infl)) {
          type_mismatch(-1);
        } else {
          Stack_Push(&s, _bool);
        }
      } else if (*(tmp->op) == _dot) {
        if (isin(first, sn) && isin(second, sn)) {
          Stack_Push(&s, _string);
        } else {
          type_mismatch(-1);
        }
      } else if (isin(*(tmp->op), tnt)) {
        Stack_Push(&s, _bool);
      }
    }
    tmp = tmp->next;
  }
  int result;
  Stack_Top(s, &result);
  Stack_Destroy(&s);
  return result;
}
