#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
    int ret = mkdir("aaa", 0755);

    if (ret == -1) {
        perror("mkdir");
        return -1;
    }
    
    return 0;
}