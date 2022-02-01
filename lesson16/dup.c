#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <string.h>


int main() {
    int fd = open("a.txt", O_CREAT | O_RDWR, 0664);

    if (fd == -1) {
        perror("open");
        return -1;
    }

    int fd1 = dup(fd);
    if (fd1 == -1) {
        perror("dup");
        return -1;
    }

    printf("fd: %d, fd1: %d \n", fd, fd1);

    close(fd);

    char *str = "Hello,world";
    int ret = write(fd1, str, strlen(str));
    if (ret == -1) {
        perror("write");
        return -1;
    }
    close(fd1);
    
    return 0;
}