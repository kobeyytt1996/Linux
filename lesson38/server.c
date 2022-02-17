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
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUF_SIZE 100

void read_childproc(int sig);

int main(int argc, char **argv) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    int fds[2];

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];

    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = read_childproc;
    state = sigaction(SIGCHLD, &act, NULL);

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(-1);
    }

    listen(serv_sock, 8);

    pipe(fds);
    pid = fork();
    if (pid == 0) {
        FILE * fp = fopen("echo.txt", "wt");
        char msgbuf[BUF_SIZE];
        int len;
        for (int i = 0; i < 10; ++i) {
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite(msgbuf, 1, len, fp);
        }
        fclose(fp);
        exit(0);
    }

    while (1)
    {
        adr_sz = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)clnt_addr, &adr_sz);
        pid = fork();
        if (pid == 0) {
            close(serv_sock);
            while ((strlen = recv(clnt_sock, buf, BUF_SIZE))) {
                
            }
        }
    }
    

    return 0;
}

void read_childproc(int sig) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
}
