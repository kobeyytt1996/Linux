#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int cfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    inet_pton(AF_INET, "192.168.161.128", &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(9999);
    serveraddr.sin_family = AF_INET;

    connect(cfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    while (1) {
        char * data = "I am client";
        write(cfd, data, strlen(data));

        char recvBuf[1024] = {0};
        int ret = read(cfd, recvBuf, sizeof(recvBuf));
        if (ret == 0) {
            printf("server closed\n");
            break;
        } else {
            printf("recv server: %s\n", recvBuf);
        }

        sleep(1);
    }

    close(cfd);

    return 0;
}