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
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define FD_LIMIT 65536
#define MAX_EVENT_NUMBER 1024
#define USER_LIMIT 5
#define BUFFER_SIZE 1024
#define PROCESS_LIMIT 65536

struct client_data {
    sockaddr_in address;
    int connfd;
    pid_t pid;
    int pipefd[2];
};

static const char *shm_name = "/my_shm";
int sig_pipefd[2];
int epollfd;
int listenfd;
int shmfd;
char *share_mem = nullptr;
client_data *users = nullptr;
int *sub_process = nullptr;
int user_count = 0;
bool stop_child = false;

int setnonblocking(int fd) {
    int old_opt = fcntl(fd, F_GETFL);
    int new_opt = old_opt | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_opt);
    return old_opt;
}

void addfd(int epollfd, int fd) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void sig_handler(int sig) {
    int save_errno = errno;
    send(sig_pipefd[1], &sig, 1, 0);
    errno = save_errno;
}

void addsig(int sig, void(*handler)(int), bool restart = true) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
}

void del_resource() {
    close(sig_pipefd[0]);
    close(sig_pipefd[1]);
    close(listenfd);
    close(epollfd);
    shm_unlink(shm_name);
    delete [] users;
    delete [] sub_process;
}

void child_tem_handler(int sig) {
    stop_child = true;
}

int run_child(int idx, client_data *users, char *share_mem) {
    epoll_event events[MAX_EVENT_NUMBER];
    int child_epollfd = epoll_create(5);
    int connfd = users[idx].connfd;
    addfd(child_epollfd, connfd);
    int pipefd = users[idx].pipefd[1];
    addfd(child_epollfd, pipefd);
    int ret;
    addsig(SIGTERM, child_tem_handler, false);

    while (!stop_child) {
        int number = epoll_wait(child_epollfd, events, MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == connfd && (events[i].events & EPOLLIN)) {
                memset(share_mem + idx * BUFFER_SIZE, '\0', BUFFER_SIZE);
                ret = recv(sockfd, share_mem + idx * BUFFER_SIZE, BUFFER_SIZE - 1, 0);
                if (ret == 0) {
                    stop_child = true;
                } else {
                    send(pipefd, (char*)&idx, sizeof(idx), 0);
                }
            } else if (sockfd == pipefd && (events[i].events & EPOLLIN)) {
                int client = 0;
                ret = recv(pipefd, &client, sizeof(client), 0);
                if (ret == 0) {
                    stop_child = true;
                } else {
                    send(connfd, share_mem + client * BUFFER_SIZE, BUFFER_SIZE, 0);
                }
            }
        }
    }

    close(pipefd);
    close(connfd);
    close(child_epollfd);
    return 0;
}

int main() {
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(9999);
    bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    listen(listenfd, 8);

    user_count = 0;
    users = new client_data[USER_LIMIT + 1];
    sub_process = new int[PROCESS_LIMIT];
    for (int i = 0; i < PROCESS_LIMIT; ++i) {
        sub_process[i] = -1;
    }

    epoll_event events[MAX_EVENT_NUMBER];
    epollfd = epoll_create(8);
    addfd(epollfd, listenfd);

    socketpair(AF_UNIX, SOCK_STREAM, 0, sig_pipefd);
    setnonblocking(sig_pipefd[1]);
    addfd(epollfd, sig_pipefd[0]);

    addsig(SIGCHLD, sig_handler);
    addsig(SIGPIPE, SIG_IGN);
    bool stop_server = false;
    bool terminate = false;

    shmfd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shmfd, USER_LIMIT * BUFFER_SIZE);

    share_mem = (char*)mmap(NULL, USER_LIMIT * BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    close(shmfd);

    while (!stop_server) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                int connfd = accept(listenfd, NULL, NULL);
                if (user_count > USER_LIMIT) {
                    send(connfd, "too many users\n", 15, 0);
                    close(connfd);
                    continue;
                }
                users[user_count].connfd = connfd;
                socketpair(AF_INET, SOCK_STREAM, 0, users[user_count].pipefd);
                pid_t pid = fork();
                if (pid == 0) {
                    close(epollfd);
                    close(listenfd);
                    close(users[user_count].pipefd[0]);
                    close(sig_pipefd[0]);
                    close(sig_pipefd[1]);
                    run_child(user_count, users, share_mem);
                    munmap(share_mem, USER_LIMIT * BUFFER_SIZE);
                    exit(0);
                } else {
                    close(connfd);
                    close(users[user_count].pipefd[1]);
                    addfd(epollfd, users[user_count].pipefd[0]);
                    users[user_count].pid = pid;
                    sub_process[pid] = user_count;
                    ++user_count;
                }
            } else if (sockfd == sig_pipefd[0] && (events[i].events & EPOLLIN)) {
                int sig;
                char signals[1024];
                int ret = recv(sig_pipefd[0], signals, 1024, 0);
                if (ret > 0) {
                    for (int i = 0; i < ret; ++i) {
                        switch (signals[i]) {
                            case SIGCHLD: {
                                pid_t pid;
                                int stat;
                                while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
                                    int del_user = sub_process[pid];
                                    sub_process[pid] = -1;
                                    if (del_user < 0 || del_user > USER_LIMIT) {
                                        continue;
                                    }
                                    epoll_ctl(epollfd, EPOLL_CTL_DEL, users[del_user].pipefd[0], NULL);
                                    close(users[del_user].pipefd[0]);
                                    users[del_user] = users[--user_count];
                                    sub_process[users[del_user].pid] = del_user;
                                }
                                if (terminate && user_count == 0) {
                                    stop_server = true;
                                }
                                break;
                            }
                            case SIGTERM:
                            case SIGINT: {
                                if (user_count == 0) {
                                    stop_server = true;
                                    break;
                                }
                                for (int i = 0; i < user_count; ++i) {
                                    kill(users[i].pid, SIGTERM);
                                }
                                terminate = true;
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                    }
                }
            } else if (events[i].events & EPOLLIN) {
                int child;
                recv(sockfd, &child, sizeof(child), 0);
                for (int j = 0; j < user_count; ++j) {
                    if (j != child) {
                        send(users[j].pipefd[0], &child, sizeof(child), 0);
                    }
                }
            }
        }
    }


    del_resource();
    return 0;
}