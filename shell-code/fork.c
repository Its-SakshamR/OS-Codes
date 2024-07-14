#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
    pid_t pid;
    int N;
    scanf("%d", &N);
    int status;
    for(int i = 0 ; i < N ; i++){
        pid = fork();
        if(pid == 0) printf("PID: %d\n",getpid());
        else{waitpid(pid, &status, 0);}
    }

    // if (pid < 0) {
    //     // Error occurred during fork
    //     perror("Fork failed");
    //     exit(EXIT_FAILURE);
    // }

    // else if(pid == 0){
    //     printf("I am child\n");
    //     exit(EXIT_SUCCESS);
    // }

    // else{
    //     int status;
    //     pid_t terminated_pid = waitpid(pid, &status, 0);

    //     printf("I am parent\n");

    //     if (terminated_pid == -1) {
    //         perror("Error waiting for child process");
    //         exit(EXIT_FAILURE);
    //     }

    //     // if (WIFEXITED(status)) {
    //     //     printf("Child process exited with status: %d\n", WEXITSTATUS(status));
    //     // } else {
    //     //     printf("Child process did not exit normally\n");
    //     // }
    // }

    return 0;
}