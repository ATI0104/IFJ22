#include "parser.h"

void AST_init(AST** a) {
  if (a == NULL) return;
  *a = NULL;
}
void AST_add(AST** a, string* f_name, code* code) {
  if (a == NULL) return;
  AST* tmp = *a;
  if (*a == NULL) {
    maloc(*a, sizeof(AST));
    tmp = *a;
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    maloc(tmp->next, sizeof(AST));
    tmp = tmp->next;
  }
  tmp->funname = f_name;
  tmp->code = code;
  tmp->next = NULL;
  tmp->first = *a;
}
void AST_destroy(AST** a) {
  if (a == NULL) return;
  AST* tmp = *a;
  if (tmp == NULL) return;
  AST* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    string_destroy(tmp->funname);
    code_destroy(&(tmp->code));
    free(tmp);
    *a = NULL;
    return;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  string_destroy(tmp_n->funname);
  code_destroy(&(tmp_n->code));
  free(tmp_n);
  tmp->next = NULL;
  AST_destroy(a);
}
void code_init(code** c) {
  if (c == NULL) return;
  *c = NULL;
}
void code_add(code** c, int lnum, code* i, code* e, code* loop, expr* exp,
              call* jmp) {
  if (c == NULL) return;
  code* tmp = *c;
  if (*c == NULL) {
    maloc(*c, sizeof(code));
    tmp = *c;
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    maloc(tmp->next, sizeof(code));
    tmp = tmp->next;
  }
  tmp->line_num = lnum;
  tmp->i = i;
  tmp->e = e;
  tmp->loop = loop;
  tmp->expression = exp;
  tmp->jmp = jmp;
  tmp->first = *c;
  tmp->next = NULL;
}

void code_destroy(code** c) {
  if (c == NULL) return;
  code* tmp = *c;
  if (tmp == NULL) return;
  code* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    code_destroy(&(tmp->i));
    code_destroy(&(tmp->e));
    code_destroy(&(tmp->loop));
    expr_destroy(&(tmp->expression), true);
    call_destroy(&(tmp->jmp));
    free(tmp);
    *c = NULL;
    return;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  code_destroy(&(tmp_n->i));
  code_destroy(&(tmp_n->e));
  code_destroy(&(tmp_n->loop));
  expr_destroy(&(tmp_n->expression), true);
  call_destroy(&(tmp_n->jmp));
  free(tmp_n);
  tmp->next = NULL;
  code_destroy(c);
}
void expr_init(expr** e) {
  if (e == NULL) return;
  *e = NULL;
}
void expr_add(expr** e, int type, string* str, int* num, double* fl, int* op,
              string* var, call* func) {
  if (e == NULL) return;
  expr* tmp = *e;
  if (*e == NULL) {
    maloc(*e, sizeof(expr));
    tmp = *e;
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    maloc(tmp->next, sizeof(expr));
    tmp = tmp->next;
  }
  tmp->type = type;
  tmp->str = str;
  tmp->num = num;
  tmp->fl = fl;
  tmp->op = op;
  tmp->var = var;
  tmp->func = func;
  tmp->first = *e;
  tmp->next = NULL;
}

void expr_destroy(expr** e, bool rec) {
  if (e == NULL) return;
  expr* tmp = *e;
  if (tmp == NULL) return;
  expr* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    if (rec) {
      string_destroy(tmp->str);
      string_destroy(tmp->var);
      if (tmp->num != NULL) free(tmp->num);
      if (tmp->fl != NULL) free(tmp->fl);
      if (tmp->op != NULL) free(tmp->op);
      call_destroy(&(tmp->func));
    }
    free(tmp);
    *e = NULL;
    return;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  string_destroy(tmp_n->str);
  string_destroy(tmp_n->var);
  if (tmp_n->num != NULL) free(tmp_n->num);
  if (tmp_n->fl != NULL) free(tmp_n->fl);
  if (tmp_n->op != NULL) free(tmp_n->op);
  call_destroy(&(tmp_n->func));
  free(tmp_n);
  tmp->next = NULL;
  expr_destroy(e, rec);
}

