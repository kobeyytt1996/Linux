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

    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));  

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET, "192.168.161.255", &addr.sin_addr.s_addr);

    

    // 3.通信
    int num = 0;
    while(1) {
        char sendBuf[128];
        sprintf(sendBuf, "broadcast: %d\n", num++);

        // 发送数据
        sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr *)&addr, sizeof(addr));
        printf("广播数据: %s\n", sendBuf);

        sleep(1);

    }

    close(fd);
    return 0;
}