#include <sys/stat.h>
#include <stdio.h>

int main() {
    int ret = chmod("a.txt", 0775);
    if (ret == -1) {
        perror("chmod");
        return -1;
    }
}