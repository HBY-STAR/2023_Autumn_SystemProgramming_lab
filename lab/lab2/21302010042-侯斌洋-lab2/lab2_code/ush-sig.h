/* ush-sig.h */
/* header file for ush-sig.c, signal handler definitions */

#ifndef _USH_SIG_H
#define _USH_SIG_H

#include "ush.h"

/* signal handling */
void ignoresig();		/* ignore interrupt and quit */
void entrysig();		/* restore interrupt and quit */
void catchsigs();		/* catch signals */
void setsig(int sig, void (*fcn) ());	/* set signal if defaulted */
void cleanup(int sig);		/* clean up and terminate */

#endif
