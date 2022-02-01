#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <string.h>


int main() {
    int fd = open("1.txt", O_RDWR | O_CREAT, 0664);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int fd1 = open("2.txt", O_RDWR | O_CREAT, 0664);
    if (fd1 == -1) {
        perror("open");
        return -1;
    }

    printf("fd: %d, fd1: %d\n", fd, fd1);

    int fd2 = dup2(fd, fd1);
    if (fd2 == -1) {
        perror("dup2");
        return -1;
    }

    char *str = "hello";
    write(fd1, str, strlen(str));
    str[1] = 'a';

    close(fd);
    close(fd1);

    return 0;
}