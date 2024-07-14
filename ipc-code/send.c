#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    int fd;
    char* myfifo = "/tmp/myfifo";
    mkfifo(myfifo,0666);

    char arr[100];
    while (1)
    {
        printf("User 1: ");
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);
 
        // Take an input arr2ing from user.
        // 80 is maximum length
   
        fgets(arr, 100, stdin);
 
        // Write the input arr2ing on FIFO
        // and close it
        write(fd, arr, strlen(arr)+1);
        if(!strcmp(arr,"end\n")){
            break;
        }
        close(fd);
    }

    return 0;
}