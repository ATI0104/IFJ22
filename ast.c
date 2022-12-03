#include "ast.h"
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
  tmp->Code = code;
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
    code_destroy(&(tmp->Code));
    free(tmp);
    *a = NULL;
    return;
  }
  while (tmp_n->next != NULL) {
    tmp_n = tmp_n->next;
    tmp = tmp->next;
  }
  string_destroy(tmp_n->funname);
  code_destroy(&(tmp_n->Code));
  free(tmp_n);
  tmp->next = NULL;
  AST_destroy(a);
}
void code_init(code** c) {
  if (c == NULL) return;
  *c = NULL;
}
void code_add(code** c, int lnum, code* i, code* e, code* loop, expr* exp,
              call* jmp,string* var, expr* ret) {
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
  tmp->var = var;
  tmp->ret = ret;
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
              string* var, call* func, int* keywords) {
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
  tmp->typekeywords = keywords;
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
      if (tmp->typekeywords != NULL) free(tmp->typekeywords);
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
bool call_check(call** c, input_param_list *param){
 int t;
  call* tmp = *c;
  if (tmp == NULL) {
    maloc(*c, sizeof(call));
    tmp = *c;
  }
  while(param!= NULL ){
    if(strcmp(param->name.txt, tmp->function_name->txt)==0){   //function is found
        while (param->next!=NULL) {
          t=varlist_get(v, tmp->in->var->txt);
          if (param->type == _int){ //_int
            if(tmp->in->i==NULL ||  t != _int) return false;
          }
          if (param->type == _float){ //_float
            if(tmp->in->f == NULL|| t != _float) return false;
          }
          if (param->type == _string){ //_string
            if(strcmp(tmp->in->s->txt,"")|| t != _string) return false;
          }
          if (param->type == _bool){ //_bool
            if(tmp->in->null == false || t != _bool) return false;
          } 
          param = param->next;
          tmp->in = tmp->in->next;
        }
        return true;
      }
  }
  return false;
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
