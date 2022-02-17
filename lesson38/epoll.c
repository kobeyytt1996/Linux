#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int epoll_fd = epoll_create(1);
    
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(serv_sock, 8);

    const int EVENT_SIZE = 100;
    struct epoll_event *ep_events = malloc(sizeof(struct epoll_event) * EVENT_SIZE);

    struct epoll_event ep_event;
    ep_event.data.fd = serv_sock;
    ep_event.events = EPOLLIN; 
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_sock, &ep_event);

    while (1) {
        int event_cnt = epoll_wait(epoll_fd, ep_events, EVENT_SIZE, -1);
        for (int i = 0; i < event_cnt; ++i) {
            
        }
    }
}