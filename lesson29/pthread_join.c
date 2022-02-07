#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int value = 10;


void *callback(void *args) {
    printf("child thread: %ld\n", pthread_self());
    // pthread_exit(NULL);
    // sleep(2);
    pthread_exit((void*)&value);
}

int main() {
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);
    if (ret) {
        printf("create: %s\n", strerror(ret));
    }

    printf("main thread: %ld child: %ld\n", pthread_self(), tid);

    int * thread_retval;

    ret = pthread_join(tid, (void*)&thread_retval);
    if (ret) {
        printf("create: %s\n", strerror(ret));
    } else {
        printf("回收成功: %d\n", *thread_retval);
    }


    pthread_exit(NULL);
    
    return 0;
}