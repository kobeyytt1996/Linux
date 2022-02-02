#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("cd", "ls", "~", NULL);
        perror("execl");

        printf("Child process");
    } else if (pid > 0) {
        printf("parent: %d\n", getpid());
    }

    return 0;
}