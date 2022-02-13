#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int tickets = 100;

void *sellTickets(void *args) {
    while (tickets > 0)
    {
        usleep(3000);
        printf("tid: %ld sell ticket: %d\n", pthread_self(), tickets--);
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellTickets, NULL);
    pthread_create(&tid2, NULL, sellTickets, NULL);
    pthread_create(&tid3, NULL, sellTickets, NULL);

    pthread_detach(tid2);
    pthread_join(tid1, NULL);
    pthread_join(tid3, NULL);


    pthread_exit(NULL);
    return 0;
}