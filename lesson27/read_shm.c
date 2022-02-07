#include <sys/ipc.h>
       #include <sys/shm.h>
       #include <stdio.h>
       #include <string.h>

int main() {
    int shmid = shmget(100, 4096, 0644);

    void *ptr = shmat(shmid, NULL, 0);

    printf("%s\n", (char*)ptr);

    printf("任意键继续\n");
    getchar();

    shmdt(ptr);

    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}