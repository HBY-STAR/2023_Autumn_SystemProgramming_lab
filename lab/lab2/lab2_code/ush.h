/* ush.h */
/* header file for ush with general includes and definitions */

#ifndef _USH_H
#define _USH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef MAXSIG
#define MAXSIG 31
#endif

#define TRUE 1
#define FALSE 0
#define lowbyte(w) ((w) & 0377)
#define highbyte(w) lowbyte((w) >> 8)

typedef short BOOLEAN;

#define MAXVAR 50

extern char **environ;

#endif
