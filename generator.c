#include "generator.h"

int count = 0;
void generate(AST* a, function_table* f) {
  printf(".IFJcode22\n");
  printf("DEFVAR GF@-_-1st\n");
  printf("DEFVAR GF@-_-2nd\n");
  printf("DEFVAR GF@-_-3rd\n");
  printf("DEFVAR GF@-_-1stt\n");
  printf("DEFVAR GF@-_-2ndt\n");
  printf("jmp -_-skip\n");
  generate_exit_codes();
  TypeConversion();
  printf("LABEL -_-skip\n");
  while (strcmp(a->funname->txt, "m@in") != 0) {
    a = a->next;
  }
  define_variables(function_table_get(&f, *a->funname));
  generate_code(a->Code, f, NULL, true, NULL);
  a = a->first;
  while (strcmp(a->funname->txt, "m@in") != 0) {
    define_variables(function_table_get(&f, *a->funname));
    generate_code(a->Code, f, a->funname, true, NULL);
    a = a->next;
  }
}
void generate_exit_codes() {
  printf("LABEL -_-exit3\n");
  printf("EXIT int@3\n");
  printf("LABEL -_-exit4\n");
  printf("EXIT int@4\n");
  printf("LABEL -_-exit5\n");
  printf("EXIT int@5\n");
  printf("LABEL -_-exit6\n");
  printf("EXIT int@6\n");
  printf("LABEL -_-exit7\n");
  printf("EXIT int@7\n");
  printf("LABEL -_-exit8\n");
  printf("EXIT int@8\n");
}
void define_variables(function_table* f) { rec_define_variables(f->variable); }
void rec_define_variables(var_table* v) {
  if (v == NULL) {
    return;
  }
  rec_define_variables(v->left_var);
  rec_define_variables(v->right_var);
  printf("DEFVAR LF@%s\n", v->name.txt);
}
void generate_code(code* c, function_table* f, string* name, bool newfunction,
                   varlist* localvars) {
  varlist* v;
  if (newfunction)
    v = NULL;
  else
    v = localvars;

  if (name != NULL) {
    printf("LABEL %s", name->txt);
    printf("DPRINT string@%s", name->txt);

  } else {
    printf("DPRINT string@main\n");
  }
  while (c != NULL) {
    if (c->i) {
      generate_if(c->i, c->e, c->expression, f, name, v);
    } else if (c->loop) {
      generate_while(c->loop, c->expression, f, name, v);
    } else if (c->var) {
      generate_assignment(c->var, c->expression, f, name, &v);
    } else if (c->ret) {
      generate_return(c->expression, f, name, v);
    } else if (c->expression) {
      evaluate_expression(c->expression, f, name, v, NULL);
    } else if (c->jmp) {
      generate_call(c->jmp, f, NULL);
    }
    c = c->next;
  }
}

void generate_if(code* ifpart, code* elsepart, expr* e, function_table* f,
                 string* functionname, varlist* v) {
  string countstr;
  string_init(&countstr);
  string_inttostr(&countstr, count++);
  string labelend;
  string_init(&labelend);
  if (functionname != NULL) string_set(&labelend, functionname->txt);
  string_append(&labelend, "_end");
  string_append(&labelend, countstr.txt);
  string label;
  string_init(&label);
  if (functionname != NULL) string_set(&label, functionname->txt);
  string_append(&label, "_else");
  string_append(&label, countstr.txt);
  printf("DEFVAR LF@%s\n", countstr.txt);
  evaluate_expression(e, f, functionname, v, &countstr);
  printf("JUMPIFEQ %s LF@%s bool@false\n", label.txt, countstr.txt);
  generate_code(ifpart, f, functionname, false, v);
  printf("JUMP %s\n", labelend.txt);
  printf("LABEL %s\n", label.txt);
  generate_code(elsepart, f, functionname, false, v);
  printf("LABEL %s\n", labelend.txt);
  return;
}

