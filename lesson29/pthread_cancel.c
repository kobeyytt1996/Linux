#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *callback(void *arg) {
    printf("tid: %ld\n", pthread_self());
    for (int i = 0; i < 5; ++i) {
        printf("i: %d\n", i);
    }
    return NULL;
}

int main() {
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);

    if (ret) {
        printf("create: %s\n", strerror(ret));
    }

    pthread_cancel(tid);

    printf("tid: %ld, main: %ld\n", tid, pthread_self());

    pthread_exit(NULL);
    
    return 0;
}