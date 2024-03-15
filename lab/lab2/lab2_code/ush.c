#include "ush.h"
#include "ush-env.h"
#include "ush-parse.h"
#include "ush-prt.h"
#include "ush-sig.h"
#include "errno.h"

#define MAXARG 20
#define MAXFNAME 500
#define MAXWORD 100
#define BADFD -2

int old_src = -1;
int old_dst = -1;

static void redirect(int srcfd, char *srcfile, int dstfd, char *dstfile,
					 BOOLEAN append, BOOLEAN bckgrnd);
static BOOLEAN builtin(int argc, char *argv[], int srcfd, int dstfd);

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// return the process id									     //
/////////////////////////////////////////////////////////
static int invoke(int argc, char *argv[], int srcfd, char *srcfile,
				  int dstfd, char *dstfile, BOOLEAN append,
				  BOOLEAN bckgrnd, BOOLEAN makepipe)
/* invoke simple command */
{
	set_shell_env();

	pid_t pid;

	//  null line
	if (argc == 0)
	{
		return 0;
	}

	if (argv[0][0] == 'c' && argv[0][1] == 'd' && argv[0][2] == '\0' ||
		argv[0][0] == 'e' && argv[0][1] == 'c' && argv[0][2] == 'h' && argv[0][3] == 'o' && argv[0][4] == '\0' ||
		argv[0][0] == 'e' && argv[0][1] == 'x' && argv[0][2] == 'i' && argv[0][3] == 't' && argv[0][4] == '\0' ||
		argv[0][0] == 'p' && argv[0][1] == 'w' && argv[0][2] == 'd' && argv[0][3] == '\0' ||
		argv[0][0] == 's' && argv[0][1] == 'e' && argv[0][2] == 't' && argv[0][3] == '\0' ||
		argv[0][0] == 'u' && argv[0][1] == 'n' && argv[0][2] == 's' && argv[0][3] == 'e' && argv[0][4] == 't' && argv[0][5] == '\0' ||
		argv[0][0] == 'e' && argv[0][1] == 'x' && argv[0][2] == 'p' && argv[0][3] == 'o' && argv[0][4] == 'r' && argv[0][5] == 't' && argv[0][6] == '\0')
	{
		// pipe
		if (srcfd != BADFD && dstfd != BADFD)
		{
			if (makepipe)
			{
				// printf("here\n");
				old_src = dup(0);
				dup2(srcfd, 0); // Redirect stdin to the read end
				close(srcfd);	// Close the read end

				old_dst = dup(1);
				// close(srcfd);// Close the read end
				dup2(dstfd, 1); // Redirect stdout to the write end
			}
		}
		else
		{
			redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
		}
		if (!builtin(argc, argv, srcfd, dstfd))
		{
			syserr("built in command error");
		}
		// redirect to stdio
		if (old_src != -1)
		{
			dup2(old_src, 0);
			close(old_src);
		}
		if (old_dst != -1)
		{
			dup2(old_dst, 1);
			close(old_dst);
		}
	}
	else
	{
		if ((pid = fork()) < 0)
		{
			syserr("fork error");
			exit(-1);
		}
		// child
		else if (pid == 0)
		{
			old_dst = -1;
			old_src = -1;
			// pipe
			if (srcfd != BADFD && dstfd != BADFD)
			{
				if (makepipe)
				{
					// printf("here\n");
					old_src = dup(0);
					dup2(srcfd, 0); // Redirect stdin to the read end
					close(srcfd);	// Close the read end

					old_dst = dup(1);
					// close(srcfd);// Close the read end
					dup2(dstfd, 1); // Redirect stdout to the write end
									// close(dstfd); // Close the write end
				}
			}
			else
			{
				redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
			}
			if (execvp(argv[0], argv) < 0)
			{
				syserr("exec error");
			}
			// redirect to stdio
			if (old_src != -1)
			{
				dup2(old_src, 0);
				close(old_src);
			}
			if (old_dst != -1)
			{
				dup2(old_dst, 1);
				close(old_dst);
			}
			exit(-1);
		}
		// parent
		else
		{
			if (!bckgrnd || makepipe)
			{
				wait(0);
			}
			return pid;
		}
	}
}

