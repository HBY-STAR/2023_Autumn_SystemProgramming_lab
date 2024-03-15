/* ush-parse.h */
/* header file for ush-parse.c, parser definitions */

#ifndef _USH_PARSE_H
#define _USH_PARSE_H

#include "ush.h"

typedef enum
{ T_WORD, T_BAR, T_AMP, T_SEMI, T_GT, T_GTGT, T_LT,
    T_NL, T_EOF
}
TOKEN;

/* scanner and parser */
TOKEN gettoken(char *word);	/* get next token */

#endif
