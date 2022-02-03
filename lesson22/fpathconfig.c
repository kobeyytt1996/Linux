#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int pipefd[2] = {0};
    pipe(pipefd);

    long length = fpathconf(pipefd[0], _PC_PIPE_BUF);
    printf("pipe: %ld\n", length);

    return 0;
}