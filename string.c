#include "string.h"
// TODO check for memory leaks
int strcnt(char* txt) {
  int i = 0;
  while (*txt != '\0') {
    i++;
    txt++;
  }
  return i;
}
void mystrcpy(char* dest, char* src) {
  if (src == NULL || dest == NULL) return;
  int i = 0;
  while (src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
}
void string_init(string* str) {
  str->size = 0;
  str->txt = NULL;
}
void string_set(string* str, char* txt) {
  if (str == NULL) return;
  str->size = strcnt(txt);
  str->txt = malloc(str->size * sizeof(char));
  mystrcpy(str->txt, txt);
}
void string_destroy(string* str) {
  free(str->txt);
  str->txt = NULL;
  str->size = 0;
}

void string_append(string* str, char* txt) {
  if (str == NULL || txt == NULL) return;
  if (str->txt == NULL) {
    string_set(str, txt);
    return;
  }
  int txtcnt = strcnt(txt);
  str->txt = realloc(str->txt, str->size + txtcnt * sizeof(char));
  while (*txt != '\0') {
    str->txt[str->size] = *txt++;
    str->size++;
  }
  str->txt[str->size] = '\0';
}

void string_appendc(string* str, char c) {
  char a[2];
  a[0] = c;
  a[1] = '\0';
  string_append(str, a);
}

bool is_empty(string* str) {
  if (str == NULL) return true;
  if (str->size == 0) return true;
  return false;
}

char pop_first_char(string* str) {
  char a = *(str->txt);
  char* tmp = str->txt;
  str->size--;
  char* tmp2 = malloc(str->size * sizeof(char));
  tmp++;
  while (tmp != NULL) {
    *tmp2 = *tmp;
    tmp2++;
    tmp++;
  }
  free(str->txt);
  str->txt = tmp2;
}