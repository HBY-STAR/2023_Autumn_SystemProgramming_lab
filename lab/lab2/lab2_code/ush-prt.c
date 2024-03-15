/* ush-prt.c */
/* error and status print functions for ush */

#include "ush.h"
#include "ush-prt.h"

void fatal(char *msg)
{				/* print wrror message and terminate */
    fprintf(stderr, "ERROR:%s\n", msg);
    exit(1);
}

void syserr(char *msg)
{				/* print system callerror message and
				   terminate */
    //extern int sys_nerr;

    fprintf(stderr, "ERROR: %s (%d", msg, errno);
    //if (errno > 0 && errno < sys_nerr)
      /* chnaged to from sys_errlist[errno] to */
      /* strerror(errno) for POSIX compliance */
      fprintf(stderr, ";%s)\n", strerror(errno));
    // else
	// fprintf(stderr, ")\n");
    // exit(1);
}


void statusprt(int pid, int status)
{				/* interrupt status code */
    int code;
    static char *sigmsg[] = {
	"",
	"Hangup",
	"Interrupt",
	"Quit",
	"Illegal instruction",
	"Trace trap",
	"IOT instruction",
	"EMT instruction",
	"Floating point exception",
	"Kill",
	"Bus error",
	"Segmentation violation",
	"Bad arg to system call",
	"Write on pipe",
	"Alarm clock",
	"Terminate signal",
	"User signal 1",
	"User signal 2",
	"Death of child",
	"Power fail",
    };

    if (status != 0 && pid != 0)
	printf("Process %d :", pid);
    if (lowbyte(status) == 0) {
	if ((code = highbyte(status)) != 0)
	    printf("Exit code %d\n", code);
    }
    else {
	if ((code = status & 00177) <= MAXSIG)
	    printf("%s", sigmsg[code]);
	else
	    printf("Signal #%d", code);
	if ((status & 0200) == 0200)
	    printf("-core dumped");
	printf("\n");
    }
}
