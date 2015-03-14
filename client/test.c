#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main()
{
    char buf[1000];
    char c;

    scanf("%s",buf);
    while(strcmp(buf,"00")!=0){
        printf("--%s--",buf);
        scanf("%s",buf);

    }






}
