#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main() {
    unlink("server.sock");
    int fd  = socket(AF_LOCAL, SOCK_STREAM, 0);

    struct sockaddr_un serveraddr;
    serveraddr.sun_family = AF_LOCAL;
    strcpy(serveraddr.sun_path, "server.sock");
    int ret = bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    listen(fd, 8);

    struct sockaddr_un cliaddr;
    int cliaddrlen = sizeof(cliaddr);
    int cfd = accept(fd, (struct sockaddr*)&cliaddr, &cliaddrlen);

    printf("client path: %s\n", cliaddr.sun_path);

    while (1) {
        char buf[128];
        int len = recv(cfd, buf, sizeof(buf), 0);
        if (len == -1) {
            perror("recv");
            exit(-1);
        } else if (len == 0) {
            printf("client closed!!!\n");
            break;
        }
        printf("server recv: %s\n", buf);
        send(cfd, buf, len, 0);
    }

    close(cfd);
    close(fd);
    return 0;
}