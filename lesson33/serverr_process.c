#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
       #include <sys/wait.h>
       #include <errno.h>

void *handlechld(void *args) {
    while (1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if (ret == -1) {
            break;
        } else if (ret == 0) {
            break;
        } else if (ret > 0) {
            printf("child process 回收\n");
        }
    }
}

int main() {
    struct sigaction chldaction;
    chldaction.sa_flags = 0;
    sigemptyset(&chldaction.sa_mask);
    chldaction.sa_handler = handlechld;
    sigaction(SIGCHLD, handlechld, NULL);

    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(9999);
    bind(lfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    listen(lfd, 8);

    struct sockaddr_in clientaddr;
    while (1) {
        socklen_t clientaddrlen = sizeof(clientaddr);
        int cfd = accept(lfd, (struct sockaddr*)&clientaddr, &clientaddrlen);
        if (cfd == -1) {
            if (errno == EINTR) {
                continue;
            }
        }
        pid_t pid;
        pid = fork();

        if (pid > 0) {
            continue;
        } else if (pid == 0) {
            char cliIP[16];
            inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, cliIP, sizeof(cliIP));
            unsigned short cliPort = ntohs(clientaddr.sin_port);

            printf("precess :%d, cliend ip: %s and port: %d\n", getpid(), cliIP, cliPort);

            char buf[1024];
            while (1) {
                int rdata = read(cfd, buf, sizeof(buf));
                if (rdata < 0) {
                    perror("read");
                    exit(-1);
                } else if (rdata > 0) {
                    printf("server recv: %s\n", buf);
                } else {
                    printf("client closed!!!\n");
                    break;
                }
                write(cfd, buf, rdata);
            }
            close(cfd);
            exit(0);
        }
    }

    close(lfd);

    return 0;
}