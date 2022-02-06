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
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGQUIT);

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    int num = 0;
    while (1)
    {
        ++num;
        sigset_t pendingset;
        sigemptyset(&pendingset);
        sigpending(&pendingset);

        for (int i = 1; i <= 31; ++i) {
            int ret = sigismember(&pendingset, i);
            if (ret == 1) {
                printf("1");
            } else if (ret == 0) {
                printf("0");
            } else {
                perror("sigismember");
                exit(-1);
            }
        }

        if (num == 10) {
            sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        }

        printf("\n");
        sleep(1);
    }
    
    return 0;
}