#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int main(){
    pid_t pid = fork();

    if(pid == 0){
        sleep(600);
        exit(EXIT_SUCCESS);
    }

    else{
        // int status;
        // waitpid(pid, &status, 0);
        sleep(2);
        if(kill(pid,SIGTERM) == 0){
            printf("Bacche ko maar daala, over!");
        }
        exit(EXIT_SUCCESS);
    }
    return 0;
}