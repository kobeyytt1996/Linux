#include <sys/select.h>

       /* According to earlier standards */
       #include <sys/time.h>
       #include <sys/types.h>
       #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sAddr;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(9999);
    sAddr.sin_addr.s_addr = INADDR_ANY;
    bind(lfd, (struct sockaddr*)&sAddr, sizeof(sAddr));

    listen(lfd, 8);

    struct pollfd fds[1024];
    for (int i = 0; i < 1024; ++i) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;

    }
    fds[0].fd = lfd;
    nfds_t upper = 1;

    while (1) {
         
        int ret = poll(fds, upper, -1);

        if (ret == -1) {
            perror("poll");
            exit(-1);
        } else if (ret == 0) {
            continue;
        } else {
            if (fds[0].revents & POLLIN) {
                struct sockaddr_in cAddr;
                socklen_t cAddrLen = sizeof(cAddr);
                int cfd = accept(lfd, (struct sockaddr*)&cAddr, &cAddrLen);

                for (int i = 1; i < 1024; ++i) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        upper = (upper > i + 1) ? upper : i + 1;
                        break;
                    }
                }
            }

            for (int i = 1; i < upper; ++i) {
                if (fds[i].revents & POLLIN) {
                    char buf[1024] = {0};
                    int len = recv(fds[i].fd, buf, sizeof(buf), 0);
                    if (len == 0) {
                        printf("client closed\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        if (i == upper - 1) {
                            --upper;
                        }
                    } else if (len == -1) {
                        perror("recv");
                        exit(-1);
                    } else {
                        printf("recv: %s\n", buf);
                        send(fds[i].fd, buf, strlen(buf) + 1, 0);
                    }
                }
            }
        }
    }

    close(lfd);

    return 0;
}