#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFSIZE 1

int main(void)
{
    int n;
    char buf[BUFFSIZE];
    int fp;

    fp = open("test_file", O_RDONLY);
    if (fp < 0)
    {
        printf("open error\n");
        exit(1);
    }
    
    while ((n = read(fp, buf, BUFFSIZE)) > 0)
    {
        ;
    }

    if (n < 0)
    {
        printf("read error");
        exit(1);
    }
    close(fp);
    exit(0);
}