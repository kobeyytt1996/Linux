#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


int main() {
    char buf[1024] = {0};
    getcwd(buf, sizeof(buf));
    printf("current: %s\n", buf);

    int ret = chdir("../lesson13");
    if (ret == -1) {
        perror("chdir");
        return -1;
    }

    int fd = open("chdir.txt", O_CREAT, 0775);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    close(fd);

    char buf1[1024] = {0};
    getcwd(buf1, sizeof(buf1));
    printf("current: %s\n", buf1);

    
    return 0;
}