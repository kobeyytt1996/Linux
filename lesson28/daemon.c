#include <sys/types.h>
       #include <signal.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>
       #include <sys/time.h>
       #include <stdlib.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <time.h>

void handleAlrm(int num) {
    time_t seconds = time(NULL);
    struct tm * loc = localtime(&seconds);
    // char buf[1024] = {0};
    // sprintf(buf, "%d-%d-%d %d:%d:%d", loc->tm_year, loc->tm_mon, loc->tm_mday, loc->tm_hour, loc->tm_min, loc->tm_sec);
    // printf("%s\n", buf);

    char *str = asctime(loc);
    int fd = open("time.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    write(fd, str, strlen(str));
    close(fd);
}

int main() {
    pid_t pid = fork();
    if (pid > 0) {
        exit(0);
    } else if (pid == 0) {
        struct sigaction action;
        action.sa_flags = 0;
        action.sa_handler = handleAlrm;
        sigemptyset(&action.sa_mask);
        sigaction(SIGALRM, &action, NULL);

        setsid();

        umask(022);

        chdir("/home/nowcoder");

        int fd = open("/dev/null", O_RDWR);
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        struct itimerval timer;
        timer.it_interval.tv_sec = 2;
        timer.it_interval.tv_usec = 0;
        timer.it_value.tv_sec = 2;
        timer.it_value.tv_usec = 0;

        setitimer(ITIMER_REAL, &timer, NULL);


        while (1)
        {
            sleep(10);
        }
        
    }

    return 0;
}