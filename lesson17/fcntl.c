#include <unistd.h>
       #include <fcntl.h>
       #include <stdio.h>
#include <string.h>

int main() {
    // int fd = open("1.txt", O_RDONLY);
    // int fd1 = fcntl(fd, F_DUPFD);

    int fd = open("1.txt", O_WRONLY);

    int flag = fcntl(fd, F_GETFL);
    flag |= O_APPEND;
    int ret = fcntl(fd, F_SETFL, flag);
    if (ret == -1) {
        perror("fcntl");
        return -1;
    }

    char * str = "yuan";
    write(fd, str, strlen(str));

    close(fd);

    return 0;
}