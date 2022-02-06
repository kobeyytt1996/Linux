#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
       #include <string.h>
#include <sys/wait.h>


int main() {
    int fd = open("test.txt", O_RDWR);
    int size = lseek(fd, 0, SEEK_END);

    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    pid_t pid = fork();

    if (pid > 0) {
        
        // wait(NULL);

        char buf[64] = {0};
        strcpy(buf, (char*)ptr);
        printf("read data: %s\n", buf);
    } else if (pid == 0) {
        // strcpy((char*)ptr, "nihao, dad");
    }

    munmap(ptr, size);
    return 0;
}