void call_init(call** c) {
  if (c == NULL) return;
  *c = NULL;
}
void call_create(call** c, string* f_name, input* in, output* out) {
  if (c == NULL) return;
  call* tmp = *c;
  if (tmp == NULL) {
    maloc(*c, sizeof(call));
    tmp = *c;
  }
  tmp->function_name = f_name;
  tmp->in = in;
  tmp->out = out;
}
void call_destroy(call** c) {
  if (c == NULL) return;
  if (*c == NULL) return;
  call* tmp = *c;
  string_destroy(tmp->function_name);
  input_destroy(&(tmp->in));
  input_destroy(&(tmp->out));
  free(tmp);
  *c = NULL;
}
void input_init(input** in) {
  if (in == NULL) return;
  *in = NULL;
}
void input_add(input** in, int* i, double* f, string* s, string* var) {
  if (in == NULL) return;
  input* tmp = *in;
  if (*in == NULL) {
    maloc(*in, sizeof(input));
    tmp = *in;
  } else {
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    maloc(tmp->next, sizeof(input));
    tmp = tmp->next;
  }
  tmp->i = i;
  tmp->f = f;
  tmp->s = s;
  tmp->var = var;
  tmp->next = NULL;
  tmp->first = *in;
}
void input_destroy(input** in) {
  if (in == NULL) return;
  input* tmp = *in;
  if (tmp == NULL) return;
  input* tmp_n = tmp->next;
  if (tmp_n == NULL) {
    if (tmp->i != NULL) free(tmp->i);
    if (tmp->f != NULL) free(tmp->f);
    string_destroy(tmp->s);
    string_destroy(tmp->var);
    free(tmp);
    *in = NULL;
    return;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  if (tmp_n->i != NULL) free(tmp_n->i);
  if (tmp_n->f != NULL) free(tmp_n->f);
  string_destroy(tmp_n->s);
  string_destroy(tmp_n->var);
  free(tmp_n);
  tmp->next = NULL;
  input_destroy(in);
}
expr* expr_pop(expr** e) {
  if (e == NULL) return NULL;
  if (*e == NULL) return NULL;
  expr* tmp = *e;
  *e = (*e)->next;
  tmp->next = NULL;
  return tmp;
}
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
      case _idenftificator:
        // TODO load function input params
        expr_add(&tmp, 0, NULL, NULL, NULL, NULL, t->t.str, NULL);
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
    if (t_2 != NULL) t_3 = t_2->next;
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
    if (t_2 != NULL) t_3 = t_2->next;
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
          free(t_3->t.i_val);
          free(t_3);
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
          free(t_3->t.i_val);
          free(t_3);
        }
      }
    }
    t_1 = t_1->next;
    if (t_1 != NULL) t_2 = t_1->next;
    if (t_2 != NULL) t_3 = t_2->next;
  }
  return t;
}

void add_func(function_table **tree){
token current = get_token();
token next;
while(current.type != _EOF ){
  if(current.type == _function){ // _function found
  current=get_token();
  if(current.type == _identificator){ // _identificator found
    
    if(function_table_get(tree, *current.str) != NULL){
      eprint("Function table already contains this function.");
    }
    else
    {
    function_table *func_ft;
    maloc(func_ft, sizeof(function_table));
    string_set(&(*func_ft).name, current.str->txt); //name
    current=get_token();
    if(current.type == _left_parenthesis){
      current=get_token(); //type
      
      while(current.type == _int || current.type == _float || current.type == _string){
        next = get_token(); 
        if(next.type == _variable){ // var
          input_param_list *params = NULL;
          string s;
          string_set(&s, next.str->txt);
          params = insert_top(s, current.type, params);
          current = get_token();
          if(current.type == _comma){
            current = get_token();
            continue; //start again
          }else if(current.type == _right_parenthesis){ //end of input parameters
            func_ft->input_type = params;
            break;
          }else{
            eprint("Invalid input parameters");
          }
        }else{
          eprint("Invalid input parameters");
        }
      } //end while
    }else{
      eprint("Invalid input parameters");
    }
    current=get_token();
    if(current.type==_semicolon){
      if(current.type == _int || current.type == _float || current.type == _string || current.type == _void){
        func_ft->output_type=current.type;
      }else{
        eprint("Invalid return value.");
      }
    }else{
      eprint("Missing return value.");
    }

    function_table_add(tree, func_ft);
  }
  
  }
  }
 }
}
