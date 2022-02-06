#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
       #include <string.h>
#include <sys/wait.h>

int main() {
    int fd = open("english.txt", O_RDONLY);
    int fd1 = open("cpy.txt", O_RDWR | O_CREAT, 0644);

    int len = lseek(fd, 0, SEEK_END);
    ftruncate(fd1, len);
    write(fd1, " ", 1);

    void *ptr = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    void *ptr1 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    memcpy(ptr1, ptr, len);

    munmap(ptr1, len);
    munmap(ptr, len);

    close(fd1);
    close(fd);
    return 0;
}