#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in sAddr;
    sAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &sAddr.sin_addr.s_addr);
    sAddr.sin_port = htons(9999);

    while (1)
    {
        sendto(udpfd, "hello", 6, 0, (struct sockaddr*)&sAddr, sizeof(sAddr));
    
    char buf[1024];
    int ret = recvfrom(udpfd, buf, sizeof(buf), 0, NULL, NULL);
    
    printf("server say; %s", buf);
    }
    

    return 0;
}