//////////////////////////////////////////////////////////
// You must implement the redirect function 					//
/////////////////////////////////////////////////////////
static void redirect(int srcfd, char *srcfile, int dstfd, char *dstfile,
					 BOOLEAN append, BOOLEAN bckgrnd)
{ /* I/O redirection */
	int rd_src_fd = 0;
	int rd_dst_fd = 1;
	old_src = -1;
	old_dst = -1;

	if (append)
	{
		if (srcfile != NULL && srcfd == BADFD)
		{
			rd_src_fd = open(srcfile, O_RDWR, 0777);
			old_src = dup(0);
			dup2(rd_src_fd, 0);
			close(rd_src_fd);
		}
		if (dstfile != NULL && dstfd == BADFD)
		{
			rd_dst_fd = open(dstfile, O_CREAT | O_RDWR | O_APPEND, 0777);
			old_dst = dup(1);
			dup2(rd_dst_fd, 1);
			close(rd_dst_fd);
		}
	}
	else
	{
		if (srcfile != NULL && srcfd == BADFD)
		{
			rd_src_fd = open(srcfile, O_RDWR, 0777);
			old_src = dup(0);
			dup2(rd_src_fd, 0);
			close(rd_src_fd);
		}
		if (dstfile != NULL && dstfd == BADFD)
		{
			rd_dst_fd = open(dstfile, O_CREAT | O_RDWR | O_TRUNC, 0777);
			old_dst = dup(1);
			dup2(rd_dst_fd, 1);
			close(rd_dst_fd);
		}
	}
}

static void waitfor(int pid)
{ /* wait for child */

	int wpid, status;

	while ((wpid = wait(&status)) != pid && wpid != -1)
		statusprt(wpid, status);
	if (wpid == pid)
		statusprt(0, status);
}
//////////////////////////////////////////////////////////
// You must implement the builtin function 	to do				//
// the builtin command. 									//
// Note :: the builtin command should not invoke the linux standard //
// function.eg, cd should not invoke chdir						//
// return true if a builtin command ,false other wise				//
//////////////////////////////////////////////////////////

static BOOLEAN builtin(int argc, char *argv[], int srcfd, int dstfd)
/* do built-in */
{
	if (argv[0][0] == 'c' && argv[0][1] == 'd')
	{
		// cd
		if (argc != 2)
		{
			return FALSE;
		}
		else
		{
			if (chdir(argv[1]) < 0)
			{
				syserr("cd error");
				return FALSE;
			}
			EVset("PWD", getcwd(NULL, 0));
			return TRUE;
		}
	}
	else if (argv[0][0] == 'e' && argv[0][1] == 'c' && argv[0][2] == 'h' && argv[0][3] == 'o')
	{
		// echo
		char buf[100];
		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] == '\'' && argv[i][strlen(argv[i]) - 1] == '\'' || argv[i][0] == '\"' && argv[i][strlen(argv[i]) - 1] == '\"')
			{
				if (write(1, &argv[i][1], strlen(argv[i]) - 2) != strlen(argv[i]) - 2)
				{
					syserr("write error");
				}
			}
			else if (argv[i][0] == '$')
			{
				char *val = EVget(&argv[i][1]);
				if (val == NULL)
				{
					syserr("val not found");
					return FALSE;
				}
				if (write(1, val, strlen(val)) != strlen(val))
				{
					syserr("write error");
				}
			}
			else
			{
				if (write(1, argv[i], strlen(argv[i])) != strlen(argv[i]))
				{
					syserr("write error");
				}
			}
			if (i < argc - 1)
			{
				write(1, " ", 1);
			}
		}
		write(1, "\n", 1);
		return TRUE;
	}
	else if (argv[0][0] == 'e' && argv[0][1] == 'x' && argv[0][2] == 'i' && argv[0][3] == 't')
	{
		// exit
		exit(0);
	}
	else if (argv[0][0] == 'p' && argv[0][1] == 'w' && argv[0][2] == 'd')
	{
		// pwd
		char *wd = getcwd(NULL, 0);

		if (NULL == wd)
		{
			syserr("pwd error");
		}
		else
		{
			write(1, wd, strlen(wd));
		}
		write(1, "\n", 1);
		return TRUE;
	}
	else if (argv[0][0] == 's' && argv[0][1] == 'e' && argv[0][2] == 't')
	{
		// set
		set(argc, argv);
		return TRUE;
	}
	else if (argv[0][0] == 'u' && argv[0][1] == 'n' && argv[0][2] == 's' && argv[0][3] == 'e' && argv[0][4] == 't')
	{
		// unset
		unset(argc, argv);
		return TRUE;
	}
	else if (argv[0][0] == 'e' && argv[0][1] == 'x' && argv[0][2] == 'p' && argv[0][3] == 'o' && argv[0][4] == 'r' && argv[0][5] == 't')
	{
		// export
		export(argc, argv);
		return TRUE;
	}

	else
	{
		// wrong cmd
		return FALSE;
	}
}

