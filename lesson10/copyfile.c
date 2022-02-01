#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    int fd = open("english.txt", O_RDONLY);
    int fd1 = open("english1.txt", O_WRONLY | O_CREAT, 0664);
    char buf[1024] = {0};
    int readNum = 0;
    if (fd != -1 && fd1 != -1) {
        while ((readNum = read(fd, buf, sizeof(buf))) > 0) {
            write(fd1, buf, readNum);
        }
    } else {
        perror("yuan");
    }
    close(fd);
    close(fd1);
}