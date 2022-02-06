#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
       #include <string.h>
#include <sys/wait.h>

int main() {
    int len = 4096;
    void *ptr = mmap(NULL, len, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 0;
    }

    pid_t pid = fork();
    if (pid > 0) {
        strcpy((char *)ptr, "hello");
        wait(NULL);
    } else if (pid == 0) {
        sleep(1);
        printf("%s\n, page size: %ld", (char*)ptr, sysconf(_SC_PAGE_SIZE));
    }

    munmap(ptr, len);

    return 0;
}