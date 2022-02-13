#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

struct Node {
    int val;
    struct Node * next;
};

struct Node *head;

pthread_mutex_t mutex;

sem_t psem, csem;

void *produce(void *args) {

    while (1)
    {
        sem_wait(&psem);
        pthread_mutex_lock(&mutex);

        struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->next = head;
        head = newNode;
        newNode->val = rand() % 1000;
        printf("add node: %d, tid: %ld\n", newNode->val, pthread_self());

        sem_post(&csem);

        pthread_mutex_unlock(&mutex);

        usleep(100);
    }
    

    pthread_exit(NULL);
}

void *consume(void *args) {

    while (1)
    {
        sem_wait(&csem);
        pthread_mutex_lock(&mutex);

     
        struct Node *getNode = head;
        head = head->next;
        getNode->next = NULL;
        printf("del node: %d, tid: %ld\n", getNode->val, pthread_self());
        free(getNode);

        sem_post(&psem);

        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&psem, 0, 8);
    sem_init(&csem, 0, 0);

    pthread_t ptids[5], ctids[5];
    for (int i = 0; i < 5; ++i) {
        pthread_create(&ptids[i], NULL, produce, NULL);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_create(&ctids[i], NULL, consume, NULL);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while (1)
    {
        sleep(10);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&psem);
    sem_destroy(&csem);

    
    pthread_exit(NULL);

    return 0;
}