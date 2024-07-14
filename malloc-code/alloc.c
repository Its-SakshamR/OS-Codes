#include "alloc.h"
#include <stdbool.h>

#define NUM_ELE (PAGESIZE / MINALLOC)
bool bitmap[NUM_ELE];
int how_many[NUM_ELE];
char* ptr;
int init_alloc(){
    ptr = (char*)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED){
        printf("Error\n");
        return -1;
    }

    memset(ptr, 0, PAGESIZE);
    for(int i = 0 ; i < NUM_ELE ; i++){
        bitmap[i] = 1;
        how_many[i] = 0;
    }

    return 0;
}


int cleanup(){
    for(int i = 0 ; i < NUM_ELE ; i++){
        bitmap[i] = 1;
        how_many[i] = 0;
    }

    return munmap(ptr, PAGESIZE);
}

char* alloc(int size){
    if(size % MINALLOC){
        printf("Requested memory is not a multiple of %d bytes\n", MINALLOC);
        return NULL;
    }

    int req_mem = size/MINALLOC;

    for (int i = 0 ; i < NUM_ELE ; i++){
        int x = 0;
        while(x < req_mem){
            if(!bitmap[i+x]){
                break;
            }
            x++;
        }

        if(x == req_mem){
            how_many[i] += x;
            x--; // As it is exactly equal to req_mem now, but it should be 1 lesser
            while(x >= 0){
                bitmap[i+x] = 0;
                x--;
            }

            return ptr+i*MINALLOC;
        }
    }

    return NULL;
}

void dealloc(char* pos){
    int chunk_num = (pos - ptr)/MINALLOC;
    int x = how_many[chunk_num];
    how_many[chunk_num] = 0;
    memset(pos,0,x*8);
    for(int i = 0 ; i < x; i++){
        bitmap[chunk_num + i] = 1;
    }
}