#include <sys/types.h>
       #include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main() {
    int ret = access("fifo1", F_OK);
    if (ret == -1) {
        printf("不存在\n");

        ret = mkfifo("fifo1", 0644);
        if (ret == -1) {
            perror("mkfifo");
            exit(1);
        }
    }

    int fd = open("fifo1", O_WRONLY);

    for (int i = 0; i < 100 ; ++i) {
        char buf[1024] = {0};
        sprintf(buf, "hello: %d\n", i);
        printf("write data: %s\n", buf);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    close(fd);



    return 0;
}