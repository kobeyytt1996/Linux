#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
       #include <sys/wait.h>
       #include <errno.h>
       #include <pthread.h>

struct sockInfo {
    int fd;
    struct sockaddr_in addr;
    pthread_t ptid;
};

void *working(void *args) {

    pthread_exit(NULL);
}

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(9999);
    bind(lfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    listen(lfd, 8);

    while (1) {
        struct sockaddr_in clientaddr;
        socklen_t clientaddrlen = sizeof(clientaddr);
        int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &clientaddrlen);

        struct sockInfo *pinfo = malloc(sizeof(sockInfo));
        pinfo.fd = cfd;
        

        pthread_t tid;
        pthread_create(&pinfo.ptid, NULL, working, NULL);
    }
    return 0;
}