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

int main() {
    int servsock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family  = AF_INET;
    servaddr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);

    connect(servsock, (sockaddr*)&servaddr, sizeof(servaddr));

    pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = servsock;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;

    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    pipe(pipefd);

    while (1) {
        poll(fds, 2, -1);
        if (fds[0].revents & POLLIN) {
            splice(fds[0].fd, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            splice(pipefd[0], NULL, servsock, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        }
        if (fds[1].revents & POLLRDHUP) {
            printf("server close\n");
            break;
        } else if (fds[1].revents & POLLIN) {
            memset(read_buf, '\0', BUFFER_SIZE);
            recv(servsock, read_buf, BUFFER_SIZE - 1, 0);
            printf("%s\n", read_buf);
        }
    }

    close(servsock);
}