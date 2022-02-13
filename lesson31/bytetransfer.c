#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
    unsigned short us = 0x0102;
    unsigned short us1 = htons(us);
    printf("%x\n", us1);

    char buf[4] = {192, 168, 1, 100};
    int num = *(int*)buf;
    int sum = htonl(num);
    unsigned char *p = (unsigned char*)&sum;

    printf("%d %d %d %d\n", p[0], p[1], p[2], p[3]);
    return 0;
}