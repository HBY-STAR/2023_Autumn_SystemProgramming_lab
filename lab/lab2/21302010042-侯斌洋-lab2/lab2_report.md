<center><h1>lab2_report</h1></center>
<p align="right">21302010042</p>
<p align="right">侯斌洋</p>
<h3>（1）cd</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
作为builtin命令在shell进程中直接调用chdir()函数，改变shell进程的工作目录。同时设置shell的$PWD变量为更改之后的目录。
</p>
<h4>代码：</h4>

```C
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
```
<h4>运行示例：（下面例子中的 print_hello 和 get_and_print 等为测试用的可执行文件，已附在源代码文件夹里）</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>pwd
/home/hby/Desktop/system_programming/lab2
>print_hello
hello
>cd ../
>pwd
/home/hby/Desktop/system_programming
>print_hello
ERROR: exec error (2;No such file or directory)
>cd lab2
>pwd
/home/hby/Desktop/system_programming/lab2
>print_hello
hello
```
</p>
<h3>（2）echo</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
对于两边带引号的字符串要去除最外层的引号之后打印，对于环境变量要搜索环境变量表获取值并打印，对于其他的字符串直接打印。
</p>
<h4>代码：</h4>

```C
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
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>echo hello
hello
>echo "hello"
hello
>echo 'hello'
hello
>echo $HOME
/home/hby
>echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
```
</p>
<h3>（3）exit</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
直接调用exit(0)正常退出即可。
</p>
<h4>代码：</h4>

```C
// exit
exit(0);
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>exit
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ 
```
</p>
<h3>（4）pwd</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
通过getcwd()函数获取当前的工作目录并打印。
</p>
<h4>代码：</h4>

```C
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
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>pwd
/home/hby/Desktop/system_programming/lab2
>cd ../
>pwd
/home/hby/Desktop/system_programming
>cd lab2
>pwd
/home/hby/Desktop/system_programming/lab2
>
```
</p>
<h3>（5）></h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
根据command()中的代码，srcfd 或 dstfd 为 BADFD 的时候就需要重定向，此时再检测对应的 srcfile 和 dstfile，若合法的话使用dup2()进行IO重定向。注意到在执行完一次命令时应当恢复IO重定向，故要保存重定向之前的文件描述符。即：

```C
old_src = dup(0);
```
之后只需要在合适的位置调用rediction()，调用完成后再进行恢复即可。如：

```C
// rediction
redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);

// builtin command
if (!builtin(argc, argv, srcfd, dstfd))
{
    syserr("built in command error");
}

// redirection recover
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
```
</p>
<h4>代码：redirection()</h4>

```C
// rediction()
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
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ cat z
cat: z: No such file or directory
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>echo hello > z
>exit
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ cat z
hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>echo world >> z
>exit
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ cat z
hello
world
```
</p>
<h3>（6）<</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
重定向输入和输出的思路是一样的。
</p>
<h4>代码：</h4>

```C
// 同上 > 处代码，IO重定向
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./get_and_print
hello
hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ cat z
hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>get_and_print < z
hello
```
</p>
<h3>（7）>></h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
重定向输出到流只需要在打开文件的时候加上O_APPEND即可。普通的重定向输出则是使用O_TRUNC直接截断文件使其长度为0之后再操作。
</p>
<h4>代码：</h4>

```C
// 同上 > 处代码，IO重定向
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ cat z
hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>echo world >> z
>exit
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ cat z
hello
world
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ 
```
</p>
<h3>（8）set & =</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
通过 EVset() 将环境变量实际存到sym表中。而 set() 则通过参数判断是调用EVprint()还是EVset()，从而完成set指令。
</p>
<h4>代码：</h4>

```C
// EVset
BOOLEAN EVset(char *name, char *val)
{ /* add name & valude to enviromnemt */
    struct varslot *v;
    v = find(name);
    if (v == NULL)
    {
        return (FALSE);
    }
    if (v->name == NULL)
    {
        v->name = calloc(200, 1);
        if (v->name == NULL)
        {
            return (FALSE);
        }
        strcat(v->name, name);
    }

    if (v->val != NULL)
    {
        free(v->val);
    }
    v->val = calloc(200, 1);
    if (v->val == NULL)
    {
        return (FALSE);
    }
    strcat(v->val, val);
    v->exported = FALSE;
    return (TRUE);
}

