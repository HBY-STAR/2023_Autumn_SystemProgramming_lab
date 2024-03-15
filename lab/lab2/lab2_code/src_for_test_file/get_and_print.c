#include<stdio.h>
int main(void){
    char s[100]="\0";
    gets(s);
    printf("%s\n",s);
    return 0;
}