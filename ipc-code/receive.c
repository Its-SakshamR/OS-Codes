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
        // Open FIFO for write only
        fd = open(myfifo, O_RDONLY);
 
        // Take an input arr2ing from user.
        // 80 is maximum length
        read(fd, arr, sizeof(arr));
 
        // Write the input arr2ing on FIFO
        // and close it
        if(!strcmp(arr,"end\n")){
            printf("Sender ended the chat");
            break;
        }
        printf("User2: %s", arr);
        arr[0] = '\0';
        close(fd);
    }

    return 0;
}