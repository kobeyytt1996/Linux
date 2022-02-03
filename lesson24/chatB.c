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
        printf("fifo1不存在\n");
        ret = mkfifo("fifo1", 0644);
        if (ret == -1) {
            perror("mkfifo");
            exit(-1);
        }
    }

    ret = access("fifo2", F_OK);
    if (ret == -1) {
        printf("fifo2不存在\n");
        ret = mkfifo("fifo2", 0644);
        if (ret == -1) {
            perror("mkfifo");
            exit(-1);
        }
    }

    int fdr = open("fifo1", O_RDONLY);
    printf("打开fifo1成功，等待读取……\n");
    int fdw = open("fifo2", O_WRONLY);
    printf("打开fifo2成功，等待写入……\n");

    char input[128] = {0};
    pid_t pid = fork();
    if (pid > 0) {
        while (1) {

        memset(input, 0, 128);
        int len = read(fdr, input, 128);
        if (len == 0) {
            printf("写端结束……\n");
            break;
        }
        printf("buf: %s\n", input);

    }
    } else if (pid == 0) {
        while (1) {
        memset(input, 0, 128);

        fgets(input, 128, stdin);

        write(fdw, input, strlen(input));
    }
    }

    close(fdr);
    close(fdw);

    return 0;
}