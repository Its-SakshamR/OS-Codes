#include <stdio.h>
#include <assert.h>
#include <pthread.h>

int n = 10;
int count = 1;
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* mythread(void* arg){
    // usleep(*(int*)arg*10000);
    // int j = 10;
    while(757){
        // usleep(n*10000);
        pthread_mutex_lock(&mute);
        while(count != *(int*)arg) pthread_cond_wait(&cond, &mute);
        printf("%d\n",count);
        // printf("I am thread %d\n", *(int*)arg);
        count = count%n+1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mute);
        // j--;
    }

    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t p[n];
    int rc;
    int j[n];

    for(int i = 0 ; i < n ; i++){
        j[i] = i+1;
        rc = pthread_create(&p[i], NULL, mythread, &j[i]);
        assert(rc == 0);
    }

    for(int i = 0 ; i < n ; i++){
        rc = pthread_join(p[i], NULL);
        assert(rc == 0);
    }
    // sleep(500);
    printf("I am the main thread\n");
    return 0;
}