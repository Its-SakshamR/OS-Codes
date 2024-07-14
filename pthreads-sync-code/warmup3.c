#include <stdio.h>
#include <assert.h>
#include <pthread.h>

int count = 1;
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* mythread(void* arg){
    // usleep(*(int*)arg*10000);
    pthread_mutex_lock(&mute);
    while(count != *(int*)arg) pthread_cond_wait(&cond, &mute);
    printf("I am thread %d\n", *(int*)arg);
    count++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mute);

    return NULL;
}

int main(int argc, char *argv[]){
    int n = 10;
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

    printf("I am the main thread\n");
    return 0;
}