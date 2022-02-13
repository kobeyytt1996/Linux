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
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    pthread_t tid;
    int ret = pthread_create(&tid, &attr, callback, NULL);

    if (ret) {
        printf("create: %s\n", strerror(ret));
    }

    size_t size;
    pthread_attr_getstacksize(&attr, &size);

    printf("child stack: %ld\n", size);

    printf("tid: %ld, main: %ld\n", tid, pthread_self());

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
    
    return 0;
}