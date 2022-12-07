#include "generator.h"

int count = 0;
void generate(AST* a, function_table* f) {
  printf(".IFJcode22\n");
  printf("DEFVAR GF@-_-1st\n");
  printf("DEFVAR GF@-_-2nd\n");
  printf("DEFVAR GF@-_-3rd\n");
  printf("DEFVAR GF@-_-1stt\n");
  printf("DEFVAR GF@-_-2ndt\n");
  printf("jmp -_-skip");
  generate_exit_codes();
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
      generate_call(c->jmp, f, name, v, NULL);
    }
    c = c->next;
  }
}

void generate_if(code* ifpart, code* elsepart, expr* e, function_table* f,
                 string* functionname, varlist* v) {
  string countstr;
  string_set(&countstr, "");
  string_inttostr(&countstr, count++);
  string labelend;
  string_set(&labelend, "");
  if (functionname != NULL) string_set(&labelend, functionname->txt);
  string_append(&labelend, "_end");
  string_append(&labelend, countstr.txt);
  string label;
  string_set(&label, "");
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
  string_set(&countstr, "");
  string_inttostr(&countstr, count++);
  string labelwhile;
  string_set(&labelwhile, "");
  if (functionname != NULL) string_set(&labelwhile, functionname->txt);
  string_append(&labelwhile, "_while");
  string_append(&labelwhile, countstr.txt);
  string exprtype;
  string_set(&exprtype, "");
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
          printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr);
          printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n", countstr);
          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr);
          printf("JMP -_-exit7\n");

          printf("LABEL firstisint%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMPIFEQ firsttofloat%s GF@-_-2ndt string@float\n", countstr);
          printf("JUMPIFEQ first%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMP -_-exit7\n");
          printf("LABEL firstisfloat%s\n", countstr);

          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@float\n", countstr);
          printf("JUMPIFEQ secondtofloat%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMPIFEQ secondisnil%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMP -_-exit7\n");

          printf("LABEL firstisnil%s\n", countstr);
          printf("JUMPIFEQ zero%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMPIFEQ second%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMPIFEQ second%s GF@-_-2ndt string@float\n", countstr);
          printf("JUMP -_-exit7\n");

          printf("LABEL firsttofloat%s\n", countstr);
          printf("INT2FLOAT GF@-_-1st GF@-_-1st\n");
          printf("JUMP calculate%s\n", countstr);

          printf("LABEL secondtofloat%s\n", countstr);
          printf("INT2FLOAT GF@-_-2nd GF@-_-2nd\n");
          printf("JUMP calculate%s\n", countstr);

          printf("LABEL secondisnil%s\n", countstr);
          printf("JUMP -_-first%s\n", countstr);

          printf("LABEL calculate%s\n", countstr);
          if (*e->op == _plus) printf("ADD GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          if (*e->op == _minus) printf("SUB GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          if (*e->op == _multiply)
            printf("MUL GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          if (*e->op == _divide) printf("DIV GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL second%s", countstr);
          printf("PUSHS GF@-_-2nd\n");
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL first%s\n", countstr);
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL -_-finish%s\n", countstr);
          counter -= 1;
          break;
        case _dot:  // sn
          printf("POPS GF@-_-2nd\n");
          printf("POPS GF@-_-1st\n");
          printf("TYPE GF@-_-1stt GF@-_-1st\n");
          printf("TYPE GF@-_-2ndt GF@-_-2nd\n");

          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr);
          printf("JUMPIFEQ firstisstring%s GF@-_-2ndt string@string\n",
                 countstr);
          printf("JUMP -_-exit7\n");

          printf("LABEL firstisnil%s\n", countstr);
          printf("JUMPIFEQ empty%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMPIFEQ second%s GF@-_-2ndt string@string\n", countstr);
          printf("JUMP -_-exit7\n");

          printf("LABEL firstisstring%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@string\n", countstr);
          printf("JUMPIFEQ first%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMP -_-exit7\n");

          printf("LABEL calculate%s\n", countstr);
          printf("CONCAT GF@-_-1st GF@-_-1st GF@-_-2nd\n");
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL second%s", countstr);
          printf("PUSHS GF@-_-2nd\n");
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL first%s\n", countstr);
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL empty%s\n", countstr);
          printf("PUSHS string@\n");  // Might cause issues
          printf("JUMP -_-finish%s\n", countstr);

          printf("LABEL -_-finish%s\n", countstr);
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
          printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr);
          printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n", countstr);
          printf("JUMPIFEQ firstisstring%s GF@-_-1stt string@string\n",
                 countstr);
          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisint%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMPIFEQ secondisintzero%s GF@-_-2ndt string@nil\n",
                 countstr);
          printf("JUMPIFEQ firsttofloat%s GF@-_-2ndt string@float\n", countstr);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisfloat%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@float\n", countstr);
          printf("JUMPIFEQ secondisfloatzero%s GF@-_-2ndt string@nil\n",
                 countstr);
          printf("JUMPIFEQ secondtofloat%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisstring%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@string\n", countstr);
          printf("JUMPIFEQ secondisstringzero%s GF@-_-2ndt string@nil\n",
                 countstr);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code

          printf("label firstisnil%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMPIFEQ firstozeroint%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMPIFEQ firstozerofloat%s GF@-_-2ndt string@float\n",
                 countstr);
          printf("JUMPIFEQ firsttoemptystring%s GF@-_-2ndt string@string\n",
                 countstr);
          printf("label secondisintzero%s\n", countstr);
          printf("MOVE GF@-_-2nd int@0\n");
          printf("JUMP calculate%s\n", countstr);
          printf("label secondisfloatzero%s\n", countstr);
          printf("MOVE GF@-_-2nd float@%a\n", 0.0);
          printf("JUMP calculate%s\n", countstr);
          printf("label secondisstringzero%s\n", countstr);
          printf("MOVE GF@-_-2nd string@\n");
          printf("JUMP calculate%s\n", countstr);
          printf("label firstozeroint%s\n", countstr);
          printf("MOVE GF@-_-1st int@0\n");
          printf("JUMP calculate%s\n", countstr);
          printf("label firstozerofloat%s\n", countstr);
          printf("MOVE GF@-_-1st float@%a\n", 0.0);
          printf("JUMP calculate%s\n", countstr);
          printf("label firsttoemptystring%s\n", countstr);
          printf("MOVE GF@-_-1st string@\n");
          printf("JUMP calculate%s\n", countstr);
          printf("label firsttofloat%s\n", countstr);
          printf("INT2FLOAT GF@-_-1st GF@-_-1st\n");
          printf("JUMP calculate%s\n", countstr);
          printf("label secondtofloat%s\n", countstr);
          printf("INT2FLOAT GF@-_-2nd GF@-_-2nd\n");
          printf("JUMP calculate%s\n", countstr);
          printf("LABEL eq%s\n", countstr);
          printf("EQ GF@-_-3rd GF@-_-1st GF@-_-2nd");
          printf("JUMP finish%s\n", countstr);
          printf("label calculate%s\n", countstr);
          if (*e->op == _lessthan)
            printf("LT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
          else if (*e->op == _greaterthan)
            printf("GT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
          else if (*e->op == _lessthanoreq) {
            printf("LT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
            printf("JUMPIFEQ eq%s GF@-_-3rd bool@false\n", countstr);
          } else if (*e->op == _greaterthanoreq) {
            printf("GT GF@-_-3rd GF@-_-1st GF@-_-2nd\n");
            printf("JUMPIFEQ eq%s GF@-_-3rd bool@false\n", countstr);
          }
          printf("LABEL finish%s\n", countstr);
          printf("PUSHS GF@-_-3rd");
        case _typecheck:      // ifns
        case _not_typecheck:  // ifns
          printf("POPS GF@-_-2nd\n");
          printf("POPS GF@-_-1st\n");
          printf("TYPE GF@-_-1stt GF@-_-1st\n");
          printf("TYPE GF@-_-2ndt GF@-_-2nd\n");
          printf("JUMPIFEQ firstisint%s GF@-_-1stt string@int\n", countstr);
          printf("JUMPIFEQ firstisfloat%s GF@-_-1stt string@float\n", countstr);
          printf("JUMPIFEQ firstisstring%s GF@-_-1stt string@string\n",
                 countstr);
          printf("JUMPIFEQ firstisnil%s GF@-_-1stt string@nil\n", countstr);
          printf("JUMPIFEQ firstisbool%s GF@-_-1stt string@bool\n", countstr);
          printf("JUMP -_-exit7\n");  // May not be the correct exit code
          printf("label firstisint%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@int\n", countstr);
          printf("JUMP false%s\n", countstr);
          printf("label firstisfloat%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@float\n", countstr);
          printf("JUMP false%s\n", countstr);
          printf("label firstisstring%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@string\n", countstr);
          printf("JUMP false%s\n", countstr);
          printf("label firstisnil%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@nil\n", countstr);
          printf("JUMP false%s\n", countstr);
          printf("label firstisbool%s\n", countstr);
          printf("JUMPIFEQ calculate%s GF@-_-2ndt string@bool\n", countstr);
          printf("JUMP false%s\n", countstr);
          printf("label calculate%s\n", countstr);
          printf("EQ GF@-_-1st GF@-_-1st GF@-_-2nd");
          printf("PUSHS GF@-_-1st\n");
          printf("JUMP finish%s\n", countstr);
          printf("label false%s\n", countstr);
          printf("PUSHS bool@false\n");
          printf("JUMP finish%s\n", countstr);
          printf("label finish%s\n", countstr);
          if (e->type == _not_typecheck) printf("NOTS\n");
          break;
      }
    } else if (e->func) {
      // generate_call(e->func, f, functionname, v); TODO
    }

    e = e->next;
  }
  if (counter > 0) {
    if (tovar != NULL) printf("POPS LF@%s\n", tovar->txt);
    return;
  } else {
    exit(6);
  }
}

void generate_call(call* c, function_table* f, string* functionname, varlist* v,
                   string* savevalue) {
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