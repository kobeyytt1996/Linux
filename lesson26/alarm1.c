#include <sys/types.h>
       #include <signal.h>
       #include <stdio.h>
       #include <string.h>
       #include <unistd.h>

int main() {
    int i = 0;
    alarm(1);

    while (1)
    {
        printf("%d\n", ++i);
    }
    
}