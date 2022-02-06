#include <sys/types.h>
       #include <signal.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>
       #include <sys/time.h>
       #include <stdlib.h>

int main() {
    sigset_t sigset;

    sigemptyset(&sigset);

    int ret = sigismember(&sigset, SIGINT);
    if (ret == 0) {
        printf("SIGINT不阻塞\n");
    } else if (ret == 1) {
        printf("SIGINT阻塞\n");
    }

    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGQUIT);

    ret = sigismember(&sigset, SIGINT);
    if (ret == 0) {
        printf("SIGINT不阻塞\n");
    } else if (ret == 1) {
        printf("SIGINT阻塞\n");
    }

    sigdelset(&sigset, SIGQUIT);

    return 0;
}