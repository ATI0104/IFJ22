#include "parser.h"
#include "scanner.h"
#include "semantics.h"
#include "string.h"
#include "symtable.h"
void printcode(code* c);
void expr_print(expr* e, bool postfix);
void print_vartable(var_table* v);
char* printexpression(expr* e);
void printAST(AST* a);
int main() {
  tlist* m = create_tlist();
  string a;
  string_init(&a);
  function_table* f = NULL;
  function_table_init(&f);
  add_func(&f, m);
  //if (check_syntax(m, f))
  if(true)
    eprint("Works!");
  else
    syntaxerror(-1);
  tlist* mainfunction;
  var_table* globalvariables;
  m = move_tokens(m, &mainfunction, &globalvariables);
  print_vartable(globalvariables);
  AST* ast = ConvertToAst(m, mainfunction, globalvariables);
  printAST(ast);
  if (ast == NULL) {
    eprint("AST is NULL");
    exit(Internal_Error);
  }
  if (Check_AST(ast, f))
    eprint("AST should be fine!");
  else{
    eprint("AST bad!");
    exit(Internal_Error);
  }
#ifdef _skip_prolog_check
  expr* e;
  int c = 0;
  e = read_expression(m, &c);
  expr_print(e, false);
  e = add_parenthesis(e);
  expr_topostfix(&e);
  expr_print(e, true);
#endif
  // Prints out basic informations about the generated tokens
  token tok;
  while (m != NULL) {
    tok = m->t;
    printf("Token from line %d\n has a type of %d.\n", *(tok.linenum),
           tok.type);
    if (tok.i_val != NULL) {
      printf("It's int value = %d\n", *tok.i_val);
    }
    if (tok.f_val != NULL) {
      printf("It's float value = %f\n", *tok.f_val);
    }
    if (tok.str != NULL) {
      printf("It's string value = \"%s\"\n", tok.str->txt);
    }
    m = m->next;
  }
  return 0;
}

void expr_print(expr* e, bool postfix) {
  if (postfix) printf("Postfix: ");
  while (e != NULL) {
    if (e->str != NULL) printf("%s ", e->str->txt);
    if (e->num != NULL) printf("%d ", *(e->num));
    if (e->fl != NULL) printf("%f ", *(e->fl));
    if (e->var != NULL) printf("%s ", e->var->txt);
    if (e->op != NULL) {
      switch (*(e->op)) {
        case _equals:
          printf("= ");
          break;
        case _plus:
          printf("+ ");
          break;
        case _minus:
          printf("- ");
          break;
        case _multiply:
          printf("* ");
          break;
        case _divide:
          printf("/ ");
          break;
        case _lessthan:
          printf("< ");
          break;
        case _greaterthan:
          printf("> ");
          break;
        case _lessthanoreq:
          printf("<= ");
          break;
        case _greaterthanoreq:
          printf(">= ");
          break;
        case _typecheck:
          printf("=== ");
          break;
        case _not_typecheck:
          printf("!== ");
          break;
        case _dot:
          printf(". ");
          break;
        case _left_parenthesis:
          printf("( ");
          break;
        case _right_parenthesis:
          printf(") ");
          break;
        default:
          printf("\"%d\"", *(e->op));
          break;
      }
    }
    e = e->next;
  }
  printf("\n");
}

void print_vartable(var_table* v) {
  if (v == NULL) return;
  printf("name: \"%s\"\n", v->name.txt);
  print_vartable(v->left_var);
  print_vartable(v->right_var);
}

void printAST(AST* a) {
  if (a == NULL) return;
  while (a != NULL) {
    printf("function: %s\n", a->funname->txt);
    printcode(a->Code);
    a = a->next;
  }
}

void printcode(code* c) {
  while (c != NULL) {
    if (c->i) {
      printf("if (%s){", printexpression(c->expression));
      printcode(c->i);
      printf("}else{\n");
      printcode(c->e);
      printf("}\n");
    } else if (c->loop) {
      printf("while (%s){", printexpression(c->expression));
      printcode(c->loop);
      printf("}\n");
    } else if (c->jmp) {
      printf("%s(", c->jmp->function_name->txt);
      input* in = c->jmp->in;
      while (in != NULL) {
        if (in->var) printf("%s,", in->var->txt);
        if (in->f) printf("%f,", *(in->f));
        if (in->i) printf("%d,", *(in->i));
        if (in->s) printf("%s,", in->s->txt);
        in = in->next;
      }
      printf(")\n");
    } else if (c->var) {
      printf("%s = %s\n", c->var->txt, printexpression(c->expression));
    }
    c = c->next;
  }
}

char* printexpression(expr* e) {
  while (e != NULL) {
    if (e->func) {
      printf("%s(", e->func->function_name->txt);
      input* in = e->func->in;
      while (in != NULL) {
        if (in->var) printf("%s,", in->var->txt);
        if (in->f) printf("%f,", *(in->f));
        if (in->i) printf("%d,", *(in->i));
        if (in->s) printf("%s,", in->s->txt);
        in = in->next;
      }
      printf(")");
    } else if (e->op) {
      printf("op");
    } else if (e->var) {
      printf("%s", e->var->txt);
    } else if (e->num) {
      printf("%d", *(e->num));
    } else if (e->fl) {
      printf("%f", *(e->fl));
    } else if (e->str) {
      printf("%s", e->str->txt);
    }
    e = e->next;
  }
  printf("\n");
  return "";
}