void generate_while(code* loop, expr* e, function_table* f,
                    string* functionname, varlist* v) {
  string countstr;
  string_init(&countstr);
  string_inttostr(&countstr, count++);
  string labelwhile;
  string_init(&labelwhile);
  if (functionname != NULL) string_set(&labelwhile, functionname->txt);
  string_append(&labelwhile, "_while");
  string_append(&labelwhile, countstr.txt);
  string exprtype;
  string_init(&exprtype);
  string_set(&exprtype, "whileexpr");
  string_append(&exprtype, countstr.txt);
  printf("DEFVAR LF@%s", exprtype.txt);
  printf("LABEL %s", labelwhile.txt);
  evaluate_expression(e, f, functionname, v, &exprtype);
  printf("JUMPIFEQ %s LF@%s nil@nil", countstr.txt, exprtype.txt);
  generate_code(loop, f, functionname, false, v);
  printf("JUMP %s", labelwhile.txt);
  printf("LABEL %s", countstr.txt);
}

void generate_assignment(string* var, expr* e, function_table* f,
                         string* functionname, varlist** v) {
  evaluate_expression(e, f, functionname, *v, var);
}

void generate_return(expr* e, function_table* f, string* functionname,
                     varlist* v) {
  string countstr;
  string_set(&countstr, "");
  string_inttostr(&countstr, count++);
  printf("DEFVAR LF@%s", countstr.txt);
  if (functionname == NULL) {
    evaluate_expression(e, f, functionname, v, &countstr);
    printf("EXIT int@0");
    return;
  }

  evaluate_expression(e, f, functionname, v, &countstr);
  printf("PUSHS LF@%s", countstr.txt);
  printf("RETURN");
}
// TODO Improve this
void evaluate_expression(expr* e, function_table* f, string* functionname,
                         varlist* v, string* tovar) {
  int counter = 0;
  while (e != NULL) {
    if (e->num) {
      printf("PUSHS int@%d\n", *(e->num));
      counter++;
    } else if (e->str) {
      printf("PUSHS string@%s", e->str->txt);
      counter++;
    } else if (e->fl) {
      printf("PUSHS float@%a", *(e->fl));
      counter++;
    } else if (e->var) {
      int type = varlist_get(v, e->var);
      switch (type) {
        case _int:
          printf("PUSHS %s\n", e->var->txt);
          counter++;
          break;
        case _float:
          printf("PUSHS %s\n", e->var->txt);
          counter++;
          break;
        case _string:
          printf("PUSHS %s\n", e->var->txt);
          counter++;
          break;
        case _void:
          printf("PUSHS nil@nil\n");
          counter++;
          break;
        case _bool:
          printf("PUSHS %s\n", e->var->txt);
          counter++;
          break;
        default:
          break;
      }
    } else if (e->op) {  // TODO add all operators & verify that both of the
      // operands are of the same type if not convert them
      string countstr;
      string_init(&countstr);
      string_inttostr(&countstr, count++);
      switch (*(e->op)) {
        case _plus:      // ifn
        case _minus:     // ifn
        case _multiply:  // ifn
        case _divide:    // ifn
          printf("POPS GF@-_-2nd\n");
          printf("POPS GF@-_-1st\n");
          printf("TYPE GF@-_-1stt GF@-_-1st\n");
          printf("TYPE GF@-_-2ndt GF@-_-2nd\n");
          printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr.txt);
          printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr.txt);
          printf("JMP -_-exit7\n");

          printf("LABEL firstisint%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@int\n", countstr.txt);
          printf("JUMPIFEQ firsttofloat%s GF@-_-2ndt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ first%s GF@-_-2ndt string@nil\n", countstr.txt);
          printf("JUMP -_-exit7\n");
          printf("LABEL firstisfloat%s\n", countstr.txt);

          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ secondtofloat%s GF@-_-2ndt string@int\n",
                 countstr.txt);
          printf("JUMPIFEQ secondisnil%s GF@-_-2ndt string@nil\n",
                 countstr.txt);
          printf("JUMP -_-exit7\n");

          printf("LABEL firstisnil%s\n", countstr.txt);
          printf("JUMPIFEQ zero%s GF@-_-2ndt string@nil\n", countstr.txt);
          printf("JUMPIFEQ second%s GF@-_-2ndt string@int\n", countstr.txt);
          printf("JUMPIFEQ second%s GF@-_-2ndt string@float\n", countstr.txt);
          printf("JUMP -_-exit7\n");

          printf("LABEL firsttofloat%s\n", countstr.txt);
          printf("INT2FLOAT GF@-_-1st GF@-_-1st\n");
          printf("JUMP calculate%s\n", countstr.txt);

          printf("LABEL secondtofloat%s\n", countstr.txt);
          printf("INT2FLOAT GF@-_-2nd GF@-_-2nd\n");
          printf("JUMP calculate%s\n", countstr.txt);

          printf("LABEL secondisnil%s\n", countstr.txt);
          printf("JUMP -_-first%s\n", countstr.txt);

          printf("LABEL calculate%s\n", countstr.txt);
          if (*e->op == _plus) printf("ADD GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          if (*e->op == _minus) printf("SUB GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          if (*e->op == _multiply)
            printf("MUL GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          if (*e->op == _divide) printf("DIV GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL second%s", countstr.txt);
          printf("PUSHS GF@-_-2nd\n");
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL first%s\n", countstr.txt);
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL -_-finish%s\n", countstr.txt);
          counter -= 1;
          break;
        case _dot:  // sn
          printf("POPS GF@-_-2nd\n");
          printf("POPS GF@-_-1st\n");
          printf("TYPE GF@-_-1stt GF@-_-1st\n");
          printf("TYPE GF@-_-2ndt GF@-_-2nd\n");

          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr.txt);
          printf("JUMPIFEQ firstisstring%s GF@-_-2ndt string@string\n",
                 countstr.txt);
          printf("JUMP -_-exit7\n");

          printf("LABEL firstisnil%s\n", countstr.txt);
          printf("JUMPIFEQ empty%s GF@-_-2ndt string@nil\n", countstr.txt);
          printf("JUMPIFEQ second%s GF@-_-2ndt string@string\n", countstr.txt);
          printf("JUMP -_-exit7\n");

          printf("LABEL firstisstring%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@string\n",
                 countstr.txt);
          printf("JUMPIFEQ first%s GF@-_-2ndt string@nil\n", countstr.txt);
          printf("JUMP -_-exit7\n");

          printf("LABEL calculate%s\n", countstr.txt);
          printf("CONCAT GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL second%s", countstr.txt);
          printf("PUSHS GF@-_-2nd\n");
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL first%s\n", countstr.txt);
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL empty%s\n", countstr.txt);
          printf("PUSHS string@\n");  // Might cause issues
          printf("JUMP -_-finish%s\n", countstr.txt);

          printf("LABEL -_-finish%s\n", countstr.txt);
          counter -= 1;
          break;
        case _lessthan:         // ifns
        case _greaterthan:      // ifns
        case _lessthanoreq:     // ifns
        case _greaterthanoreq:  // ifns
          printf("POPS GF@-_-2nd\n");
          printf("POPS GF@-_-1st\n");
          printf("TYPE GF@-_-1stt GF@-_-1st\n");
          printf("TYPE GF@-_-2ndt GF@-_-2nd\n");
          printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr.txt);
          printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ firstisstring%s GF@-_-1stt string@string\n",
                 countstr.txt);
          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr.txt);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisint%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@int\n", countstr.txt);
          printf("JUMPIFEQ secondisintzero%s GF@-_-2ndt string@nil\n",
                 countstr.txt);
          printf("JUMPIFEQ firsttofloat%s GF@-_-2ndt string@float\n",
                 countstr.txt);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisfloat%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ secondisfloatzero%s GF@-_-2ndt string@nil\n",
                 countstr.txt);
          printf("JUMPIFEQ secondtofloat%s GF@-_-2ndt string@int\n",
                 countstr.txt);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisstring%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@string\n",
                 countstr.txt);
          printf("JUMPIFEQ secondisstringzero%s GF@-_-2ndt string@nil\n",
                 countstr.txt);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisnil%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@nil\n", countstr.txt);
          printf("JUMPIFEQ firstozeroint%s GF@-_-2ndt string@int\n",
                 countstr.txt);
          printf("JUMPIFEQ firstozerofloat%s GF@-_-2ndt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ firsttoemptystring%s GF@-_-2ndt string@string\n",
                 countstr.txt);
          printf("label secondisintzero%s\n", countstr.txt);
          printf("MOVE GF@-_-2nd int@0\n");
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label secondisfloatzero%s\n", countstr.txt);
          printf("MOVE GF@-_-2nd float@%a\n", 0.0);
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label secondisstringzero%s\n", countstr.txt);
          printf("MOVE GF@-_-2nd string@\n");
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label firstozeroint%s\n", countstr.txt);
          printf("MOVE GF@-_-1st int@0\n");
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label firstozerofloat%s\n", countstr.txt);
          printf("MOVE GF@-_-1st float@%a\n", 0.0);
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label firsttoemptystring%s\n", countstr.txt);
          printf("MOVE GF@-_-1st string@\n");
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label firsttofloat%s\n", countstr.txt);
          printf("INT2FLOAT GF@-_-1st GF@-_-1st\n");
          printf("JUMP calculate%s\n", countstr.txt);
          printf("label secondtofloat%s\n", countstr.txt);
          printf("INT2FLOAT GF@-_-2nd GF@-_-2nd\n");
          printf("JUMP calculate%s\n", countstr.txt);
          printf("LABEL eq%s\n", countstr.txt);
          printf("EQ GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
          printf("JUMP finish%s\n", countstr.txt);
          printf("label calculate%s\n", countstr.txt);
          if (*e->op == _lessthan)
            printf("LT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
          else if (*e->op == _greaterthan)
            printf("GT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
          else if (*e->op == _lessthanoreq) {
            printf("LT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
            printf("JUMPIFEQ eq%s GF@-_-3rd bool@false\n", countstr.txt);
          } else if (*e->op == _greaterthanoreq) {
            printf("GT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
            printf("JUMPIFEQ eq%s GF@-_-3rd bool@false\n", countstr.txt);
          }
          printf("LABEL finish%s\n", countstr.txt);
          printf("PUSHS GF@-_-3rd");
          break;
        case _typecheck:      // ifns
        case _not_typecheck:  // ifns
          printf("POPS GF@-_-2nd\n");
          printf("POPS GF@-_-1st\n");
          printf("TYPE GF@-_-1stt GF@-_-1st\n");
          printf("TYPE GF@-_-2ndt GF@-_-2nd\n");
          printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr.txt);
          printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n",
                 countstr.txt);
          printf("JUMPIFEQ firstisstring%s GF@-_-1stt string@string\n",
                 countstr.txt);
          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr.txt);
          printf("JUMPIFEQ firstisbool%s GF@-_-1stt string@bool\n",
                 countstr.txt);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code
          printf("label firstisint%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@int\n", countstr.txt);
          printf("JUMP false%s\n", countstr.txt);
          printf("label firstisfloat%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@float\n",
                 countstr.txt);
          printf("JUMP false%s\n", countstr.txt);
          printf("label firstisstring%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@string\n",
                 countstr.txt);
          printf("JUMP false%s\n", countstr.txt);
          printf("label firstisnil%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@nil\n", countstr.txt);
          printf("JUMP false%s\n", countstr.txt);
          printf("label firstisbool%s\n", countstr.txt);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@bool\n", countstr.txt);
          printf("JUMP false%s\n", countstr.txt);
          printf("label calculate%s\n", countstr.txt);
          printf("EQ GF@-_-1st GF@-_-1st GF@-_-2nd");
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP finish%s\n", countstr.txt);
          printf("label false%s\n", countstr.txt);
          printf("PUSHS bool@false\n");
          printf("JUMP finish%s\n", countstr.txt);
          printf("label finish%s\n", countstr.txt);
          if (e->type == _not_typecheck) printf("NOTS\n");
          break;
      }
    } else if (e->func) {
      generate_call(e->func, f, tovar);
      return;
    }

    e = e->next;
  }
  if (counter > 0) {
    if (tovar != NULL) printf("POPS LF@%s\n", tovar->txt);
    return;
  } else {
    return;
  }
}

