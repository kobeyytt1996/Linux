#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    int fd = open("hello.txt", O_RDWR);
    if (fd != -1) {
        int ret = lseek(fd, 10, SEEK_END);
        if (ret == -1) {
            perror("lseek");
        }
        write(fd, "\0", 1);
    }
    close(fd);

    return 0;
}