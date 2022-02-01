#include <sys/types.h>
       #include <unistd.h>
       #include <stdio.h>
       #include <string.h>

int main() {
    int num = 10;
    pid_t pid = fork();

    if (pid > 0) {
        printf("pid: %d\n", pid);
        printf("parent: pid: %d ppid: %d\n", getpid(), getppid());
    } else if (pid == 0) {
        printf("child: pid: %d ppid: %d\n", getpid(), getppid());
    } else {
        perror("fork");
        return -1;
    }

    for (int i = 0; i < 3; ++i) {
        printf("i: %d\n", i);
        sleep(1);
    }

    return 0;
}