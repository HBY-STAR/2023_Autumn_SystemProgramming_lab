#include <sys/types.h> 
#include <sys/ioctl.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static int sigflag;
static sigset_t newmask, oldmask, zeromask;

static void siguser(int signo)
{
    sigflag = 1;
}

void TELL_WAIT(void)
{
    if (signal(SIGUSR1, siguser) == SIG_ERR)
        printf("siguser1 error\n");
    if (signal(SIGUSR2, siguser) == SIG_ERR)
        printf("siguser2 error\n");
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask))
        printf("sigblock error\n");
}

void TELL_PARENT(pid_t pid)
{
    kill(pid, SIGUSR2);
}

void TELL_CHILD(pid_t pid)
{
    kill(pid, SIGUSR1);
}

void WAIT_PARENT(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);

    sigflag = 0;

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL))
        printf("sigsetmask error");
}

void WAIT_CHILD(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);

    sigflag = 0;

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL))
        printf("sigsetmask error");
}

void test_parent_wait_child(){
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        // 创建进程失败
        fprintf(stderr, "创建子进程失败\n");
    }
    else if (pid == 0)
    {
        // 在子进程中
        printf("this is child.  PID:%d\n", getpid());
        TELL_PARENT(getppid());
        WAIT_PARENT();
        exit(0);
    }
    else
    {
        // 在父进程中
        WAIT_CHILD();
        printf("here is parent. PID:%d\n", getpid());
        TELL_CHILD(pid);
    }
}

void test_child_wait_parent(){
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        // 创建进程失败
        fprintf(stderr, "创建子进程失败\n");
    }
    else if (pid == 0)
    {
        // 在子进程中
        WAIT_PARENT();
        printf("this is child.  PID:%d\n", getpid());
        TELL_PARENT(getppid());
        exit(0);
    }
    else
    {
        // 在父进程中
        printf("here is parent. PID:%d\n", getpid());
        TELL_CHILD(pid);
        WAIT_CHILD();
    }
}

int main(void)
{
    TELL_WAIT();

    printf("===========TEST_START===========\n");
    printf("Parent first:\n");
    test_child_wait_parent();
    printf("\n");
    printf("Child  first:\n");
    test_parent_wait_child();
    printf("===========TEST_END=============\n");
    printf("\n");
    printf("====Child_Wait_Parent_Output====\n");
    test_child_wait_parent();
    printf("==========Output_End============\n");
    return 0;
}