//set
void set(int argc, char *argv[])
{ /* set command */
    if (argc == 1)
    {
        EVprint();
        return;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            int namelen = strcspn(argv[i], "=");
            if (namelen == strlen(argv[i]))
            {
                syserr("set error");
            }
            else
            {
                char name[100] = {'\0'};
                strncpy(name, argv[i], namelen);
                name[namelen] = '\0';
                if (!EVset(name, &argv[i][namelen + 1]))
                {
                    syserr("set error");
                }
            }
        }
    }
}
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>set
[E] HOME=/home/hby
[E] PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
[E] PWD=/home/hby/Desktop/system_programming/lab2
>set USER_1=hello
>set
[E] HOME=/home/hby
[E] PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
[E] PWD=/home/hby/Desktop/system_programming/lab2
    USER_1=hello
>export USER_1
>set
[E] HOME=/home/hby
[E] PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
[E] PWD=/home/hby/Desktop/system_programming/lab2
[E] USER_1=hello
>export USER_2=world
>export
[E] HOME=/home/hby
[E] PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
[E] PWD=/home/hby/Desktop/system_programming/lab2
[E] USER_1=hello
[E] USER_2=world
>unset USER_1
>unset USER_2
>export
[E] HOME=/home/hby
[E] PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
[E] PWD=/home/hby/Desktop/system_programming/lab2
>
```
</p>
<h3>（9）export & =</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
通过 EVexport() 设置环境变量为导出状态。export() 则通过参数判断是调用 EVprint()、 EVexport() 还是同时调用 EVset()和 EVexport()，从而实现export命令的三种功能。
</p>
<h4>代码：</h4>

```C
// EVexport
BOOLEAN EVexport(char *name)
{ /* set variable to be exported */
    struct varslot *v;
    v = find(name);
    if (v == NULL || v->name == NULL)
    {
        return (FALSE);
    }
    v->exported = TRUE;
    return (TRUE);
}

// export
void export(int argc, char *argv[])
{ /* export command */
    int i;

    if (argc == 1)
    {
        set(argc, argv);
        return;
    }
    for (int i = 1; i < argc; i++)
    {
        int namelen = strcspn(argv[i], "=");
        if (namelen == strlen(argv[i]))
        {
            if (!EVexport(argv[i]))
            {
                printf("Cannot export %s\n", argv[i]);
                return;
            }
        }
        else
        {
            char name[100];
            strncpy(name, argv[i], namelen);
            name[namelen] = '\0';
            if (!EVset(name, &argv[i][namelen + 1]) || !EVexport(name))
            {
                syserr("export error");
            }
        }
    }
}
```
<h4>运行示例：</h4>

```shell
#见上 set & = 处运行示例
```
</p>
<h3>（10）unset</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
通过 EVunset() 将sym表中对应的环境变量删除。 而unset() 则解析 argv 并直接调用EVunset()。
</p>
<h4>代码：</h4>

```C
// EVunset
BOOLEAN EVunset(char *name)
{ /* add name & valude to enviromnemt */
    struct varslot *v;
    v = find(name);
    if (v == NULL || v->name == NULL)
    {
        return (FALSE);
    }
    free(v->name);
    if (v->val != NULL)
    {
        free(v->val);
    }
    v->name = NULL;
    v->val = NULL;
    v->exported = FALSE;
    return (TRUE);
}