static TOKEN command(int *waitpid, BOOLEAN makepipe, int *pipefdp)
{ /* do simple cmd */

	TOKEN token, term;
	int argc, srcfd, dstfd, pid, pfd[2];
	char *argv[MAXARG + 1], srcfile[MAXFNAME], dstfile[MAXFNAME];
	char word[MAXWORD];
	BOOLEAN append;

	argc = 0;
	srcfd = 0;
	dstfd = 1;
	while (1)
	{
		switch (token = gettoken(word))
		{
		case T_WORD:
			if (argc == MAXARG)
			{
				fprintf(stderr, "Too many args\n");
				break;
			}
			if ((argv[argc] = malloc(strlen(word) + 1)) == NULL)
			{
				fprintf(stderr, "Out of argmemory\n");
				break;
			}
			strcpy(argv[argc], word);
			argc++;
			continue;
		case T_LT:
			if (makepipe)
			{
				fprintf(stderr, "Extra<\n");
				break;
			}
			if (gettoken(srcfile) != T_WORD)
			{
				fprintf(stderr, "Illegal <\n");
				break;
			}
			srcfd = BADFD;
			continue;
		case T_GT:
			if (dstfd != 1)
			{
				fprintf(stderr, "Extra > or >> \n");
				break;
			}
			if (gettoken(dstfile) != T_WORD)
			{
				fprintf(stderr, "Illegal > or >> \n");
				break;
			}
			dstfd = BADFD;
			append = FALSE;
			continue;
		case T_GTGT:
			if (dstfd != 1)
			{
				fprintf(stderr, "Extra > or >>\n");
				break;
			}
			if (gettoken(dstfile) != T_WORD)
			{
				fprintf(stderr, "Illegal >or >>\n");
				break;
			}
			dstfd = BADFD;
			append = TRUE;
			continue;
		case T_BAR:
		case T_AMP:
		case T_SEMI:
		case T_NL:
			argv[argc] = NULL;
			if (token == T_BAR)
			{
				if (dstfd != 1)
				{
					fprintf(stderr, "> or >> conflicts with |\n");
					break;
				}
				// term = command(waitpid, TRUE, &dstfd);
				term = command(waitpid, TRUE, &srcfd);
				makepipe = TRUE;
			}
			else
				term = token;

			if (makepipe && pipefdp != NULL)
			{
				if (pipe(pfd) == -1)
					syserr("pipe");
				if (pfd[0] == 0)
				{
					pipe(pfd);
				}
				// printf("pfd: %d %d\n", pfd[0], pfd[1]);
				//  *pipefdp = pfd[1];
				//  srcfd = pfd[0];
				dstfd = pfd[1];
				*pipefdp = pfd[0];
			}

			if (term == T_AMP)
				pid = invoke(argc, argv, srcfd,
							 srcfile, dstfd, dstfile, append, TRUE, makepipe);
			else
			{
				// printf("%s %d %d %d\n", argv[0], srcfd, dstfd, makepipe);
				pid = invoke(argc, argv, srcfd, srcfile, dstfd, dstfile,
							 append, FALSE, makepipe);
			}

			if (token != T_BAR)
				*waitpid = pid;
			if (argc == 0 && (token != T_NL || srcfd > 1))
				fprintf(stderr, "Missing command\n");
			while (--argc >= 0)
				free(argv[argc]);
			return (term);
		case T_EOF:
			exit(0);
		}
	}
}

/////////////////////////////////////////////////////////////////
// you should better not modify this function 						     //
/////////////////////////////////////////////////////////////////
int main()
{ /* real shell */
	char *prompt;
	int pid, fd;
	TOKEN term;
	void waitfor();

	ignoresig();
	if (!EVinit())
		fatal("can't initialize environment");
	if ((prompt = EVget("PS2")) == NULL)
		prompt = ">";
	printf("%s", prompt);

	while (1)
	{
		term = command(&pid, FALSE, NULL);
		if (term != T_AMP && pid != 0)
			waitfor(pid);
		if (term == T_NL)
			printf("%s", prompt);
		for (fd = 3; fd < 20; fd++)
			(void)close(fd); /* ignore error */
	}
	return (0);
}
