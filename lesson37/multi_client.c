#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {

    // 1.创建一个通信的socket
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    
    if(fd == -1) {
        perror("socket");
        exit(-1);
    }   

    
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    struct ip_mreq imr_addr;
    inet_pton(AF_INET, "239.0.0.10", &imr_addr.imr_multiaddr.s_addr);
    imr_addr.imr_interface.s_addr = INADDR_ANY;
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr_addr, sizeof(imr_addr));

    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    
    int num = 0;
    // 3.通信
    while(1) {

        char recvBuf[128];

        // 接收数据
        int num = recvfrom(fd, recvBuf, sizeof(recvBuf), 0, NULL, NULL);
        printf("server say : %s\n", recvBuf);

    }

    close(fd);
    return 0;
}