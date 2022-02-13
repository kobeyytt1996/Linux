#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int num = 1;
pthread_mutex_t mutex;
pthread_rwlock_t rwlock;

void *thWrite(void *args) {
    while (1)
    {
        // pthread_mutex_lock(&mutex);
        pthread_rwlock_wrlock(&rwlock);
        ++num;
        printf("write tid: %ld, num: %d\n", pthread_self(), num);
        usleep(100);
        // pthread_mutex_unlock(&mutex);
        pthread_rwlock_unlock(&rwlock);
    }


    pthread_exit(NULL);
}

void *thRead(void *args) {
    while (1)
    {
        // pthread_mutex_lock(&mutex);
        pthread_rwlock_rdlock(&rwlock);
        printf("read tid: %ld, num: %d\n", pthread_self(), num);
        usleep(100);
        // pthread_mutex_unlock(&mutex);
        pthread_rwlock_unlock(&rwlock);
    }

    pthread_exit(NULL);
}

int main() {
    // pthread_mutex_init(&mutex, NULL);
    pthread_rwlock_init(&rwlock, NULL);
    pthread_t wtids[3], rtids[5];
    for (int i = 0; i < 3; ++i) {
        pthread_create(&wtids[i], NULL, thWrite, NULL);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_create(&rtids[i], NULL, thRead, NULL);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_detach(wtids[i]);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_detach(rtids[i]);

    }

    // pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    pthread_rwlock_destroy(&rwlock);


    return 0;
}