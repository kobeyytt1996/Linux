#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int tickets = 100;

pthread_mutex_t mutex;

void *sellTickets(void *args) {

    while (1)
    {
        usleep(6000);
        pthread_mutex_lock(&mutex);

        if (tickets < 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        printf("tid: %ld sell ticket: %d\n", pthread_self(), tickets--);

        pthread_mutex_unlock(&mutex);

    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellTickets, NULL);
    pthread_create(&tid2, NULL, sellTickets, NULL);
    pthread_create(&tid3, NULL, sellTickets, NULL);

    pthread_detach(tid2);
    pthread_join(tid1, NULL);
    pthread_join(tid3, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    return 0;
}