/* ush-env.h */
/* header file for ush-env.c, environment definitions */

#ifndef _USH_ENV_H
#define _USH_ENV_H

#include "ush.h"

/* environment processing functions */
BOOLEAN assign(char **p, char *s);	/* initialize name or value */
BOOLEAN EVexport(char *name);	/* set variable to be exported */
struct varslot *find(char *name);	/* find symbol table entry */
BOOLEAN EVset(char *name, char *val);	/* add name & valude to
					   enviromnemt */
BOOLEAN EVinit();		/* initialize symbol table from
				   environment */
char *EVget(char *name);	/* get value of variable */
BOOLEAN EVunset(char *name);
void EVprint();			/* printf environment */
void unset(int argc, char *argv[]);	/* assignment command */
void set(int argc, char *argv[]);	/* set command */
void export(int argc, char *argv[]);	/* export command */
void set_shell_env();

#endif
