#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#ifndef _token
#define _token

#define _EMPTY -1
#define _EOF 0
#define _SYMBOL 1
/*...*/
typedef struct token
{
    string str;
    int i_val;
    double f_val;
    int linenum;
    int type;
}token;

token get_token();
#endif
