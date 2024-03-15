#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFSIZE 1
#define FILESIZE 409600

int main(void)
{
    int n;
    int fp;
    fp = open("test_file2", O_WRONLY | O_CREAT);
    if (fp < 0)
    {
        printf("open error\n");
        exit(1);
    }
    // write buf
    char buf[BUFFSIZE];
    for (int i = 0; i < BUFFSIZE; i++)
    {
        buf[i] = '2';
    }

    // start write
    for (int i = 0; i < FILESIZE / BUFFSIZE; i++)
    {
        if (write(fp, buf, BUFFSIZE) != BUFFSIZE)
        {
            printf("write error\n");
            exit(1);
        }
    }
    close(fp);
    exit(0);
}