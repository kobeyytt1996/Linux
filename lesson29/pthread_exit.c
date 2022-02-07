#include <pthread.h>
#include <stdio.h>
#include <string.h>

void *callback(void *args) {
    printf("child thread: %ld\n", pthread_self());
    // pthread_exit(NULL);
    
    return NULL;
}

int main() {
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);
    if (ret) {
        printf("create: %s\n", strerror(ret));
    }

    printf("main thread: %ld child: %ld\n", pthread_self(), tid);
    pthread_exit(NULL);
    
    return 0;
}