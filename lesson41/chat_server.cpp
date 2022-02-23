#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

#define BUFFER_SIZE 64
#define _GNU_SOURCE 1
#define USER_LIMIT 5
#define FD_LIMIT 65535

struct client_data {
    sockaddr_in address;
    char *write_buf;
    char buf[BUFFER_SIZE];
};

int setnonblocking(int fd) {
    int old_opt = fcntl(fd, F_GETFL);
    int new_opt = old_opt | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_opt);
    return old_opt;
}

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(9999);
    bind(lfd, (sockaddr*)&servaddr, sizeof(servaddr));

    listen(lfd, 8);

    client_data *users = new client_data[FD_LIMIT];
    pollfd fds[USER_LIMIT + 1];
    int user_counter = 0;
    for (int i = 0; i <= USER_LIMIT; ++i) {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = lfd;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    while (1) {
        poll(fds, user_counter + 1, -1);
        for (int i = 0; i <= user_counter; ++i) {
            if (fds[i].fd == lfd && (fds[i].revents & POLLIN)) {
                sockaddr_in clnaddr;
                socklen_t clnaddrlen = sizeof(clnaddr);
                int connfd = accept(lfd, (sockaddr*)&clnaddr, &clnaddrlen);
                if (user_counter >= USER_LIMIT) {
                    const char *info = "too many users\n";
                    send(connfd, info, strlen(info), 0);
                    close(connfd);
                    continue;
                }
                ++user_counter;
                users[connfd].address = clnaddr;
                setnonblocking(connfd);
                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLERR | POLLRDHUP;
                fds[user_counter].revents = 0;
            } else if (fds[i].revents & POLLERR) {
                char errors[100];
                socklen_t length = sizeof(errors);
                getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &length);
                continue;
            } else if (fds[i].revents & POLLRDHUP) {
                close(fds[i].fd);
                fds[i] = fds[user_counter--];
                i--;
            } else if (fds[i].revents & POLLIN) {
                int connfd = fds[i].fd;
                memset(users[connfd].buf, '\0', BUFFER_SIZE);
                int ret = recv(connfd, users[connfd].buf, BUFFER_SIZE, 0);
                if (ret > 0) {
                    for (int j = 1; j < user_counter + 1; ++j) {
                        if (j == i) {
                            continue;
                        }
                        fds[j].events &= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                }
                continue;
            } else if (fds[i].revents & POLLOUT) {
                int connfd = fds[i].fd;
                if (!users[connfd].write_buf) {
                    continue;
                }
                send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
                users[connfd].write_buf = nullptr;
                fds[i].events &= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }

    delete [] users;
    close(lfd);
    return 0;
}