#include <pthread.h>
#include <stdio.h>
#include <string.h>

void *callback(void *arg) {
    printf("child thread arg: %d\n", *(int *)arg);
    return NULL;
}

int main() {
    pthread_t tid;
    int num = 10;

    int ret = pthread_create(&tid, NULL, callback, &num);

    if (ret) {
        printf("error: %s\n", strerror(ret));
    }

    for (int i = 0; i < 5; ++i) {
        printf("%d\n", i);
    }
    return 0;
}