// unset
void unset(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (!EVunset(argv[i]))
        {
            syserr("unset error");
        }
    }
}
```
<h4>运行示例：</h4>

```shell
#见上 set & = 处运行示例
```
</p>
<h3>（11） | </h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
由于command函数中对于一行管道命令的执行是递归的，故一行命令的执行是从右往左的。见下：

```C
if (token == T_BAR)
{
    if (dstfd != 1)
    {
        fprintf(stderr, "> or >> conflicts with |\n");
        break;
    }

    // look at this line !
    term = command(waitpid, TRUE, &srcfd);

    makepipe = TRUE;
}
```
因此在不对command函数进行大幅度改动的情况下，本次lab实现了从右往左顺序的管道，具体见下面示例，希望能酌情给分。
管道实现的主要思路为:首先在command函数中创建管道，并将其赋值给srcfd和dstfd,之后对srcfd和dstfd进行正确的重定向和恢复重定向即可。
</p>
<h4>代码：</h4>

```C
// 以下为在command函数中改动的代码（注释部分为原本代码）
// term = command(waitpid, TRUE, &dstfd);
term = command(waitpid, TRUE, &srcfd);

// *pipefdp = pfd[1];
// srcfd = pfd[0];
dstfd = pfd[1];
*pipefdp = pfd[0];

// 以下为invoke函数中的代码
// pipe
if (srcfd != BADFD && dstfd != BADFD)
{
    if (makepipe)
    {
        old_src = dup(0); // Save old to recover
        dup2(srcfd, 0); // Redirect stdin to the read end
        close(srcfd);	// Close the read end

        old_dst = dup(1); // Save old to recover
        dup2(dstfd, 1); // Redirect stdout to the write end
    }
}
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./get_and_print_1+get
hello
1 + hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./get_and_print_2+get
world
2 + world
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./print_hello | ./get_and_print_2+get | ./get_and_print_1+get
1 + 2 + hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>get_and_print_1+get | get_and_print_2+get | print_hello
1 + 2 + hello
>get_and_print_1+get | print_hello
1 + hello
>get_and_print_1+get | echo world
1 + world
# 注：可以观察到spsh的管道顺序恰好与bash相反。
```
</p>

<h3>（12）execvp</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
对于可执行程序，由于执行 execvp() 会替换当前进程空间，故需要 fork() 并在子进程中执行该程序。
</p>
<h4>代码：</h4>

```C
// execvp
if ((pid = fork()) < 0)
{
    syserr("fork error");
    exit(-1);
}
// child
else if (pid == 0)
{
    redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
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
    if (!bckgrnd)
    {
        wait(0);
    }
    return pid;
}
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./print_hello
hello
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>print_hello
hello
```
</p>

<h3>（13）&</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
若指定为后台执行，则shell在fork后不会等待子进程执行完毕，否则shell会一直wait直到子进程返回。
</p>
<h4>代码：</h4>

```C
// &
if (!bckgrnd)
{
    wait(0);
}
return pid;
```
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>get_and_print
hello
hello
>get_and_print &
>hello
ERROR: exec error (2;No such file or directory)
```
</p>




<h3>（14）指令合法性检查</h3>
<p>
<h4>思路：</h4>
<p>&emsp;&emsp;
在各个builtin命令内部先检查argv是否符合要求并通过syserr()提示错误。在command里面也帮我们实现了一部分的指令合法性检查。下面是对每个命令的合法性检查例子。
</p>
<h4>运行示例：</h4>

```shell
hby@hby-ubuntu:~/Desktop/system_programming/lab2$ ./spsh
>print_hello
hello
>not_a_program
ERROR: exec error (2;No such file or directory)
>cd not_a_path
ERROR: cd error (2;No such file or directory)
ERROR: built in command error (2;No such file or directory)
>print_hello >
Illegal > or >> 

hello
>print_hello >>  
Illegal >or >>

hello
>print_hello < 
Illegal <

hello
>print_hello |
Missing command
hello
>set not_legal
ERROR: set error (9;Bad file descriptor)
>unset not_legal
ERROR: unset error (9;Bad file descriptor)
>export not_legal
Cannot export not_legal
```
</p>

<h3>（15）注：还有一些代码并未在上面列出，具体实现可见源代码文件</h3>