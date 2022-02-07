#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *callback(void *arg) {
    printf("tid: %ld\n", pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);

    if (ret) {
        printf("create: %s\n", strerror(ret));
    }

    printf("tid: %ld, main: %ld\n", tid, pthread_self());

    pthread_detach(tid);
    
    ret = pthread_join(tid, NULL);
    if (ret) {
        printf("join: %s\n", strerror(ret));
    }

    pthread_exit(NULL);
    
    return 0;
}