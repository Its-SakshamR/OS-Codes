#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
    int k = 5;
    printf("Abhi to bas shuruat hai!\n");
    execlp("sleep","sleep","2", (char*)NULL);
    k = 6;
    printf("Khatam! Tata, Bye-Bye\n");

    printf("%d",k);
}