void generate_call(call* c, function_table* f, string* savevalue) {
  if (f == NULL) {
    function_undefined(-1);
  }
  function_table* func = function_table_get(&f, *c->function_name);
  if (func == NULL) {
    function_undefined(-1);
  }
  if (func->built_in) {
    call_built_in_func(c, savevalue);
    return;
  }
  printf("CREATEFRAME\n");
  input* i = c->in;
  while (i != NULL) {
    if (i->i) {
      printf("PUSHS int@%d\n", *(i->i));
    } else if (i->f) {
      printf("PUSHS float@%a\n", *(i->f));
    } else if (i->s) {
      printf("PUSHS string@%s\n", i->s->txt);
    } else if (i->null) {
      printf("PUSHS nil@nil\n");
    } else if (i->var) {
      printf("PUSHS LF@%s\n", i->var->txt);
    }
    i = i->next;
  }
  printf("CALL %s\n", c->function_name->txt);
  printf("PUSHFRAME\n");
  if (savevalue != NULL) {
    printf("POPS LF@%s\n", savevalue->txt);
  }
}

void call_built_in_func(call* c, string* savevalue) {
  string countstr;
  string_init(&countstr);
  string_inttostr(&countstr, count++);
  if (strcmp(c->function_name->txt, "write") == 0) {
    input* i = c->in;
    while (i != NULL) {
      if (i->i) {
        printf("WRITE int@%d\n", *(i->i));
      } else if (i->f) {
        printf("WRITE float@%a\n", *(i->f));
      } else if (i->s) {
        printf("WRITE string@%s\n", i->s->txt);
      } else if (i->null) {
        printf("WRITE nil@nil\n");
      } else if (i->var) {
        printf("WRITE LF@%s\n", i->var->txt);
      }
      i = i->next;
    }
  } else if (strcmp(c->function_name->txt, "reads") == 0) {
    printf("READ LF@%s string\n", savevalue->txt);
  } else if (strcmp(c->function_name->txt, "readi") == 0) {
    printf("READ LF@%s int\n", savevalue->txt);
  } else if (strcmp(c->function_name->txt, "readf") == 0) {
    printf("READ LF@%s float\n", savevalue->txt);
  } else if (strcmp(c->function_name->txt, "floatval") == 0) {
    printf("TYPE GF@-_-1stt LF@%s\n", c->in->var->txt);
    printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr.txt);
    printf("JUMPIFEQ finish%s GF@-_-1stt string@float\n", countstr.txt);
    printf("JUMPIFEQ zero%s GF@-_-1stt string@nil\n", countstr.txt);
    printf("JUMP -_-exit4\n");
    printf("label firstisint%s\n", countstr.txt);
    if (savevalue)
      printf("INT2FLOAT LF@%s LF@%s\ns", savevalue->txt, c->in->var->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label zero%s\n", countstr.txt);
    if (savevalue) printf("MOVE LF@%s float@%a\n", savevalue->txt, 0.0);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label finish%s\n", countstr.txt);
  } else if (strcmp(c->function_name->txt, "intval") == 0) {
    printf("TYPE GF@-_-1stt LF@%s", c->in->var->txt);
    printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n", countstr.txt);
    printf("JUMPIFEQ finish%s GF@-_-1stt string@int\n", countstr.txt);
    printf("JUMPIFEQ zero%s GF@-_-1stt string@nil\n", countstr.txt);
    printf("JUMP -_-exit4\n");
    printf("label firstisfloat%s\n", countstr.txt);
    if (savevalue)
      printf("FLOAT2INT LF@%s LF@%s\n", savevalue->txt, c->in->var->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label zero%s\n", countstr.txt);
    if (savevalue) printf("MOVE LF@%s int@0\n", savevalue->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label finish%s\n", countstr.txt);
  } else if (strcmp(c->function_name->txt, "strval") == 0) {
    printf("TYPE GF@-_-1stt LF@%s", c->in->var->txt);
    printf("JUMPIFEQ calculate%s GF@-_-1stt string@string\n", countstr.txt);
    printf("JUMPIFEQ zero%s GF@-_-1stt string@nil\n", countstr.txt);
    printf("JUMP -_-exit4\n");
    printf("label zero%s\n", countstr.txt);
    if (savevalue) printf("MOVE LF@%s string@\n", savevalue->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label calculate%s\n", countstr.txt);
    if (savevalue)
      printf("MOVE LF@%s LF@%s\n", savevalue->txt, c->in->var->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label finish%s\n", countstr.txt);
  } else if (strcmp(c->function_name->txt, "strlen") == 0) {
    printf("TYPE GF@-_-1stt LF@%s", c->in->var->txt);
    printf("JUMPIFEQ zero%s GF@-_-1stt string@nil\n", countstr.txt);
    printf("STRLEN LF@%s LF@%s\n", savevalue->txt, c->in->var->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label zero%s\n", countstr.txt);
    if (savevalue) printf("MOVE LF@%s int@0\n", savevalue->txt);
    printf("JUMP finish%s\n", countstr.txt);
    printf("label finish%s\n", countstr.txt);
  } else if (strcmp(c->function_name->txt, "substring")) {
  }
}

void TypeConversion() {  //!!!TODO change LABELS
  printf("Label -_-1stShouldBeInt\n");
  printf("TYPE GF@-_-1stt GF@-_-1st\n");
  printf("JUMPIFEQ -_-1stIsInt_1  GF@-_-1stt string@int\n");
  printf("JUMPIFEQ -_-2ndShouldBeFloat_1 GF@-_-1stt string@float\n");
  printf("JUMPIFEQ -_-1stIsNil_1 GF@-_-1stt string@nil\n");
  printf("JUMP -_-exit4\n");

  printf("Label -_-1stIsInt_1\n");
  printf("RETURN\n");

  printf("Label -_-1stIsFloat_1\n");
  printf("FLOAT2INT GF@-_-1st GF@-_-1st\n");
  printf("RETURN\n");

  printf("Label -_-1stIsNil_1\n");
  printf("MOVE GF@-_-1st int@0\n");
  printf("RETURN\n");

  printf("LABEL -_-1stShouldBeFloat\n");
  printf("TYPE GF@-_-1stt GF@-_-1st\n");
  printf("JUMPIFEQ -_-1stIsFloat_2 GF@-_-1stt string@float\n");
  printf("JUMPIFEQ -_-1stIsInt_2 GF@-_-1stt string@int\n");
  printf("JUMPIFEQ -_-1stIsNil_2 GF@-_-1stt string@nil\n");
  printf("JUMP -_-exit4\n");
  printf("Label -_-1stIsFloat_2\n");
  printf("RETURN\n");

  printf("Label -_-1stIsInt_2\n");
  printf("INT2FLOAT GF@-_-1st GF@-_-1st\n");
  printf("RETURN\n");

  printf("Label -_-1stIsNil_2\n");
  printf("MOVE GF@-_-1st float@%a\n", 0.0);
  printf("RETURN\n");

  printf("LABEL -_-1stShouldBeString\n");
  printf("TYPE GF@-_-1stt GF@-_-1st\n");
  printf("JUMPIFEQ -_-1stIsString_1 GF@-_-1stt string@string\n");
  printf("JUMPIFEQ -_-1stIsNil_3 GF@-_-1stt string@nil\n");
  printf("JUMP -_-exit4\n");

  printf("Label -_-1stIsString_1\n");
  printf("RETURN\n");

  printf("Label -_-1stIsNil_3\n");
  printf("MOVE GF@-_-1st string@\n");
  printf("RETURN\n");

  printf("LABEL -_-2ndShouldBeFloat_1\n");
  printf("TYPE GF@-_-2ndt GF@-_-2nd\n");
  printf("JUMPIFEQ -_-2ndIsFloat GF@-_-2ndt string@float\n");
  printf("JUMPIFEQ -_-2ndIsInt GF@-_-2ndt string@int\n");
  printf("JUMPIFEQ -_-2ndIsNil GF@-_-2ndt string@nil\n");
  printf("JUMP -_-exit4\n");
  printf("Label -_-2ndIsFloat\n");
  printf("RETURN\n");
  printf("Label -_-2ndIsInt\n");
  printf("INT2FLOAT GF@-_-2nd GF@-_-2nd\n");
  printf("RETURN\n");
  printf("Label -_-2ndIsNil\n");
  printf("MOVE GF@-_-2nd float@%a\n", 0.0);
  printf("RETURN\n");

  printf("LABEL -_-2ndShouldBeFirst\n");
  printf("TYPE GF@-_-1stt GF@-_-1st\n");
  printf("JUMPIFEQ -_-1stIsNil_4 GF@-_-1stt string@nil\n");
  printf("JUMPIFEQ -_-1stIsInt_3 GF@-_-1stt string@int\n");
  printf("JUMPIFEQ -_-1stIsFloat_3 GF@-_-1stt string@float\n");
  printf("JUMPIFEQ -_-1stIsString_2 GF@-_-1stt string@string\n");
  printf("JUMP -_-exit4\n");

  printf("Label -_-1stIsNil_4\n");
  printf("TYPE GF@3rd GF@3rd\n");
  printf("JUMPIFEQ -_-1stand2ndshouldbeInt GF@3rd string@int\n");
  printf("JUMPIFEQ -_-1stand2ndshouldbeFloat_2 GF@3rd string@float\n");
  printf("JUMPIFEQ -_-1stand2ndshouldbeString GF@3rd string@string\n");
  printf("JUMP -_-exit4\n");

  printf("Label -_-1stand2ndshouldbeInt\n");
  printf("MOVE GF@-_-1st int@0\n");
  printf("MOVE GF@-_-2nd int@0\n");
  printf("RETURN\n");

  printf("Label -_-1stand2ndshouldbeFloat_2\n");
  printf("MOVE GF@-_-1st float@%a\n", 0.0);
  printf("MOVE GF@-_-2nd float@%a\n", 0.0);
  printf("RETURN\n");

  printf("Label -_-1stand2ndshouldbeString\n");
  printf("MOVE GF@-_-1st string@\n");
  printf("MOVE GF@-_-2nd string@\n");
  printf("RETURN\n");

  printf("Label -_-1stIsInt_3\n");
  printf("MOVE GF@-_-2nd int@0\n");
  printf("RETURN\n");

  printf("Label -_-1stIsFloat_3\n");
  printf("MOVE GF@-_-2nd float@%a\n", 0.0);
  printf("RETURN\n");

  printf("Label -_-1stIsString_2\n");
  printf("MOVE GF@-_-2nd string@\n");
  printf("RETURN\n");

}
