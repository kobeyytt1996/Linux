#include <arpa/inet.h>
#include <stdio.h>

int main() {
    char buf[] = "192.128.1.4";

    struct in_addr addr;

    inet_pton(AF_INET, buf, &addr);

    unsigned char * p = (unsigned char *)&addr;

    printf("%d\n", *p);

    char ip[17] = {0};

    inet_ntop(AF_INET, &addr, ip, 16);

    printf("%s\n", ip);

    return 0;
}