#include<stdio.h>
int main(void){
    char s[100]="\0";
    gets(s);
    printf("2+%s\n",s);
    return 0;
}