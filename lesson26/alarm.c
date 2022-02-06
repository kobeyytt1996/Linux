#include <sys/types.h>
       #include <signal.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>

int main() {
    int seconds = alarm(5);
    printf("seconds: %d\n", seconds);

    sleep(2);
    seconds = alarm(2);
    printf("seconds: %d\n", seconds);

    while (1)
    {
        
    }
    
    return 0;
}