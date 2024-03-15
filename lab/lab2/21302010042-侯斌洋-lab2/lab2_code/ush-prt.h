/* ush-prt.h */
/* header file for ush-prt.c, error/print definitions */

#ifndef _USH_PRT_H
#define _USH_PRT_H

#include "ush.h"
#include "errno.h"

/* error terminations and message printing */
void fatal(char *msg);		/* print wrror message and terminate */
void syserr(char *msg);		/* print system callerror message and
				   terminate */
void statusprt(int pid, int status);	/* interrupt status code */

#endif
