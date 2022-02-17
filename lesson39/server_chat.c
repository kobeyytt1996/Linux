#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

pthread_mutex_t mutex;
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];

void *handle_clnt(void *args);
void send_msg(char *buf, int str_len, int self_sock);

int main() {
    int sock_serv = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr, clnt_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(9999);
    bind(sock_serv, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(sock_serv, 8);

    pthread_mutex_init(&mutex, NULL);
    int clnt_addr_sz = sizeof(clnt_addr);
    pthread_t pid;
    while (1) {
        int clnt_sock = accept(sock_serv, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        pthread_mutex_lock(&mutex);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutex);

        pthread_create(&pid, NULL, handle_clnt, &clnt_sock);
        pthread_detach(pid);
    }

    close(sock_serv);

    return 0;
}

void *handle_clnt(void *args) {
    int clnt_sock = *(int*)args;
    char buf[BUF_SIZE];
    int str_len = 0;
    while ((str_len = recv(clnt_sock, buf, BUF_SIZE, 0)) != 0) {
        send_msg(buf, str_len, clnt_sock);
    }

    if (str_len == 0) {
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < clnt_cnt; ++i) {
            if (clnt_socks[i] == clnt_sock) {
                while (i < clnt_cnt-1) {
                    clnt_socks[i] = clnt_socks[i+1];
                    ++i;
                }
                break;
            }
        }
        --clnt_cnt;
        pthread_mutex_unlock(&mutex);

        close(clnt_sock);
    }

    pthread_exit(NULL);
}


void send_msg(char *buf, int str_len, int self_sock) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clnt_cnt; ++i) {
        if (clnt_socks[i] != self_sock) {
            send(clnt_socks[i], buf, str_len, 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}