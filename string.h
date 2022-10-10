#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifndef mydynamicstring
#define mydynamicstring
typedef struct string {
  char* txt;
  int size;
} string;
int strcnt(char* txt);
void string_init(string* str);
void string_set(string* str, char* txt);
void string_destroy(string* str);
void string_append(string* str, char *txt);
void string_appendc(string* str, char c);
bool is_empty(string *str);
#endif

