#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "unix_socket_example"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_un serv_addr;
    char buffer[256];

    /* create socket, get sockfd handle */
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH); 

    bzero(buffer,256);
    FILE* fp;
    fp = fopen("abc.txt", "r");

    if(fp == NULL){
        printf("Error Opening file\n");
        exit(1);
    }

    fseek(fp, 0 ,SEEK_END);
    int file_size = ftell(fp);
    fclose(fp);

    n = sendto(sockfd, &file_size, sizeof(file_size), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (n < 0) 
        error("ERROR writing to socket");

    fp = fopen("abc.txt", "r");
    while(fgets(buffer, 256, stdin) != NULL){
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (strcmp(buffer, "end\n") == 0)
            break;
    }

    fclose(fp);

    close(sockfd);
    return 0;
}
