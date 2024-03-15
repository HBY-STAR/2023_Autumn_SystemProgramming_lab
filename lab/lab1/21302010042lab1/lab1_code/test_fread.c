#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFSIZE 1

int main(void)
{
    int n;
    char buf[BUFFSIZE];
    FILE *fp;

    fp = fopen("test_file", "r");
    if (fp == NULL)
    {
        printf("open error\n");
        exit(1);
    }

    while ((n = fread(buf, BUFFSIZE, 1, fp)) > 0)
    {
        ;
    }

    if (n < 0)
    {
        printf("read error");
        exit(1);
    }
    fclose(fp);
    exit(0);
}