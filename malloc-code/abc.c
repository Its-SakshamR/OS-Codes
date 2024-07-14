#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(){
    unsigned long long int n = 100000000;
    int *p = (int*)mmap(0,n*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(p==MAP_FAILED){
        printf("Error\n");
    }
    else{
        for(int i = 0 ; i  < n ; i++){
            p[i] = i+1;
            // printf("%d ",p[i]);
            // sprintf(p, i);
            printf("%d ", p[i]);
            // printf("Give: ");
            // scanf("%d",&p[i]);
            usleep(0.6);
        }
        // sleep(600);
    }
}