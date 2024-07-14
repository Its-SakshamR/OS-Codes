#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static volatile int counter = 0;
pthread_mutex_t m;

void* mythread(void* arg){
    printf("%s : begin\n", (char*)arg);

    pthread_mutex_lock(&m);
    for(int i = 0 ; i < 1000 ; i++){
        counter++;
    }
    pthread_mutex_unlock(&m);

    return NULL;
}

int main(int argc, char *argv[]){
    int n = 10;
    pthread_t p[n];
    int rc;

    for(int i = 0 ; i < n ; i++){
        rc = pthread_create(&p[i], NULL, mythread, "Thread Created");
        assert(rc == 0);
    }

    for(int i = 0 ; i < n ; i++){
        rc = pthread_join(p[i], NULL);
        assert(rc == 0);
    }

    printf("Counter: %d\n", counter);
    return 0;
}
