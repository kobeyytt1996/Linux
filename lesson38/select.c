#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

int main() {
    fd_set reads, temp;
    int result, str_len;
    char buf[1024];
    struct timeval timeout;

    FD_ZERO(&reads);
    FD_SET(STDIN_FILENO, &reads);

    while (1) {
        temp = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        result = select(1, &reads, NULL, NULL, &timeout);
        if (result == 0) {
            puts("time-out");
        } else if (result > 0) {
            if (FD_ISSET(STDIN_FILENO, &reads)) {
                printf("msg\n");
            }
        }
        reads = temp;
    }
}