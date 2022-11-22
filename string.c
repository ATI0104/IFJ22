#include "string.h"
// TODO check for memory leaks

/**
 * @brief Counts the number of characters in a string
 * 
 * @param txt  String to count characters in
 * @return int Number of characters in the string
 */
int strcnt(char* txt) {
  int i = 0;
  while (*txt != '\0') {
    i++;
    txt++;
  }
  return i;
}
/**
 * @brief Copies a string to another string
 * 
 * @param dest destination string
 * @param src source string
 */
void mystrcpy(char* dest, char* src) {
  if (src == NULL || dest == NULL) return;
  int i = 0;
  while (src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
}
/**
 * @brief Initializes the string structure
 * 
 * @param str String to initialize
 */
void string_init(string* str) {
  str->size = 0;
  str->txt = NULL;
}

/**
 * @brief Sets the string to a given value
 * 
 * @param str String to set
 * @param txt value to set the string to
 */
void string_set(string* str, char* txt) {
  if (str == NULL) return;
  str->size = strcnt(txt);
  if (str->txt != NULL) free(str->txt);
  maloc(str->txt, str->size + 1);
  mystrcpy(str->txt, txt);
}
/**
 * @brief Destroys the string structure
 * 
 * @param str String to destroy
 */
void string_destroy(string* str) {
  free(str->txt);
  str->txt = NULL;
  str->size = 0;
}
/**
 * @brief Append a string to another string
 * 
 * @param str String to append to
 * @param txt string to append
 */
void string_append(string* str, char* txt) {
  if (str == NULL || txt == NULL) return;
  if (str->txt == NULL) {
    string_set(str, txt);
    return;
  }
  int txtcnt = strcnt(txt);
  if (txtcnt == 0) return;
  str->txt = realloc(str->txt, str->size + 1 + txtcnt);
  while (*txt != '\0') {
    str->txt[str->size] = *txt++;
    str->size++;
  }
  str->txt[str->size] = '\0';
}
/**
 * @brief Append a character to a string
 * 
 * @param str String to append to
 * @param c Character to append
 */
void string_appendc(string* str, char c) {
  char a[2];
  a[0] = c;
  a[1] = '\0';
  string_append(str, a);
}
/**
 * @brief Checks if a string is empty
 * 
 * @param str string to check
 * @return true the string is empty
 * @return false the string is not empty
 */
bool is_empty(string* str) {
  if (str == NULL) return true;
  if (str->size == 0) return true;
  return false;
}
/**
 * @brief Delete the last character of a string
 * 
 * @param str 
 */
void string_deletelast(string* str) {
  if (str == NULL) return;
  if (str->txt == NULL) return;
  if (str->size >= 1) {
    str->size--;
    str->txt[str->size] = '\0';
    char* tmp = str->txt;
    str->txt = NULL;
    maloc(str->txt, str->size + 1);
    strcpy(str->txt, tmp);
    free(tmp);
    return;
  }
  string_destroy(str);
}
/**
 * @brief Delete the first character of a string
 * 
 * @param str 
 */
void string_deletefirst(string* str) {
  if (str == NULL) return;
  if (str->txt == NULL) return;
  if (str->size >= 1) {
    str->size--;
    string* tmp;
    maloc(tmp, sizeof(string));
    string_init(tmp);
    string_append(tmp, str->txt + 1);
    char* swap = str->txt;
    str->txt = tmp->txt;
    tmp->txt = swap;
    string_destroy(tmp);
    free(tmp);
  }
  string_destroy(str);
}