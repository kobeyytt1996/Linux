#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


int main() {
    struct stat aStat;
    int ret = stat("a.txt", &aStat);
    if (ret == -1) {
        perror("stat");
        return -1;
    }
    printf("size: %ld\n", aStat.st_size);
    
}