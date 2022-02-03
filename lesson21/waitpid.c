#include <sys/types.h>
       #include <sys/wait.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>
       #include <stdlib.h>


int main() {

    pid_t pid = 1;
    for (int i = 0; i < 5 && pid > 0; ++i) {
        pid = fork();
    }

    if (pid > 0) {
        while (1)
        {
            printf("parent pid: %d\n", getpid());
            // int ret = wait(NULL);
            int st;
            int ret = waitpid(-1, &st, WNOHANG);

            if (ret == -1) {
                break;
            }

            if (!ret) {
                continue;
            }

            if (ret > 0 && WIFEXITED(st)) {
                printf("状态码: %d, st: %d\n", WEXITSTATUS(st), st);
            }

            if (ret > 0 && WIFSIGNALED(st)) {
                printf("信号: %d\n", WTERMSIG(st));
            }

            printf("child die, pid: %d\n", ret);
            sleep(1);
        }
        
    } else {
        while (1)
        {
            // printf("child pid: %d\n", getpid());
            sleep(1);
        }

        exit(1);
        
    }


    return 0;
}