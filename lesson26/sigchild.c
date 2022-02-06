#include <sys/types.h>
       #include <signal.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>
       #include <sys/time.h>
       #include <stdlib.h>
       #include <sys/wait.h>


void myHandler(int num) {
    printf("signal: %d\n", num);
    while (1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if (ret > 0) {
            printf("die, pid %d\n", ret);
        } else if (ret == 0) {
            break;
        } else {
            break;
        }
    }
}

int main() {
    pid_t pid;

    // sigset_t set;
    // sigemptyset(&set);
    // sigaddset(&set, SIGCHLD);

    // sigprocmask(SIG_BLOCK, &set, NULL);
    for (int i = 0; i < 20; ++i) {
        pid = fork();
        if (pid == 0) {
            break;
        }
    }

    if (pid > 0) {
        struct sigaction act;
        act.sa_handler = myHandler;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        sigaction(SIGCHLD, &act, NULL);

        // sigprocmask(SIG_UNBLOCK, &set, NULL);

        while (1) {
            printf("parent pid: %d\n", getpid());
            sleep(1);
        }
    } else if (pid == 0) {
        printf("child pid: %d\n", getpid());
    }

    return 0;
}