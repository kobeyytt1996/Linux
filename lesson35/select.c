#include <sys/select.h>

       /* According to earlier standards */
       #include <sys/time.h>
       #include <sys/types.h>
       #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sAddr;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(9999);
    sAddr.sin_addr.s_addr = INADDR_ANY;
    bind(lfd, (struct sockaddr*)&sAddr, sizeof(sAddr));

    listen(lfd, 8);

    fd_set rdset, tempset;
    FD_ZERO(&rdset);
    FD_ZERO(&tempset);
    FD_SET(lfd, &rdset);
    FD_SET(lfd, &tempset);
    int maxfd = lfd;

    while (1) {
        int ret = select(maxfd + 1, &rdset, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select");
            exit(-1);
        } else if (ret == 0) {
            continue;
        } else {
            if (FD_ISSET(lfd, &rdset) == 1) {
                struct sockaddr_in cAddr;
                socklen_t cAddrLen = sizeof(cAddr);
                int cfd = accept(lfd, (struct sockaddr*)&cAddr, &cAddrLen);

                FD_SET(cfd, &tempset);
                maxfd = maxfd > cfd ? maxfd : cfd;
            }

            for (int i = lfd + 1; i <= maxfd; ++i) {
                if (FD_ISSET(i, &rdset) == 1) {
                    char buf[1024] = {0};
                    int len = recv(i, buf, sizeof(buf), 0);
                    if (len == 0) {
                        printf("client closed\n");
                        FD_CLR(i, &tempset);
                        close(i);
                    } else if (len == -1) {
                        perror("recv");
                        exit(-1);
                    } else {
                        printf("recv: %s\n", buf);
                        send(i, buf, strlen(buf) + 1, 0);
                    }
                }
            }

            memcpy(&rdset, &tempset, sizeof(tempset));
        }
    }

    close(lfd);

    return 0;
}