#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    // struct in_addr ipaddr;
    // inet_pton(AF_INET, "192.168.161.128", &ipaddr);
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(9999);
    bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));

    listen(lfd, 5);

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &len);

    if (cfd == -1) {
        perror("accept");
        exit(-1);
    }

    char clientIP[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    printf("client IP: %s\n", clientIP);
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client port: %d\n", clientPort);

    while (1) {
        char recvBuf[1024] = {0};
        int ret = read(cfd, recvBuf, sizeof(recvBuf));
        if (ret == 0) {
            printf("client closed\n");
            break;
        } else {
            printf("recv client: %s\n", recvBuf);
        }

        char * data = "I am server";
        write(cfd, data, strlen(data));
    }

    close(cfd);
    close(lfd);
    
    return 0;
}