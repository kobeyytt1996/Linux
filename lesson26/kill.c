#include <sys/types.h>
       #include <signal.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>

int main() {
    pid_t pid = fork();
    if (pid > 0) {
        kill(pid, SIGINT);
    } else if (pid == 0) {
        
    }

    return 0;
}