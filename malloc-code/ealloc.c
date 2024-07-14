#include "ealloc.h"
#include <stdbool.h>

#define NUM_ELE (PAGESIZE / MINALLOC)
#define MAX_PAGES 4
int curr_pages_used;
bool bitmap[MAX_PAGES][NUM_ELE];
int how_many[MAX_PAGES][NUM_ELE];
char* ptr[MAX_PAGES];
// int chunks_left[MAX_PAGES];

void init_alloc(){
    for(int j = 0 ; j < MAX_PAGES ; j++){
        for(int i = 0 ; i < NUM_ELE ; i++){
            bitmap[j][i] = 1;
            how_many[j][i] = 0;
        }
        // chunks_left[j] = NUM_ELE;
        ptr[j] = NULL;
    }
    curr_pages_used = 0;
}

void cleanup(){
    for(int j = 0 ; j < MAX_PAGES ; j++){
        for(int i = 0 ; i < NUM_ELE ; i++){
            bitmap[j][i] = 1;
            how_many[j][i] = 0;
        }
        // chunks_left[j] = NUM_ELE;
    }
}

char *alloc(int size){
    if(size % MINALLOC){
        printf("Requested memory is not a multiple of %d bytes\n", MINALLOC);
        return NULL;
    }

    int req_mem = size/MINALLOC;
    // int p = 0;

    // while (p < MAX_PAGES){
    //     if(chunks_left[p] >= req_mem){
    //         break;
    //     }
    //     p++;
    // }

    // if(p == MAX_PAGES){
    //     return NULL;
    // }

    // if(chunks_left[p] == NUM_ELE){
    //     ptr[p] = (char*)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //     curr_pages_used++;
    //         if(ptr[p] == MAP_FAILED){
    //         printf("Error\n");
    //         return NULL;
    //     }

    //     memset(ptr[p], 0, PAGESIZE);
    // }

    for(int j = 0 ; j < curr_pages_used ; j++){
        for (int i = 0 ; i < NUM_ELE ; i++){
            int x = 0;
            while(x < req_mem){
                if(!bitmap[j][i+x]){
                    break;
                    // i = i + x - 1;
                }
                x++;
            }

            if(x == req_mem){
                how_many[j][i] += x;
                x--; // As it is exactly equal to req_mem now, but it should be 1 lesser
                while(x >= 0){
                    bitmap[j][i+x] = 0;
                    x--;
                }
                // chunks_left[j] -= req_mem;
                return ptr[j]+i*MINALLOC;
            }
        }
    }

    for(int j = curr_pages_used ; j < MAX_PAGES ; j++){
        ptr[j] = (char*)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        curr_pages_used++;
        if(ptr[j] == MAP_FAILED){
            printf("Error\n");
            return NULL;
        }

        memset(ptr[j], 0, PAGESIZE);
        for (int i = 0 ; i < NUM_ELE ; i++){
            int x = 0;
            while(x < req_mem){
                if(!bitmap[j][i+x]){
                    break;
                }
                x++;
            }

            if(x == req_mem){
                how_many[j][i] += x;
                x--; // As it is exactly equal to req_mem now, but it should be 1 lesser
                while(x >= 0){
                    bitmap[j][i+x] = 0;
                    x--;
                }
                // chunks_left[j] -= req_mem;
                return ptr[j]+i*MINALLOC;
            }
        }
        return NULL;
    }
}

void dealloc(char* pos){
    int p = 0;
    for(int i=0;i<curr_pages_used;i++){
        if(pos-ptr[i]>=0 && (pos-ptr[i])<PAGESIZE) {
            p=i;
            break;
        }
    }
    // while(p < MAX_PAGES && pos > ptr[p]){
    //     p++;
    // }

    int chunk_num = (pos - ptr[p])/MINALLOC;

    int x = how_many[p][chunk_num];
    how_many[p][chunk_num] = 0;
    memset(pos, 0, x*8);
    for(int i = 0 ; i < x ; i++){
        bitmap[p][chunk_num + i] = 1;
    }
    // chunks_left[p] += x;
}