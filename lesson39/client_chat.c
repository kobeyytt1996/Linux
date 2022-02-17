#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

char name[20];
void *sendMsg(void *args);
void *recvMsg(void *args);

int main(int argc, char **argv) {
    sprintf(name, "[%s]", argv[1]);

    int clnt_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(9999);

    connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    pthread_t send_pid, recv_pid;
    pthread_create(&send_pid, NULL, sendMsg, &clnt_sock);
    pthread_create(&recv_pid, NULL, recvMsg, &clnt_sock);
    pthread_join(send_pid, NULL);
    pthread_join(recv_pid, NULL);

    close(clnt_sock);
    return 0;
}

void *sendMsg(void *args) {
    int sock = *(int*)args;
    char name_msg[100];
    char msg[80];
    while (1) {
        fgets(msg, 80, stdin);
        if (!strcmp(msg, "q\n")) {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);
        send(sock, name_msg, sizeof(name_msg), 0);
    }
    pthread_exit(NULL);
}

void *recvMsg(void *args) {
    int sock = *(int*)args;
    char name_msg[100];

    while (1) {
        int str_len = recv(sock, name_msg, 100, 0);
        if (str_len <= 0) {
            pthread_exit(NULL);
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    pthread_exit(NULL);
}