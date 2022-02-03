#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int pipefd[2] = {0};
    pipe(pipefd);

    pid_t pid = fork();

    if (pid > 0) {
        close(pipefd[1]);

        char buf[1024] = {0};
        int len = 0;
        sleep(1);
        while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) != 0)
        {
            printf("%s", buf);
            // bzero(buf, sizeof(buf));
            memset(buf, 0, 1024);
        }
        
        wait(NULL);
    } else if (pid == 0) {
        close(pipefd[0]);

        fcntl(pipefd[1], F_SETFL, O_NONBLOCK);
        dup2(pipefd[1], STDOUT_FILENO);
        fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);
        execlp("ps", "ps", "aux", NULL);
    }
}