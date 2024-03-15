/* ush-sig.c */
/* signal handling functions for ush */

#include "ush.h"
#include "ush-sig.h"
#include "ush-prt.h"

static void (*entryint) (), (*entryquit) ();

void ignoresig()
{				/* ignore interrupt and quit */
    static BOOLEAN first = TRUE;

    if (first) {
	first = FALSE;
	entryint = signal(SIGINT, SIG_IGN);
	entryquit = signal(SIGQUIT, SIG_IGN);
	if (entryint == SIG_ERR || entryquit == SIG_ERR)
	    syserr("signal");
    }

    else if (signal(SIGINT, SIG_IGN) == SIG_ERR ||
	     signal(SIGQUIT, SIG_IGN) == SIG_ERR)
	syserr("signal");
}


void entrysig()
{				/* restore interrupt and quit */
    if (signal(SIGINT, entryint) == SIG_ERR ||
	signal(SIGQUIT, entryquit) == SIG_ERR)
	syserr("signal");
}

void catchsigs()
{				/* catch signals */
    void cleanup();
    // static void setsig();

    setsig(SIGHUP, cleanup);
    setsig(SIGINT, cleanup);
    setsig(SIGQUIT, cleanup);
    setsig(SIGTERM, cleanup);
}

void setsig(sig, fcn)		/* set signal if defaulted */
     int sig;
     void (*fcn) ();
{
    if (signal(sig, SIG_IGN) == SIG_ERR)
	syserr("signal");
    else if (signal(sig, SIG_IGN) == SIG_IGN)
	return;
    else if (signal(sig, SIG_IGN) == SIG_DFL) {
	if (signal(sig, fcn) == SIG_ERR)
	    syserr("signal");
    }
    else
	fatal("signal already caught!");

}

void cleanup(int sig)
{				/* clean up and terminate */

    if (signal(sig, SIG_IGN) == SIG_ERR)
	syserr("signal");

    switch (sig) {
    case SIGHUP:
	fprintf(stderr, "Hangup.\n");
	break;
    case SIGINT:
	fprintf(stderr, "Interrupt.\n");
	break;
    case SIGQUIT:
	fprintf(stderr, "Quit.\n");
    }
    exit(1);
}
