#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
    int ret = truncate("b.txt", 5);
    if (ret == -1) {
        perror("trun");
        return -1;
    }
    return 0;
}