#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFSIZE 4096
#define FILESIZE 40960000

int main(void)
{
    int n;
    int fp;
    fp = open("test_file", O_WRONLY | O_CREAT | O_SYNC);
    if (fp < 0)
    {
        printf("open error\n");
        exit(1);
    }
    // write buf
    char buf[BUFFSIZE];
    for (int i = 0; i < BUFFSIZE; i++)
    {
        buf[i] = '6';
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