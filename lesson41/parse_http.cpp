#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 4096

enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER};

enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN};

enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION};

static const char * szret[] = {"correct\n", "wrong\n"};

LINE_STATUS parse_line(char *buffer, int &checked_index, int &read_index) {
    char temp;
    for (; checked_index < read_index; ++checked_index) {
        temp = buffer[checked_index];
        if (temp == '\r') {
            if (checked_index + 1 == read_index) {
                return LINE_OPEN;
            } else if (buffer[checked_index + 1] == '\n') {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            } else {
                return LINE_BAD;
            }
        } else if (temp == '\n') {
            if ((checked_index > 1) && buffer[checked_index-1] == '\r') {
                buffer[checked_index-1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }

    return LINE_OPEN;
}

HTTP_CODE parse_requestline(char *temp, CHECK_STATE &checkstate) {
    char *url = strpbrk(temp, " \t");
    if (!url) {
        return BAD_REQUEST;
    }
    *url++ = '\0';

    char *method = temp;
    if (strcasecmp(method, "GET") == 0) {
        printf("method GET\n");
    } else {
        return BAD_REQUEST;
    }

    url += strspn(url, " \t");
    char *version = strpbrk(url, " \t");
    if (!version) {
        return BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn(version, " \t");
    if (strcasecmp(version, "HTTP/1.1") != 0) {
        return BAD_REQUEST;
    }
    if (strncasecmp(url, "http://", 7) == 0) {
        url += 7;
        url = strchr(url, '/');
    }
    if (!url || url[0] != '/') {
        return BAD_REQUEST;
    }
    checkstate = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

HTTP_CODE parse_headers(char *temp) {
    if (temp[0] == '\0') {
        return GET_REQUEST;
    } else if (strncasecmp(temp, "Host:", 5) == 0) {
        temp += 5;
        temp += strspn(temp, " \t");
        printf("host is : %s\n", temp);
    } else {
        printf("not handle\n");
    }
    return NO_REQUEST;
}

HTTP_CODE parse_content(char *buffer, int &checked_index, CHECK_STATE &checkstate, int &read_index, int &start_line) {
    LINE_STATUS linestatus = LINE_OK;
    HTTP_CODE retcode = NO_REQUEST;
    while ((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK) {
        char *temp = buffer + start_line;
        start_line = checked_index;
        switch (checkstate)
        {
        case CHECK_STATE_REQUESTLINE:
            retcode = parse_requestline(buffer, checkstate);
            if (retcode == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            break;
        case CHECK_STATE_HEADER:
            retcode = parse_headers(buffer);
            if (retcode == BAD_REQUEST) {
                return BAD_REQUEST;
            } else if (retcode == GET_REQUEST) {
                return GET_REQUEST;
            }
            break;
        default:
            return INTERNAL_ERROR;
            break;
        }
    }

    if (linestatus == LINE_OPEN) {
        return NO_REQUEST;
    } else {
        return BAD_REQUEST;
    }
}

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(9999);
    bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    listen(lfd, 8);

    struct sockaddr_in clnaddr;
    socklen_t clnaddrlen = sizeof(clnaddr);
    int cfd = accept(lfd, (struct sockaddr*)&clnaddr, &clnaddrlen);

    char buffer[BUFFER_SIZE];
    int checked_index = 0, read_index = 0, start_line = 0;
    CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;
    int data_read = 0;
    while (1) {
        data_read = recv(cfd, buffer + read_index, BUFFER_SIZE - read_index, 0);
        if (data_read == 0) {
            printf("client close\n");
            break;
        }
        read_index += data_read;
        HTTP_CODE result = parse_content(buffer, checked_index, checkstate, read_index, start_line);
        if (result == NO_REQUEST) {
            continue;
        } else if (result == GET_REQUEST) {
            send(cfd, szret[0], strlen(szret[0]), 0);
        } else {
            send(cfd, szret[1], strlen(szret[1]), 0);
        }
    }
    close(cfd);
    close(lfd);
}