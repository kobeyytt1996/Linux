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
#include <sys/epoll.h>


int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sAddr;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(9999);
    sAddr.sin_addr.s_addr = INADDR_ANY;
    bind(lfd, (struct sockaddr*)&sAddr, sizeof(sAddr));

    listen(lfd, 8);

    int efd = epoll_create(1);
    struct epoll_event levent;
    levent.events = EPOLLIN;
    levent.data.fd = lfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &levent);

    struct epoll_event events[1024];
    while (1) {
        int ret = epoll_wait(efd, events, 1024, -1);
        if (ret == -1) {
            perror("epoll");
            exit(-1);
        }
        printf("ret = %d\n", ret);
        if (ret > 0) {
            for (int i = 0; i < ret; ++i) {
                if (lfd == events[i].data.fd) {
                    struct sockaddr_in clientaddr;
                    socklen_t cAddrLen = sizeof(clientaddr);
                    int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &cAddrLen);

                    struct epoll_event cevent;
                    cevent.data.fd = cfd;
                    cevent.events = EPOLLIN;
                    epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &cevent);
                } else {
                    char buf[5];
                    int num = recv(events[i].data.fd, buf, sizeof(buf), 0);
                    if (num > 0) {
                        printf("server recv: %s\n", buf);
                        send(events[i].data.fd, buf, 1024, 0);
                    } else if (num == 0) {
                        printf("client closed!!!\n");
                        epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        close(events[i].data.fd);
                    } else {
                        perror("recv");
                        exit(-1);
                    }
                }
            }
        }
    }

    close(efd);
    close(lfd);
    return 0;
}