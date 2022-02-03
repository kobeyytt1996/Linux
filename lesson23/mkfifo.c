#include <sys/types.h>
       #include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
    

    return 0;
}