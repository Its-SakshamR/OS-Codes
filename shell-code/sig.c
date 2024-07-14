#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void ctrlCHandler(int signum) {
    printf("Mai rukega nhi\n");
}


int main(){
    signal(SIGINT, ctrlCHandler);
    int x = 0;
    while(1){
        x = x+1;
        printf("%d\n",x);
        sleep(2);
    }

    return 0;
}