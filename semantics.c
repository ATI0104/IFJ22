#include "semantics.h"

bool Check_AST(AST* a, function_table* f) {
  while (a != NULL) {
    {
      function_table* thisfunction = function_table_get(&f, *(a->funname));
      if (Check_code(a->Code, f, thisfunction->variable, thisfunction->name)) {
        a = a->next;
      } else
        return false;
    }
  }
  return true;
}
bool Check_code(code* c, function_table* f, var_table* localVars,
                string funname) {
  varlist* v = NULL;
  while (c != NULL) {
    if (c->i) {
      Check_expression(c->expression, f, v);
      if (Check_code(c->i, f, localVars, funname)) {
        if (Check_code(c->e, f, localVars, funname)) {
          c = c->next;
          continue;
        } else
          return false;
      }
      return false;
    } else if (c->loop) {
      Check_expression(c->expression, f, v);
      if (Check_code(c->loop, f, localVars, funname)) {
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
  } else {
    // Check Expression
    int type = get_expression_type(ret, v, f);
    if (type == thisfunction->output_type) {
      return true;
    } else if (type != 0 && thisfunction->output_type != _void) {
      no_return(-1);
    }
  }
  no_return(-1);
}

bool call_check(call* c, function_table* f, varlist* v) {
  if (v == NULL) return false;
  if (c == NULL) return false;
  function_table* calledfunc = function_table_get(&f, *(c->function_name));
  if (calledfunc == NULL) function_undefined(-1);
  input* i = c->in;
  input_param_list* funparams = calledfunc->input_type;
  while (i != NULL) {
    if (funparams == NULL) {
      incorrect_number_of_arguments(-1);
    }
    switch (funparams->type) {
      case _int:
        if (i->i) {
          break;
        } else if (i->var) {
          if (varlist_get(&v, i->var) == _int) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        }
        incorrect_type_of_argument(-1);
        break;
      case _float:
        if (i->f) {
          break;
        } else if (i->var) {
          if (varlist_get(&v, i->var) == _float) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        }
        incorrect_type_of_argument(-1);
        break;
      case _string:
        if (i->s) {
          break;
        } else if (i->var) {
          if (varlist_get(&v, i->var) == _string) {
            break;
          } else {
            incorrect_type_of_argument(-1);
          }
        }
        incorrect_type_of_argument(-1);
        break;
      case _void:
        if (i->null) {
          break;
        } else if (i->var) {
          if (varlist_get(&v, i->var) == _void) {
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
  while (e != NULL) {
    if (e->var) {
      if (varlist_get(&v, e->var) == 0) {
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
 * https://www.geeksforgeeks.org/evaluation-prefix-expressions/
 * @param e expression
 * @param v function's variable table where the expression is located
 * @param f function table
 * @return int expression type
 */
int get_expression_type(expr* e, varlist* v, function_table* f) {
  if (e == NULL) return -1;
  Stack* s;
  Stack_Init(&s);
  expr* tmp = expression_flip(e);
  int pmm[] = {_plus, _minus, _multiply};
  int in[] = {_int, _null};
  int ifn[] = {_int, _float, _null};
  int lglgtn[] = {_lessthan,        _greaterthan, _lessthanoreq,
                  _greaterthanoreq, _typecheck,   _not_typecheck};
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
      Stack_Push(&s, v->current_type);
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
      } else if (isin(*(tmp->op), lglgtn)) {
        if (first == _string && isin(second, ifn)) {
          type_mismatch(-1);
        } else if (second == _string && isin(first, ifn)) {
          type_mismatch(-1);
        } else {
          Stack_Push(&s, _bool);
        }
      } else if (*(tmp->op) == _dot) {
        if (isin(first, ifn) && isin(second, ifn)) {
          type_mismatch(-1);
        } else {
          Stack_Push(&s, _string);
        }
      }
    }
  }
  int result;
  Stack_Top(s, &result);
  e = expression_flip(e);
  return result;
}
