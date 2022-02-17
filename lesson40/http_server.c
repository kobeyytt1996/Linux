#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void *handle_request(void *args);
void send_error(FILE *clnt_write);
char *content_type(char *file);
void send_data(FILE *clnt_write, char *file_name, char *ct);

int main() {
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr, clnt_addr;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.161.128", &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(9999);
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(serv_sock, 8);

    pthread_t pid;
    while (1) {
        int clnt_addr_len = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        pthread_create(&pid, NULL, handle_request, &clnt_sock);
        pthread_detach(pid);
    }

    close(serv_sock);

    return 0;
}

void *handle_request(void *args) {
    int clnt_sock = *(int*)args;
    char req_line[SMALL_BUF];
    FILE *clnt_read, *clnt_write;

    char method[10];
    char file_name[30];
    char ct[15];

    clnt_read = fdopen(clnt_sock, "r");
    clnt_write = fdopen(dup(clnt_sock), "w");
    fgets(req_line, SMALL_BUF, clnt_read);
    if (strstr(req_line, "HTTP/") == NULL) {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        pthread_exit(NULL);
    }

    strcpy(method, strtok(req_line, " /"));
    strcpy(file_name, strtok(NULL, " /"));
    strcpy(ct, content_type(file_name));
    if (strstr(req_line, "GET") == NULL) {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        pthread_exit(NULL);
    }
    fclose(clnt_read);
    send_data(clnt_write, file_name, ct);
}

char *content_type(char *file) {
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);

    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));

    if (!strcmp(extension, "html")) {
        return "text/html";
    } else {
        return "text/plain";
    }
}

void send_data(FILE *clnt_write, char *file_name, char *ct) {
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web\r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];
    sprintf(cnt_type, "Content-type:%s\r\n", ct);

    FILE *send_file = fopen(file_name, "r");
    if (send_file == NULL) {
        send_error(clnt_write);
        return;
    }

    fputs(protocol, clnt_write);
    fputs(server, clnt_write);
    fputs(cnt_len, clnt_write);
    fputs(cnt_type, clnt_write);

    char buf[BUF_SIZE];
    while (fgets(buf, BUF_SIZE, send_file) != NULL) {
        fputs(buf, clnt_write);
        fflush(clnt_write);
    }
    fflush(clnt_write);
    fclose(clnt_write);
}

void send_error(FILE *clnt_write) {
    char protocol[] = "HTTP/1.0 400 Bad Request Yuan";
    char server[] = "Server:Linux Web\r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/plain\r\n";

    fputs(protocol, clnt_write);
    fputs(server, clnt_write);
    fputs(cnt_len, clnt_write);
    fputs(cnt_type, clnt_write);
    fflush(clnt_write);
}

