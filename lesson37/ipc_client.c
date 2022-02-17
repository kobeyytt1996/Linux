#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main() {
    int fd  = socket(AF_LOCAL, SOCK_STREAM, 0);

    struct sockaddr_un clientaddr;
    clientaddr.sun_family = AF_LOCAL;
    strcpy(clientaddr.sun_path, "client.sock");
    int ret = bind(fd, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    struct sockaddr_un serveraddr;
    serveraddr.sun_family = AF_LOCAL;
    strcpy(serveraddr.sun_path, "server.sock");
    connect(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    while (1) {
        char buf[128] = "last";
        send(fd, buf, strlen(buf) + 1, 0);
        int len = recv(fd, buf, sizeof(buf), 0);
        if (len == -1) {
            perror("recv");
            exit(-1);
        } else if (len == 0) {
            printf("server closed!!!\n");
            break;
        }
        printf("client recv: %s\n", buf);
        
        sleep(1);
    }

    close(fd);
    return 0;
}